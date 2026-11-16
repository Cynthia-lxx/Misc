import { apiGet, apiPost } from "../api.js";
import { query } from "../router.js";

function esc(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[c]));
}

/** 不需要用户输入的参数种类（出参 / 自动长度 / 恒 NULL 指针）。 */
const AUTO_PARAM = (p) => p.hidden
  || p.type.startsWith("out:")
  || ["buf", "bin", "outint", "ptr"].includes(p.type);

const hexOf = (v) => `0x${Number(v).toString(16).toUpperCase()}`; // eslint-disable-line
/* ================= 编排 ⇄ 汇编：同一程序的两种形态（自动同步） =================
 * programToAsm：积木树 → 真实 x86-64（Intel 语法，Windows x64 调用约定），永远成功；
 * asmToProgram：汇编文本 → 积木树，仅识别本工具生成的规范子集，
 *   超出子集 → {ok:false, err:{ln,line,msg}}，前端据此「脱钩」，改回可解析状态即自动复联。
 *
 * 映射约定：
 *   $N / $N.ret / $N.ret.value        → mov reg, [slot_N]
 *   $N.out.p（结构 / outint）          → [slot_N_out_p]
 *   $N.out.p.len_chars / .size        → [slot_N_out_p_len] / [slot_N_out_p_size]
 *   buf / bin 出参（作指针）           → lea reg, [buf_N_p]
 *   字符串字面量                       → lea reg, [str_K]（.data：du=UTF-16LE；含引号/控制字符退化为 dw 码元表）
 *   null / 空                          → xor reg, reg
 *   表达式                             → mov rax, … / push rax / mov r10, rax / pop rax / add|sub|imul|idiv r10
 *   repeat                             → push rbx + 计数 + .Lk_loop/test/je + .Lk_next/dec/jmp + .Lk_end/pop rbx
 *   if（&& ||、== != < <= > >=）       → cmp/test + jcc 至 .Lk_else / .Lk_chk_i / .Lk_end / .Lk_then
 *   break / continue                   → jmp .Lk_end / .Lk_next（k = 最近循环）
 */

const ASM_ARG_REGS = ["rcx", "rdx", "r8", "r9"];
const ASM_ZERO = { t: "imm", v: 0 };
const asmIdent = (s) => String(s).replace(/[^0-9A-Za-z_]/g, "_") || "x";
const asmIsInt = (s) => /^-?(0x[0-9A-Fa-f]+|\d+)$/.test(String(s).trim());

function asmInvertCmp(op) {
  return { "==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">" }[op];
}

/* ---- 表达式 tokenizer / 递归下降（$N 引用、整数、+ - * / %、比较、&& ||、括号） ---- */
function asmTokenize(src) {
  const toks = [];
  let i = 0;
  while (i < src.length) {
    const c = src[i];
    if (/\s/.test(c)) { i++; continue; }
    const two = src.slice(i, i + 2);
    if (["==", "!=", "<=", ">=", "&&", "||"].includes(two)) { toks.push(two); i += 2; continue; }
    if ("+-*/%()<>".includes(c)) { toks.push(c); i++; continue; }
    if (c === "$" || /[0-9A-Za-z_]/.test(c)) {
      let j = c === "$" ? i + 1 : i;
      if (c === "$" && j >= src.length) return null;
      while (j < src.length && /[0-9A-Za-z_.]/.test(src[j])) j++;
      toks.push(src.slice(i, j));
      i = j;
      continue;
    }
    return null;
  }
  return toks;
}

function asmParseExpr(toks, refLabel) {
  let pos = 0;
  const peek = () => toks[pos];
  const CMP_OPS = ["==", "!=", "<", "<=", ">", ">="];
  function parseOr() {
    let l = parseAnd();
    if (!l) return null;
    while (peek() === "||") { pos++; const r = parseAnd(); if (!r) return null; l = { t: "or", l, r }; }
    return l;
  }
  function parseAnd() {
    let l = parseCmp();
    if (!l) return null;
    while (peek() === "&&") { pos++; const r = parseCmp(); if (!r) return null; l = { t: "and", l, r }; }
    return l;
  }
  function parseCmp() {
    const l = parseAdd();
    if (!l) return null;
    if (CMP_OPS.includes(peek())) {
      const op = peek(); pos++;
      const r = parseAdd();
      if (!r) return null;
      return { t: "cmp", op, l, r };
    }
    return l;
  }
  function parseAdd() {
    let l = parseMul();
    if (!l) return null;
    while (peek() === "+" || peek() === "-") {
      const op = peek(); pos++;
      const r = parseMul();
      if (!r) return null;
      l = { t: "bin", op, l, r };
    }
    return l;
  }
  function parseMul() {
    let l = parseUnary();
    if (!l) return null;
    while (peek() === "*" || peek() === "/" || peek() === "%") {
      const op = peek(); pos++;
      const r = parseUnary();
      if (!r) return null;
      l = { t: "bin", op, l, r };
    }
    return l;
  }
  function parseUnary() {
    if (peek() === "-") { pos++; const e = parseUnary(); return e && { t: "neg", e }; }
    if (peek() === "(") {
      pos++;
      const e = parseOr();
      if (!e || peek() !== ")") return null;
      pos++;
      return e;
    }
    const t = peek();
    if (t === undefined) return null;
    pos++;
    if (t.startsWith("$")) {
      const label = refLabel(t);
      return label ? { t: "ref", label } : null;
    }
    if (asmIsInt(t)) return { t: "imm", v: Number(t) };
    return null;
  }
  const ast = parseOr();
  if (!ast || pos !== toks.length) return null;
  return ast;
}

/** 条件表达式 → {disjs: [[atom,…],…]}（disj 间 ||，disj 内 &&；非比较表达式 → (e)!=0）。 */
function asmParseCond(cond, refLabel) {
  const toks = asmTokenize(String(cond ?? ""));
  if (!toks || !toks.length) return null;
  const ast = asmParseExpr(toks, refLabel);
  if (!ast) return null;
  const andAtoms = (e) => {
    const atoms = [];
    const flat = (x) => {
      if (x.t === "and") { flat(x.l); flat(x.r); return; }
      if (x.t === "cmp") { atoms.push({ op: x.op, l: x.l, r: x.r }); return; }
      atoms.push({ op: "!=", l: x, r: { t: "imm", v: 0 } });
    };
    flat(e);
    return atoms;
  };
  if (ast.t === "or") {
    const disjs = [];
    const flat = (e) => (e.t === "or" ? (flat(e.l), flat(e.r)) : disjs.push(e));
    flat(ast);
    return { disjs: disjs.map(andAtoms) };
  }
  return { disjs: [andAtoms(ast)] };
}

/* ---- 代码生成：积木树 → x86-64 文本（永远成功） ---- */
function programToAsm(prog, specOf) {
  const code = [];
  const dataLines = [];
  const strings = [];
  const strKey = new Map();
  const loopStack = [];
  let lblN = 0;
  const emit = (s) => code.push(s);

  // 预扫描：槽位编号（DFS 序，与后端 $N 编号一致）+ out 参数标签（支持前向引用）
  const callByN = new Map();
  (function prescan(nodes) {
    for (const nd of nodes || []) {
      if (nd.kind === "call" || !nd.kind) {
        const n = callByN.size + 1;
        const spec = specOf(nd.api);
        callByN.set(n, { node: nd, n, params: spec ? spec.params : [] });
      }
      prescan(nd.body); prescan(nd.then); prescan(nd.else);
    }
  })(prog);
  const callQueue = [...callByN.values()];

  function strLabel(value) {
    value = String(value);
    if (strKey.has(value)) return strKey.get(value);
    const key = `str_${strings.length + 1}`;
    strings.push({ key, value });
    strKey.set(value, key);
    return key;
  }

  // 引用 → 汇编标签（依据预扫描的参数类型）；无法表达 → null
  function refLabel(ref) {
    const parts = String(ref).slice(1).split(".");
    const n = parseInt(parts[0], 10);
    const call = callByN.get(n);
    if (!call) return null;
    const rest = parts.slice(1).filter((s) => s !== "ret" && s !== "value");
    if (!rest.length) return `slot_${n}`;
    if (rest[0] !== "out") return null;
    const pname = rest[1];
    const p = pname ? call.params.find((q) => q.name === pname) : null;
    if (!p) return `slot_${n}_out_${asmIdent(rest.slice(1).join("_"))}`;
    const leaf = rest[2];
    if (p.type === "buf") return leaf === "len_chars" ? `slot_${n}_out_${asmIdent(pname)}_len` : `buf_${n}_${asmIdent(pname)}`;
    if (p.type === "bin") return leaf === "size" ? `slot_${n}_out_${asmIdent(pname)}_size` : `buf_${n}_${asmIdent(pname)}`;
    return `slot_${n}_out_${asmIdent(pname)}`;
  }

  // 表达式 AST → 指令（结果进 reg）；不支持的比较/逻辑节点返回 false
  function genExpr(e, reg) {
    if (!e) return false;
    if (e.t === "imm") { emit(`mov ${reg}, ${e.v}`); return true; }
    if (e.t === "ref") { emit(`mov ${reg}, [${e.label}]`); return true; }
    if (e.t === "neg") { if (!genExpr(e.e, reg)) return false; emit(`neg ${reg}`); return true; }
    if (e.t === "cmp" || e.t === "and" || e.t === "or") return false;
    if (!genExpr(e.l, "rax")) return false;
    emit("push rax");
    if (!genExpr(e.r, "rax")) { emit("pop rax"); return false; }
    emit("mov r10, rax");
    emit("pop rax");
    if (e.op === "+") emit("add rax, r10");
    else if (e.op === "-") emit("sub rax, r10");
    else if (e.op === "*") emit("imul rax, r10");
    else if (e.op === "/") { emit("cqo"); emit("idiv r10"); }
    else if (e.op === "%") { emit("cqo"); emit("idiv r10"); emit("mov rax, rdx"); }
    else return false;
    if (reg !== "rax") emit(`mov ${reg}, rax`);
    return true;
  }

  // 参数值 → 指令（结果进 reg）
  function emitValue(v, reg) {
    if (v == null || String(v).trim() === "") { emit(`xor ${reg}, ${reg}`); return; }
    const s = String(v).trim();
    if (asmIsInt(s)) { emit(`mov ${reg}, ${Number(s)}`); return; }
    if (s.startsWith("$") || /[+\-*/%()]/.test(s)) {
      const toks = asmTokenize(s);
      const ast = toks && asmParseExpr(toks, refLabel);
      if (ast && genExpr(ast, reg)) return;
      emit("; [!] 该引用/表达式无法在汇编中表达，按 0 处理");
      emit(`xor ${reg}, ${reg}`);
      return;
    }
    emit(`lea ${reg}, [${strLabel(s)}]`);
  }

  const JUMP_TRUE = { "==": "je", "!=": "jne", "<": "jl", "<=": "jle", ">": "jg", ">=": "jge" };
  const JUMP_FALSE = { "==": "jne", "!=": "je", "<": "jge", "<=": "jg", ">": "jle", ">=": "jl" };

  function emitCall(node) {
    const m = callQueue.shift();
    if (!m) return;
    const spec = specOf(node.api);
    const params = spec ? spec.params : [];
    emit(`; [${m.n}] ${node.sig || `${node.api}()`}`);
    for (let i = 0; i < Math.min(params.length, 4); i++) {
      const p = params[i];
      const reg = ASM_ARG_REGS[i];
      const v = node.args[p.name] !== undefined ? node.args[p.name] : (p.default ?? null);
      if (p.type.startsWith("out:")) emit(`lea ${reg}, [slot_${m.n}_out_${asmIdent(p.name)}]`);
      else if (p.type === "buf" || p.type === "bin") emit(`lea ${reg}, [buf_${m.n}_${asmIdent(p.name)}]`);
      else if (p.type === "ptr") emit(`xor ${reg}, ${reg}`);
      else emitValue(v, reg);
    }
    let frameSize = 0;
    if (params.length > 4) {
      frameSize = Math.ceil((0x20 + 8 * (params.length - 4)) / 16) * 16;
      emit(`sub rsp, 0x${frameSize.toString(16)}`);
      for (let i = 4; i < params.length; i++) {
        const p = params[i];
        const v = node.args[p.name] !== undefined ? node.args[p.name] : (p.default ?? null);
        if (p.type.startsWith("out:")) emit(`lea rax, [slot_${m.n}_out_${asmIdent(p.name)}]`);
        else if (p.type === "buf" || p.type === "bin") emit(`lea rax, [buf_${m.n}_${asmIdent(p.name)}]`);
        else if (p.type === "ptr") emit("xor eax, eax");
        else emitValue(v, "rax");
        emit(`mov qword [rsp+0x${(0x20 + 8 * (i - 4)).toString(16)}], rax`);
      }
    }
    emit(`call ${node.api}`);
    if (frameSize) emit(`add rsp, 0x${frameSize.toString(16)}`);
    emit(`mov [slot_${m.n}], rax`);
    // 数据段：返回槽 + 出参槽/缓冲区
    dataLines.push(`slot_${m.n}: dq 0   ; $${m.n}.ret.value`);
    for (const p of params) {
      if (p.type.startsWith("out:")) dataLines.push(`slot_${m.n}_out_${asmIdent(p.name)}: dq 0   ; $${m.n}.out.${p.name}`);
      else if (p.type === "buf") dataLines.push(`slot_${m.n}_out_${asmIdent(p.name)}_len: dq 0   ; $${m.n}.out.${p.name}.len_chars`,
        `buf_${m.n}_${asmIdent(p.name)}: times 512 db 0   ; $${m.n}.out.${p.name}（wchar 缓冲区指针）`);
      else if (p.type === "bin") dataLines.push(`slot_${m.n}_out_${asmIdent(p.name)}_size: dq 0   ; $${m.n}.out.${p.name}.size`,
        `buf_${m.n}_${asmIdent(p.name)}: times 256 db 0   ; $${m.n}.out.${p.name}（字节缓冲区指针）`);
    }
  }

  function emitRepeat(node) {
    const k = ++lblN;
    loopStack.push(k);
    emit(`; [块] repeat ${node.times ?? "?"}`);
    emit("push rbx");
    emitValue(node.times ?? 3, "rbx");
    emit(`.L${k}_loop:`);
    emit("test rbx, rbx");
    emit(`je .L${k}_end`);
    emitNodes(node.body);
    emit(`.L${k}_next:`);
    emit("dec rbx");
    emit(`jmp .L${k}_loop`);
    emit(`.L${k}_end:`);
    emit("pop rbx");
    loopStack.pop();
  }

  function emitAtom(a, target, jumpOnTrue) {
    if (!genExpr(a.l, "rax")) {
      emit("; [!] 条件原子无法表达，按恒假处理");
      emit("xor rax, rax");
      emit("test rax, rax");
      emit(`${(jumpOnTrue ? JUMP_TRUE : JUMP_FALSE)[a.op]} ${target}`);
      return;
    }
    if (a.r && a.r.t === "imm") emit(`cmp rax, ${a.r.v}`);
    else {
      emit("push rax");
      if (!genExpr(a.r, "rax")) {
        emit("pop rax");
        emit("; [!] 条件原子无法表达，按恒假处理");
        emit("xor rax, rax");
        emit("test rax, rax");
        emit(`${(jumpOnTrue ? JUMP_TRUE : JUMP_FALSE)[a.op]} ${target}`);
        return;
      }
      emit("mov r10, rax");
      emit("pop rax");
      emit("cmp rax, r10");
    }
    emit(`${(jumpOnTrue ? JUMP_TRUE : JUMP_FALSE)[a.op]} ${target}`);
  }

  function emitIf(node) {
    const k = ++lblN;
    const elseEmpty = !(Array.isArray(node.else) && node.else.length);
    emit(`; [块] if ${node.cond ?? ""}`);
    const parsed = asmParseCond(node.cond, refLabel);
    if (!parsed) {
      emit("; [!] 条件无法表达，按恒假处理（原条件：" + String(node.cond ?? "") + "）");
      emit("xor rax, rax");
      emit("test rax, rax");
      emit(`je ${elseEmpty ? `.L${k}_end` : `.L${k}_else`}`);
    } else if (parsed.disjs.length === 1) {
      parsed.disjs[0].forEach((a) => emitAtom(a, elseEmpty ? `.L${k}_end` : `.L${k}_else`, false));
    } else {
      parsed.disjs.forEach((atoms, i) => {
        if (i > 0) emit(`.L${k}_chk_${i + 1}:`);
        const falseT = i === parsed.disjs.length - 1 ? (elseEmpty ? `.L${k}_end` : `.L${k}_else`) : `.L${k}_chk_${i + 2}`;
        atoms.forEach((a) => emitAtom(a, falseT, false));
        emit(`jmp .L${k}_then`);
      });
      emit(`.L${k}_then:`);
    }
    emitNodes(node.then);
    if (!elseEmpty) {
      emit(`jmp .L${k}_end`);
      emit(`.L${k}_else:`);
      emitNodes(node.else);
    }
    emit(`.L${k}_end:`);
  }

  function emitNodes(nodes) {
    for (const nd of nodes || []) {
      const kind = nd.kind || "call";
      if (kind === "call") emitCall(nd);
      else if (kind === "repeat") emitRepeat(nd);
      else if (kind === "if") emitIf(nd);
      else if (kind === "break" || kind === "continue") {
        if (!loopStack.length) emit(`; [!] ${kind} 不在任何循环内，已忽略`);
        else emit(`jmp .L${loopStack[loopStack.length - 1]}_${kind === "break" ? "end" : "next"}`);
      }
    }
  }

  emit("entry:");
  emitNodes(prog);
  emit("ret");

  const strData = strings.map(({ key, value }) => {
    if (/^[\x20-\x7E]*$/.test(value) && !value.includes('"')) return `${key}: du "${value}", 0`;
    const words = [...value].map((ch) => `0x${ch.codePointAt(0).toString(16)}`).join(", ");
    return `${key}: dw ${words}${words ? ", " : ""}0`;
  });

  const header = [
    "; NTLab 编排 ⇄ 汇编（同一程序的两种形态，自动同步）",
    "; 积木侧任何改动会自动重写本文本；编辑本文本会自动尝试还原为积木，",
    "; 超出规范子集则暂时脱钩（改回可解析状态即自动复联）。",
    "; Windows x64 调用约定：rcx/rdx/r8/r9 + 栈；$N → slot_N；字符串 → str_K（UTF-16LE）。",
    "BITS 64",
    "default rel",
    "",
    "section .data",
    "align 8",
    ...dataLines,
    ...strData,
    "",
    "section .text",
  ];
  return { text: [...header, ...code].join("\n") };
}

/* ---- 解析还原：x86-64 文本 → 积木树（仅规范子集） ---- */
function asmToProgram(text, specOf) {
  const rawLines = String(text).split("\n");
  // 剥注释（跳过引号内的分号）
  const lines = rawLines.map((s, ix) => {
    let out = "";
    let q = null;
    for (const ch of s) {
      if (q) { out += ch; if (ch === q) q = null; }
      else if (ch === '"') { q = ch; out += ch; }
      else if (ch === ";") break;
      else out += ch;
    }
    return { ln: ix + 1, t: out.trim() };
  });
  const err = (ln, line, msg) => ({ ok: false, err: { ln, line, msg } });

  // 数据段：字符串标签
  const strByLabel = new Map();
  let inData = false;
  for (const { t } of lines) {
    if (/^section\b/.test(t)) { inData = /\bdata\b/.test(t); continue; }
    if (!inData || !t) continue;
    let m = t.match(/^(str_\d+):\s*du\s+"(.*)",\s*0$/);
    if (m) { strByLabel.set(m[1], m[2]); continue; }
    m = t.match(/^(str_\d+):\s*dw\s+(.+)$/);
    if (m) {
      const words = m[2].split(",").map((x) => x.trim()).filter(Boolean).map((x) => parseInt(x, 0));
      strByLabel.set(m[1], words.map((c) => String.fromCodePoint(c)).join("").replace(/\0$/, ""));
    }
  }

  function labelSym(lbl) {
    let m = lbl.match(/^slot_(\d+)$/);
    if (m) return { t: "ref", n: +m[1], path: "ret.value" };
    m = lbl.match(/^slot_(\d+)_out_(.+?)(_(len|size))?$/);
    if (m) {
      const suffix = m[4] === "len" ? ".len_chars" : m[4] === "size" ? ".size" : "";
      return { t: "ref", n: +m[1], path: `out.${m[2]}${suffix}` };
    }
    m = lbl.match(/^buf_(\d+)_(.+)$/);
    if (m) return { t: "ref", n: +m[1], path: `out.${m[2]}.text` };
    m = lbl.match(/^str_(\d+)$/);
    if (m) return { t: "str", v: strByLabel.get(`str_${m[1]}`) ?? "" };
    return null;
  }

  function operandSym(op, st) {
    if (asmIsInt(op)) return { t: "imm", v: Number(op) };
    const m = op.match(/^\[(.+)\]$/);
    if (m) return labelSym(m[1].trim());
    if (/^[a-z][a-z0-9]*$/.test(op)) return st[op] ?? null;
    return null;
  }

  function valStr(v) {
    if (!v) return "0";
    if (v.t === "imm") return String(v.v);
    if (v.t === "ref") return `$${v.n}${v.path ? "." + v.path : ""}`;
    if (v.t === "str") return String(v.v);
    if (v.t === "neg") return `(-${valStr(v.e)})`;
    if (v.t === "bin") return `(${valStr(v.l)} ${v.op} ${valStr(v.r)})`;
    return "0";
  }

  // 单条指令的符号解释（语句内局部状态）；返回 null 或错误消息
  const ASM_REG_OK = new Set(["rax", "rbx", "r10", "rcx", "rdx", "r8", "r9", "eax"]);
  function interp(t, st) {
    let m = t.match(/^mov\s+([a-z][a-z0-9]*),\s*(.+)$/);
    if (m && !/^qword/.test(m[2])) {
      if (!ASM_REG_OK.has(m[1])) return `无法识别的寄存器：${m[1]}（超出规范子集）`;
      const v = operandSym(m[2].trim(), st);
      if (!v) return `无法识别的操作数：${m[2]}`;
      st[m[1]] = v;
      return null;
    }
    if (t === "push rax") { st.stack.push(st.rax ?? ASM_ZERO); return null; }
    if (t === "pop rax") { st.rax = st.stack.pop() ?? ASM_ZERO; return null; }
    m = t.match(/^xor\s+([a-z][a-z0-9]*),\s*\1$/);
    if (m) {
      if (!ASM_REG_OK.has(m[1])) return `无法识别的寄存器：${m[1]}（超出规范子集）`;
      st[m[1]] = ASM_ZERO;
      return null;
    }
    m = t.match(/^lea\s+([a-z][a-z0-9]*),\s*\[(.+)\]$/);
    if (m) {
      if (!ASM_REG_OK.has(m[1])) return `无法识别的寄存器：${m[1]}（超出规范子集）`;
      const v = labelSym(m[2].trim());
      if (!v) return `无法识别的标签：${m[2]}`;
      st[m[1]] = v;
      return null;
    }
    m = t.match(/^imul\s+([a-z][a-z0-9]*),\s*([a-z][a-z0-9]*),\s*(.+)$/);
    if (m) {
      if (!ASM_REG_OK.has(m[1])) return `无法识别的寄存器：${m[1]}（超出规范子集）`;
      const b = operandSym(m[3].trim(), st);
      if (!b) return `无法识别的操作数：${m[3]}`;
      st[m[1]] = { t: "bin", op: "*", l: st[m[2]] ?? ASM_ZERO, r: b };
      return null;
    }
    m = t.match(/^(add|sub|imul)\s+(?!rsp\b)([a-z][a-z0-9]*),\s*(.+)$/);
    if (m) {
      if (!ASM_REG_OK.has(m[2])) return `无法识别的寄存器：${m[2]}（超出规范子集）`;
      const b = operandSym(m[3].trim(), st);
      if (!b) return `无法识别的操作数：${m[3]}`;
      const op = m[1] === "imul" ? "*" : (m[1] === "add" ? "+" : "-");
      st[m[2]] = { t: "bin", op, l: st[m[2]] ?? ASM_ZERO, r: b };
      return null;
    }
    m = t.match(/^neg\s+([a-z][a-z0-9]*)$/);
    if (m) {
      if (!ASM_REG_OK.has(m[1])) return `无法识别的寄存器：${m[1]}（超出规范子集）`;
      st[m[1]] = { t: "neg", e: st[m[1]] ?? ASM_ZERO };
      return null;
    }
    if (t === "cqo") return null;
    m = t.match(/^idiv\s+(.+)$/);
    if (m) {
      const b = operandSym(m[1].trim(), st);
      if (!b) return `无法识别的操作数：${m[1]}`;
      const a = st.rax ?? ASM_ZERO;
      st.rax = { t: "bin", op: "/", l: a, r: b };
      st.rdx = { t: "bin", op: "%", l: a, r: b };
      return null;
    }
    m = t.match(/^mov\s+qword\s*\[rsp\+(0x[0-9a-f]+|\d+)\],\s*([a-z][a-z0-9]*)$/);
    if (m) {
      if (!ASM_REG_OK.has(m[2])) return `无法识别的寄存器：${m[2]}（超出规范子集）`;
      st.stk.set(parseInt(m[1], 0), st[m[2]] ?? ASM_ZERO);
      return null;
    }
    if (/^(sub|add)\s+rsp,\s*(0x[0-9a-f]+|\d+)$/.test(t)) return null;
    return `无法识别的指令：${t}`;
  }

  const newSymState = () => ({ stack: [], stk: new Map() });
  const defaultEqual = (txt, def) => {
    if (def == null) return false;
    const ds = String(def).trim();
    if (ds === txt) return true;
    if (ds === "" && (txt === "0" || txt === "")) return true;   // 空默认 → xor reg,reg → "0"
    return asmIsInt(ds) && asmIsInt(txt) && Number(ds) === Number(txt);
  };
  const isHardAnchor = (t) => /^call\s/.test(t) || t === "push rbx" || /^\.L\d+_(loop|next|end|else):$/.test(t)
    || /^jmp\s+\.L\d+_(loop|next|end)$/.test(t) || /^mov\s*\[slot_/.test(t) || t === "ret" || t === "pop rbx"
    || /^mov\s+qword\s*\[rsp/.test(t) || t === "dec rbx";

  // 从 j 向前找下一个原子终点（cmp/test rax + jcc 结构标签）或体开始锚点
  function huntAtom(j, knownK) {
    let evalStart = null;
    for (let q = j; q < lines.length; q++) {
      const t = lines[q].t;
      if (!t) continue;
      if (/^jmp\s+\.L\d+_then$/.test(t)) continue;
      if (/^\.L\d+_(then|chk_\d+):$/.test(t)) continue;
      if (evalStart === null) evalStart = q;
      if (/^cmp\s/.test(t) || /^test\s+rax,\s*rax$/.test(t)) {
        let r = q + 1;
        while (r < lines.length && !lines[r].t) r++;
        const m2 = lines[r] && lines[r].t.match(/^(j\w+)\s+\.L(\d+)_(else|end|then|chk_\d+)$/);
        if (!m2) return { body: true };
        const k2 = +m2[2];
        if (knownK != null && k2 !== knownK) return { body: true };
        return { atom: true, evalStart, cmpIdx: q, jccIdx: r, k: k2, kind: m2[3], jmp: m2[1], next: r + 1 };
      }
      if (isHardAnchor(t)) return { body: true };
    }
    return { body: true };
  }

  function firstAnchor(i) {
    for (let q = i; q < lines.length; q++) {
      const t = lines[q].t;
      if (!t) continue;
      if (/^cmp\s/.test(t) || /^test\s+rax,\s*rax$/.test(t)) return { q, type: "cond" };
      if (/^call\s/.test(t)) return { q, type: "call" };
      if (t === "push rbx") return { q, type: "repeat" };
      if (/^jmp\s+\.L\d+_(end|next)$/.test(t)) return { q, type: "brk" };
      if (/^\.L\d+_/.test(t) || /^entry:$/.test(t)) return { q, type: "label" };
      if (t === "ret") return { q, type: "eof" };
      if (/^jmp\s/.test(t)) return { q, type: "bad" };
      if (/^(mov|lea|xor|push|pop|add|sub|imul|neg|idiv|cqo)\b/.test(t)) continue;
      return { q, type: "bad" };
    }
    return { q: lines.length, type: "eof" };
  }

  const repStack = [];   // 当前嵌套的循环标签 k（break/continue 校验用）
  const enclosingRepeat = (k) => repStack.includes(k);
  let callCount = 0;

  function parseCallStmt(i) {
    const st = newSymState();
    let j = i;
    let api = null;
    for (; j < lines.length; j++) {
      const t = lines[j].t;
      if (!t) continue;
      const m = t.match(/^call\s+(\w+)$/);
      if (m) { api = m[1]; j++; break; }
      const e = interp(t, st);
      if (e) return err(lines[j].ln, t, e);
    }
    if (!api) return err(lines[i].ln, lines[i].t, "调用语句缺少 call 指令");
    while (j < lines.length && !lines[j].t) j++;
    if (lines[j] && /^(add|sub)\s+rsp,/.test(lines[j].t)) j++;
    while (j < lines.length && !lines[j].t) j++;
    const m = lines[j] && lines[j].t.match(/^mov\s*\[slot_(\d+)\],\s*rax$/);
    if (!m) return err(lines[i].ln, lines[i].t, "call 之后缺少 mov [slot_N], rax");
    const n = +m[1];
    if (n !== callCount + 1) return err(lines[j].ln, lines[j].t, `slot 编号应为 ${callCount + 1}，却是 ${n}`);
    callCount++;
    const spec = specOf(api);
    if (!spec) return err(lines[j].ln, lines[j].t, `${api} 不在白名单`);
    const syms = [st.rcx, st.rdx, st.r8, st.r9];
    const args = {};
    const params = spec.params;
    for (let idx = 0; idx < params.length; idx++) {
      const p = params[idx];
      const v = idx < 4 ? syms[idx] : st.stk.get(0x20 + 8 * (idx - 4));
      if (v == null) return err(lines[j].ln, lines[j].t, `第 ${idx + 1} 个参数（${p.name}）缺少装载指令`);
      if (AUTO_PARAM(p)) continue;
      const txt = valStr(v);
      if (!defaultEqual(txt, p.default)) args[p.name] = txt;
    }
    return { node: { kind: "call", api, args }, idx: j + 1 };
  }

  function condStrOf(atoms) {
    const atomStr = (a) => `${valStr(a.l)} ${a.op} ${valStr(a.r)}`;
    const groups = [];
    for (const a of atoms) {
      const g = groups[groups.length - 1];
      if (g && g.target === a.target) g.atoms.push(a);
      else groups.push({ target: a.target, atoms: [a] });
    }
    return groups.map((g) => g.atoms.map(atomStr).join(" && ")).join(" || ");
  }

  function parseIfStmt(i) {
    const atoms = [];
    let j = i;
    let k = null;
    for (;;) {
      const h = huntAtom(j, k);
      if (!h.atom) break;
      const st = newSymState();
      for (let q = h.evalStart; q < h.cmpIdx; q++) {
        const t = lines[q].t;
        if (!t) continue;
        const e = interp(t, st);
        if (e) return err(lines[q].ln, t, e);
      }
      const cmpT = lines[h.cmpIdx].t;
      const mm = cmpT.match(/^cmp\s+(.+),\s*(.+)$/);
      let l, r;
      if (mm) {
        l = operandSym(mm[1].trim(), st);
        r = operandSym(mm[2].trim(), st);
        if (!l || !r) return err(lines[h.cmpIdx].ln, cmpT, "比较的操作数无法识别");
      } else {
        l = st.rax ?? ASM_ZERO;
        r = ASM_ZERO;
      }
      const opMap = { je: "==", jne: "!=", jl: "<", jle: "<=", jg: ">", jge: ">=", jb: "<", jae: ">=", jbe: "<=", ja: ">", jnae: "<", jnb: ">=", jnbe: ">", jna: "<=" };
      const direct = opMap[h.jmp];
      if (!direct) return err(lines[h.jccIdx].ln, lines[h.jccIdx].t, `无法识别的跳转助记符 ${h.jmp}`);
      atoms.push({
        op: asmInvertCmp(direct),   // 代码生成统一「条件为假则跳」，原子真值 = 落空条件
        l, r,
        target: `.L${h.k}_${h.kind}`,
      });
      if (k == null) k = h.k;
      j = h.next;
    }
    if (!atoms.length) return err(lines[i].ln, lines[i].t, "条件结构缺失（cmp/test + jcc）");
    // 跳过 || 形态的结构标记
    while (j < lines.length) {
      const t = lines[j].t;
      if (!t || /^jmp\s+\.L\d+_then$/.test(t) || /^\.L\d+_(then|chk_\d+):$/.test(t)) { j++; continue; }
      break;
    }
    const b1 = parseBlock(j, (t) => t === `.L${k}_else:` || t === `.L${k}_end:` || t === `jmp .L${k}_end`);
    if (b1.err) return b1;
    const node = { kind: "if", cond: condStrOf(atoms), then: b1.nodes, else: [] };
    let idx = b1.idx;
    const stopT = idx < lines.length ? lines[idx].t : "";
    if (stopT === `.L${k}_else:`) {
      const b2 = parseBlock(idx + 1, (t) => t === `.L${k}_end:`);
      if (b2.err) return b2;
      node.else = b2.nodes;
      idx = b2.idx;
      if (lines[idx] && lines[idx].t === `.L${k}_end:`) idx++;
      else return err(lines[idx]?.ln ?? idx, lines[idx]?.t ?? "", "缺少 .Lk_end 收尾");
    } else if (stopT === `jmp .L${k}_end`) {
      idx++;
      while (idx < lines.length && !lines[idx].t) idx++;
      if (lines[idx] && lines[idx].t === `.L${k}_else:`) {
        const b2 = parseBlock(idx + 1, (t) => t === `.L${k}_end:`);
        if (b2.err) return b2;
        node.else = b2.nodes;
        idx = b2.idx;
        if (lines[idx] && lines[idx].t === `.L${k}_end:`) idx++;
        else return err(lines[idx]?.ln ?? idx, lines[idx]?.t ?? "", "缺少 .Lk_end 收尾");
      } else if (lines[idx] && lines[idx].t === `.L${k}_end:`) idx++;
      else return err(lines[idx]?.ln ?? idx, lines[idx]?.t ?? "", "条件块未闭合");
    } else if (stopT === `.L${k}_end:`) {
      idx++;
    } else {
      return err(lines[idx]?.ln ?? idx, lines[idx]?.t ?? "", "条件块未闭合");
    }
    return { node, idx };
  }

  function parseRepeatStmt(i) {
    const st = newSymState();
    let j = i + 1;
    let loopIdx = -1;
    for (; j < lines.length; j++) {
      const t = lines[j].t;
      if (!t) continue;
      if (/^\.L\d+_loop:$/.test(t)) { loopIdx = j; break; }
      const e = interp(t, st);
      if (e) return err(lines[j].ln, t, e);
    }
    if (loopIdx < 0) return err(lines[i].ln, lines[i].t, "push rbx 之后缺少 .Lk_loop 循环头");
    const k = +lines[loopIdx].t.match(/^\.L(\d+)_loop:$/)[1];
    if (!st.rbx) return err(lines[loopIdx].ln, lines[loopIdx].t, "循环计数缺少 mov rbx, …");
    const times = valStr(st.rbx);
    j = loopIdx + 1;
    while (j < lines.length && !lines[j].t) j++;
    if (!lines[j] || lines[j].t !== "test rbx, rbx") return err(lines[j]?.ln ?? j, lines[j]?.t ?? "", "循环头缺少 test rbx, rbx");
    j++;
    while (j < lines.length && !lines[j].t) j++;
    if (!lines[j] || lines[j].t !== `je .L${k}_end`) return err(lines[j]?.ln ?? j, lines[j]?.t ?? "", "循环头缺少 je .Lk_end");
    repStack.push(k);
    const b = parseBlock(j + 1, (t) => t === `.L${k}_next:`);
    repStack.pop();
    if (b.err) return b;
    let idx = b.idx;
    if (lines[idx] && lines[idx].t === `.L${k}_next:`) idx++;   // 消费停住的循环体收尾标签
    const expect = ["dec rbx", `jmp .L${k}_loop`, `.L${k}_end:`, "pop rbx"];
    for (const want of expect) {
      while (idx < lines.length && !lines[idx].t) idx++;
      if (!lines[idx] || lines[idx].t !== want) {
        return err(lines[idx]?.ln ?? idx, lines[idx]?.t ?? "", `循环尾结构破损：应为 ${want}`);
      }
      idx++;
    }
    return { node: { kind: "repeat", times, body: b.nodes }, idx };
  }

  function parseBlock(startIdx, stopTest) {
    const nodes = [];
    let i = startIdx;
    for (;;) {
      while (i < lines.length && !lines[i].t) i++;
      if (i >= lines.length) return { nodes, idx: i };
      const t = lines[i].t;
      if (/^(entry:|ret|BITS\b.*|default\b.*|section\b.*|align\b.*|global\b.*)$/.test(t)) { i++; continue; }
      if (/^[\w.]+:\s*(dq|du|dw|times)\b/.test(t)) { i++; continue; }
      if (/^\.L\d+_(loop|next|end|then|else|chk_\d+):$/.test(t)) {
        if (stopTest && stopTest(t)) return { nodes, idx: i };
        return err(lines[i].ln, t, `意外的标签 ${t}（块未按规范闭合）`);
      }
      if (stopTest && stopTest(t)) return { nodes, idx: i };
      const anc = firstAnchor(i);
      if (anc.type === "eof") {
        if (anc.q > i) return err(lines[i].ln, t, "语句不完整（超出积木可还原的规范子集）");
        return { nodes, idx: anc.q };
      }
      if (anc.type === "bad") return err(lines[anc.q]?.ln ?? lines[i].ln, lines[anc.q]?.t ?? t, "无法识别的语句（超出积木可还原的规范子集）");
      if (anc.type === "label") return err(lines[anc.q].ln, lines[anc.q].t, "意外的标签/指令");
      let r;
      if (anc.type === "cond") r = parseIfStmt(i);
      else if (anc.type === "call") r = parseCallStmt(i);
      else if (anc.type === "repeat") r = parseRepeatStmt(i);
      else if (anc.type === "brk") {
        const m = lines[anc.q].t.match(/^jmp\s+\.L(\d+)_(end|next)$/);
        if (!enclosingRepeat(+m[1])) {
          return err(lines[anc.q].ln, lines[anc.q].t, "break/continue 指向的循环不在当前嵌套内");
        }
        nodes.push({ kind: m[2] === "end" ? "break" : "continue" });
        i = anc.q + 1;
        continue;
      }
      if (r.err) return r;
      nodes.push(r.node);
      i = r.idx;
    }
  }

  const top = parseBlock(0, null);
  if (top.err) return top;
  return { ok: true, program: top.nodes };
}

/* ---------------- 页面 ---------------- */

export async function mount(el) {
  el.innerHTML = `
    <div class="page-head">
      <h1>WinAPI 试验台<span class="page-hint" title="白名单内的 API 在本服务器进程中执行；编排区支持循环/条件/跳出块；参数可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块；「编排」与「汇编」是同一程序的两种形态，自动双向同步（汇编超出规范子集时暂时脱钩）">?</span></h1>
      <p>白名单调用 · 积木编排（循环 / 条件 / 跳出） · NTSTATUS / GetLastError 渲染 · 编排 ⇄ 汇编 实时同步</p>
    </div>
    <div class="playground">
      <div class="pg-left panel">
        <input class="filter-input" id="pg-search" placeholder="搜索 API / 模块…">
        <div class="pg-catalog" id="pg-catalog"></div>
      </div>
      <div class="pg-right">
        <ui-tabs id="pg-tabs">
          <span data-value="call" label="调用卡"></span>
          <span data-value="queue" label="编排"></span>
          <span data-value="asm" label="汇编"></span>
        </ui-tabs>
        <div class="pg-body" id="pg-body"></div>
      </div>
    </div>`;

  const cat = await apiGet("/api/winapi/catalog");
  const apis = cat.apis;

  const bodyEl = el.querySelector("#pg-body");
  const tabsEl = el.querySelector("#pg-tabs");
  const searchEl = el.querySelector("#pg-search");
  const catalogEl = el.querySelector("#pg-catalog");
  const pgLeft = el.querySelector(".pg-left");

  // 左栏 = 编排块的「家门口」：拖块到这里松手即移除
  pgLeft.addEventListener("dragover", (e) => {
    if (![...e.dataTransfer.types].includes("text/pg-move")) return;
    e.preventDefault();
    pgLeft.classList.add("pg-danger");
  });
  pgLeft.addEventListener("dragleave", () => pgLeft.classList.remove("pg-danger"));
  pgLeft.addEventListener("drop", (e) => {
    pgLeft.classList.remove("pg-danger");
    let fromPath = null;
    try { fromPath = JSON.parse(e.dataTransfer.getData("text/pg-move") || "null"); } catch { /* ignore */ }
    if (!Array.isArray(fromPath)) return;
    e.preventDefault();
    takeAt(fromPath);
    if (editingPath === JSON.stringify(fromPath)) editingPath = null;
    programChanged();
    swap(composeView());
  });

  let selected = null;                       // 当前选中的 spec
  let program = [];                          // 编排程序树（块：call/repeat/if/break/continue）
  let session = null;                        // 最近一次执行结果
  let sessions = [];                         // 历史会话（服务端内存）
  let editingPath = null;                    // 正在编辑参数的 call 块路径（JSON 字符串）
  let dragState = null;                      // {arr, idx, mark, markEl}

  const specByName = new Map(apis.map((a) => [a.name, a]));

  /* ---- 编排 ⇄ 汇编 同步管理（同一程序的两种形态） ---- */
  let asmText = programToAsm([], specByName.get.bind(specByName)).text;
  let asmDirty = false;                      // true = 汇编侧有无法还原的编辑（脱钩）
  let asmErr = null;                         // {ln, line, msg}
  let asmTimer = null;

  function syncSig(nodes) {
    for (const nd of nodes || []) {
      if ((nd.kind === "call" || !nd.kind) && !nd.sig) nd.sig = sigOf(nd.api, nd.args);
      syncSig(nd.body); syncSig(nd.then); syncSig(nd.else);
    }
  }

  /** 积木侧发生改动：以积木为准重新生成汇编（并解除脱钩）。 */
  function programChanged() {
    asmText = programToAsm(program, (n) => specByName.get(n)).text;
    asmDirty = false;
    asmErr = null;
    if (currentTab === "asm") {
      const ta = document.getElementById("pg-asm-ta");
      if (ta) ta.value = asmText;
    }
    updateAsmStatus();
  }

  /** 汇编侧编辑：尝试还原为积木；成功则整体替换并复联，失败则脱钩。 */
  function tryAsm() {
    const r = asmToProgram(asmText, (n) => specByName.get(n));
    if (r.ok) {
      syncSig(r.program);
      program = r.program;
      editingPath = null;
      asmDirty = false;
      asmErr = null;
      if (currentTab === "queue") { countSlots(); swap(composeView()); }
    } else {
      asmDirty = true;
      asmErr = r.err;
    }
    updateAsmStatus();
  }

  function updateAsmStatus() {
    const chip = document.getElementById("pg-asm-chip");
    if (chip) {
      chip.textContent = asmDirty ? "已脱钩" : "已同步";
      chip.className = "pg-sync-chip" + (asmDirty ? " off" : "");
      chip.title = asmDirty && asmErr ? `第 ${asmErr.ln} 行：${asmErr.msg}` : "积木与汇编是同一程序的两种形态";
    }
    const banner = document.getElementById("pg-asm-banner");
    if (banner) {
      if (asmDirty && asmErr) {
        banner.style.display = "";
        banner.innerHTML = `汇编第 ${asmErr.ln} 行无法还原为积木：${esc(asmErr.line)}<br>`
          + `<span class="pg-dim">原因：${esc(asmErr.msg)}。继续编辑至可解析即自动复联；`
          + `也可回「编排」做任意修改（以积木为准重新生成汇编）。</span>`;
      } else banner.style.display = "none";
    }
  }

  /** 调用块的默认参数：取白名单默认值（出参/自动参数除外）。 */
  function defaultArgs(spec) {
    const args = {};
    for (const p of spec.params) {
      if (!AUTO_PARAM(p) && p.default != null) args[p.name] = p.default;
    }
    return args;
  }

  function sigOf(name, args) {
    const spec = specByName.get(name);
    // 按白名单参数顺序展示；未在 schema 中的键（不应出现）排在其后
    const keys = spec
      ? [...spec.params.filter((p) => !AUTO_PARAM(p)).map((p) => p.name),
         ...Object.keys(args).filter((k) => !spec.params.some((p) => p.name === k))]
      : Object.keys(args);
    return `${name}(${keys.filter((k) => args[k] != null).map((k) => `${k}=${args[k]}`).join(", ")})`;
  }

  function paramTooltip(p) {
    const def = p.default != null ? `默认 ${p.default}` : "无默认值";
    return `${p.name} (${p.type}) — ${p.desc || "无说明"}；${def}；可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块`;
  }

  /* ---- 编排树操作：path = [idx, "body", idx, "then", idx, ...] ---- */
  function getArr(path) { // path 长度为偶数（不含末端索引）
    let arr = program;
    for (let k = 0; k < path.length; k += 2) arr = arr[path[k]][path[k + 1]];
    return arr;
  }
  function takeAt(path) {
    const parent = path.length === 1 ? program : getArr(path.slice(0, -1));
    return parent.splice(path[path.length - 1], 1)[0];
  }
  /** 程序中第 N 个 call 块（静态编号，DFS 序）。 */
  function countSlots() {
    let n = 0;
    const walk = (nodes) => {
      if (!Array.isArray(nodes)) return;
      for (const nd of nodes) {
        if (nd.kind === "call" || !nd.kind) nd._no = ++n;
        walk(nd.body); walk(nd.then); walk(nd.else);
      }
    };
    walk(program);
  }
  function makeCall(name) {
    const spec = specByName.get(name);
    const args = defaultArgs(spec);
    return { kind: "call", api: name, args, sig: sigOf(name, args) };
  }
  function makeBlock(kind) {
    if (kind === "repeat") return { kind, times: 3, body: [] };
    if (kind === "if") return { kind, cond: "$1.ret.value == 1", then: [], else: [] };
    return { kind };  // break / continue
  }

  /* ---- 目录 ---- */
  function renderCatalog(filter = "") {
    const q = filter.trim().toLowerCase();
    const list = q
      ? apis.filter((a) => a.name.toLowerCase().includes(q) || a.module.toLowerCase().includes(q))
      : apis;
    catalogEl.replaceChildren(...list.map((a) => {
      const d = document.createElement("div");
      d.className = "pg-item" + (selected && selected.name === a.name ? " sel" : "");
      d.innerHTML = `<span class="nm">${esc(a.name)}</span>`
        + `<span class="mod">${esc(a.module)}</span>`
        + `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">`
        + `${a.side === "observable" ? "有副作用" : "safe"}</span>`;
      d.addEventListener("click", () => {
        selected = a;
        renderCatalog(filter);
        if (currentTab === "call") swap(renderCallCard());
      });
      // 拖拽：把 API 拎进右侧编排区（拖动开始时自动切到编排标签）
      d.draggable = true;
      d.addEventListener("dragstart", (e) => {
        e.dataTransfer.setData("text/pg-api", a.name);
        e.dataTransfer.effectAllowed = "copy";
        showTab("queue");
      });
      return d;
    }));
    if (!list.length) {
      const d = document.createElement("div");
      d.className = "empty";
      d.textContent = "（无匹配 API）";
      catalogEl.replaceChildren(d);
    }
  }
  searchEl.addEventListener("input", () => renderCatalog(searchEl.value));

  /* ---- 调用卡 ---- */
  function argInputs(spec) {
    const rows = [];
    for (const p of spec.params.filter((p) => !AUTO_PARAM(p))) {
      const row = document.createElement("div");
      row.className = "row";
      const def = p.default != null ? String(p.default) : "";
      row.innerHTML = `<label>${esc(p.name)} <span class="dim">(${esc(p.type)})</span></label>`;
      const input = document.createElement("input");
      input.className = "filter-input";
      input.style.width = "260px";
      input.value = def;
      input.placeholder = p.desc || p.type;
      input.dataset.param = p.name;
      row.appendChild(input);
      rows.push(row);
    }
    return { rows };
  }

  function collectArgs(spec, root) {
    const args = {};
    for (const input of root.querySelectorAll("input[data-param]")) {
      const v = input.value.trim();
      if (v !== "") args[input.dataset.param] = v;
    }
    return args;
  }

  function renderRet(part) {
    if (part == null) return "—";
    if (typeof part !== "object") return esc(String(part));
    let s = `${esc(String(part.value))} <span class="dim">(${esc(part.hex ?? "")})</span>`;
    if (part.ntstatus) {
      s += ` · ${part.success ? "STATUS_SUCCESS" : "失败"}`;
      if (part.ntstatus.message) s += ` — ${esc(part.ntstatus.message)}`;
    }
    if (part.last_error) s += ` · GetLastError=${esc(String(part.last_error))}`;
    if (part.valid === false) s += ' <span class="err">INVALID</span>';
    return s;
  }

  function renderStepCard(step, idx) {
    const d = document.createElement("div");
    d.className = "pg-step-card";
    const head = `<h4>#${idx + 1} ${esc(step.module ?? "")}!${esc(step.api ?? "?")}`
      + `<span class="dur">${[step.where, step.duration_ms != null ? `${step.duration_ms} ms` : ""]
        .filter(Boolean).join(" · ")}</span></h4>`;
    if (step.error) {
      d.innerHTML = head + `<div class="err">${esc(step.error)}</div>`;
      return d;
    }
    const rows = [];
    for (const [k, v] of Object.entries(step.args ?? {})) {
      rows.push(`<tr><td class="k">${esc(k)}</td><td class="v">${esc(v == null ? "null" : String(v))}</td></tr>`);
    }
    rows.push(`<tr><td class="k">返回</td><td class="v">${renderRet(step.ret)}</td></tr>`);
    for (const [k, fields] of Object.entries(step.out ?? {})) {
      for (const [fk, fv] of Object.entries(fields)) {
        rows.push(`<tr><td class="k">${esc(k)}.${esc(fk)}</td><td class="v">${esc(String(fv))}</td></tr>`);
      }
    }
    d.innerHTML = head + `<table>${rows.join("")}</table>`;
    return d;
  }

  function renderSession(session) {
    const wrap = document.createElement("div");
    if (!session) {
      wrap.innerHTML = '<p class="table-meta">尚无执行结果。选择左侧 API，或在编排区搭好后执行。</p>';
      return wrap;
    }
    const h = document.createElement("h3");
    h.textContent = `会话 ${session.id} · 成功 ${session.ok} / 失败 ${session.failed}`;
    h.style.margin = "0 0 10px";
    wrap.append(h, ...session.steps.map((s, i) => renderStepCard(s, i)));
    return wrap;
  }

  function renderCallCard(result = null) {
    const wrap = document.createElement("div");
    if (!selected) {
      wrap.innerHTML = '<p class="table-meta">从左侧选择一个 API 构建调用。</p>';
      return wrap;
    }
    const a = selected;
    const panel = document.createElement("div");
    panel.className = "panel pg-card";
    panel.style.marginBottom = "12px";
    const head = document.createElement("h3");
    head.innerHTML = `${esc(a.module)}!<b>${esc(a.name)}</b> `
      + `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">${a.side === "observable" ? "有副作用" : "safe"}</span>`;
    const desc = document.createElement("p");
    desc.className = "table-meta";
    desc.textContent = a.desc + (a.note ? ` · ${a.note}` : "");
    panel.append(head, desc);

    const { rows } = argInputs(a);
    if (rows.length) panel.append(...rows);
    else {
      const noArgs = document.createElement("p");
      noArgs.className = "table-meta";
      noArgs.textContent = "（无输入参数，出参自动生成）";
      panel.appendChild(noArgs);
    }

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0";
    const runBtn = document.createElement("ui-button");
    runBtn.setAttribute("label", "立即执行");
    runBtn.setAttribute("variant", "accent");
    const addBtn = document.createElement("ui-button");
    addBtn.setAttribute("label", "加入编排");
    btns.append(runBtn, addBtn);
    panel.appendChild(btns);

    const hint = document.createElement("p");
    hint.className = "table-meta";
    hint.textContent = "参数可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块";
    panel.appendChild(hint);

    const out = document.createElement("div");
    if (result) out.appendChild(renderSession(result.session));

    runBtn.addEventListener("click", async () => {
      const args = collectArgs(a, panel);
      try {
        const s = await apiPost("/api/winapi/run", { calls: [{ api: a.name, args }] });
        out.replaceChildren(renderSession(s));
      } catch (err) {
        out.innerHTML = `<div class="error-box"><span class="code">${esc(err.code)}</span> — ${esc(err.message)}</div>`;
      }
    });
    addBtn.addEventListener("click", () => {
      const args = collectArgs(a, panel);
      program.push({ kind: "call", api: a.name, args, sig: sigOf(a.name, args) });
      tabsEl.setAttribute("value", "queue");
      programChanged();
      swap(composeView());
    });

    wrap.append(panel, out);
    return wrap;
  }

  /* ---- 调用块的紧凑参数编辑器（点击块头展开） ---- */
  function buildEditor(node, slotNo) {
    const spec = specByName.get(node.api);
    const ed = document.createElement("div");
    ed.className = "pg-edit";
    const h = document.createElement("span");
    h.className = "pg-edit-h";
    h.textContent = `编辑调用 #${slotNo} ${node.api}`;
    ed.appendChild(h);
    for (const p of spec.params.filter((p) => !AUTO_PARAM(p))) {
      const fld = document.createElement("span");
      fld.className = "fld";
      const label = document.createElement("label");
      label.textContent = p.name;
      label.title = paramTooltip(p);
      const input = document.createElement("input");
      input.value = node.args[p.name] != null ? String(node.args[p.name]) : "";
      input.title = paramTooltip(p);
      input.placeholder = p.desc || p.type;
      input.addEventListener("input", () => {
        const v = input.value.trim();
        if (v === "") delete node.args[p.name];
        else node.args[p.name] = v;
        node.sig = sigOf(node.api, node.args);
        const blk = ed.closest(".pg-blk");
        const sigEl = blk && blk.querySelector(".sig");
        if (sigEl) sigEl.textContent = node.sig;
        programChanged();
      });
      fld.append(label, input);
      ed.appendChild(fld);
    }
    return ed;
  }

  /* ---- 编排（积木式程序树） ---- */
  function clearDropMarks() {
    document.querySelectorAll(".pg-drop-before,.pg-drop-after,.pg-into")
      .forEach((n) => n.classList.remove("pg-drop-before", "pg-drop-after", "pg-into"));
  }
  function isProgDrag(e) {
    return [...e.dataTransfer.types].some((t) => t === "text/pg-api" || t === "text/pg-move");
  }

  function doDrop(e) {
    const t = dragState;
    dragState = null;
    clearDropMarks();
    if (!t) return;
    let node;
    const apiName = e.dataTransfer.getData("text/pg-api");
    if (apiName) {
      node = makeCall(apiName);
    } else {
      const fromPath = JSON.parse(e.dataTransfer.getData("text/pg-move"));
      node = takeAt(fromPath);
      const fromArr = fromPath.length === 1 ? program : getArr(fromPath.slice(0, -1));
      const fromIdx = fromPath[fromPath.length - 1];
      if (fromArr === t.arr && fromIdx < t.idx) t.idx--;
    }
    if (node) t.arr.splice(t.idx, 0, node);
    programChanged();
    swap(composeView());
  }

  /** 通用投放区：getTarget(e) → {arr, idx, mark, markEl}。 */
  function attachDropZone(el, getTarget) {
    el.addEventListener("dragover", (e) => {
      if (!isProgDrag(e)) return;
      e.preventDefault();
      e.stopPropagation();
      e.dataTransfer.dropEffect = e.dataTransfer.types.includes("text/pg-api") ? "copy" : "move";
      const t = getTarget(e);
      if (!t) return;
      dragState = t;
      clearDropMarks();
      t.markEl.classList.add(t.mark);
    });
    el.addEventListener("drop", (e) => {
      if (!isProgDrag(e)) return;
      e.preventDefault();
      e.stopPropagation();
      doDrop(e);
    });
  }

  function renderBlock(node, path) {
    const kind = node.kind || "call";
    const blk = document.createElement("div");
    blk.className = "pg-blk pg-" + (kind === "call" ? "call" : kind);
    blk.dataset.path = JSON.stringify(path);
    const hd = document.createElement("div");
    hd.className = "hd";
    blk.appendChild(hd);
    const isEditing = editingPath === JSON.stringify(path);

    // 拖拽（块头为手柄）+ 投放判定：上/中/下三分
    hd.draggable = !isEditing;
    hd.addEventListener("dragstart", (e) => {
      e.dataTransfer.setData("text/pg-move", JSON.stringify(path));
      e.dataTransfer.effectAllowed = "move";
      blk.classList.add("dragging");
    });
    hd.addEventListener("dragend", () => { blk.classList.remove("dragging"); clearDropMarks(); });
    attachDropZone(blk, (e) => {
      const rc = blk.getBoundingClientRect();
      const rel = (e.clientY - rc.top) / rc.height;
      const last = path[path.length - 1];
      if (containerKeyFor(node)) {
        if (rel < 0.3) return { arr: getArr(path.slice(0, -1)), idx: last, mark: "pg-drop-before", markEl: blk };
        if (rel > 0.7) return { arr: getArr(path.slice(0, -1)), idx: last + 1, mark: "pg-drop-after", markEl: blk };
        const carr = node[containerKeyFor(node)];
        return { arr: carr, idx: carr.length, mark: "pg-into", markEl: blk };
      }
      const idx = rel < 0.5 ? last : last + 1;
      const mark = rel < 0.5 ? "pg-drop-before" : "pg-drop-after";
      return { arr: getArr(path.slice(0, -1)), idx, mark, markEl: blk };
    });

    if (kind === "call") {
      hd.innerHTML = `<span class="no">#${node._no}</span><span class="sig">${esc(node.sig)}</span>`;
      hd.title = "点击编辑参数 · 拖动调整位置 · 拖回左侧列表移除";
      const del = document.createElement("ui-button");
      del.setAttribute("label", "移除");
      del.addEventListener("click", (e) => {
        e.stopPropagation();
        takeAt(path);
        if (editingPath === JSON.stringify(path)) editingPath = null;
        programChanged();
        swap(composeView());
      });
      hd.appendChild(del);
      hd.addEventListener("click", () => {
        editingPath = isEditing ? null : JSON.stringify(path);
        swap(composeView());
      });
      if (isEditing) blk.appendChild(buildEditor(node, node._no));
      return blk;
    }

    if (kind === "repeat") {
      hd.innerHTML = `<span class="kw">重复</span>`;
      const inp = document.createElement("input");
      inp.className = "pg-cond";
      inp.value = String(node.times ?? 3);
      inp.title = "循环次数：整数或表达式，可用 $N 引用（如 $2.ret.value）";
      inp.addEventListener("input", () => { node.times = inp.value.trim(); programChanged(); });
      hd.append(inp, mkText("次"));
      blk.appendChild(mkContainer(node, path, "body"));
      return blk;
    }

    if (kind === "if") {
      hd.innerHTML = `<span class="kw">如果</span>`;
      const inp = document.createElement("input");
      inp.className = "pg-cond";
      inp.value = String(node.cond ?? "");
      inp.title = "条件表达式：比较/逻辑运算（支持 && ||），$N.path 引用，如 $1.ret.value == 1";
      inp.addEventListener("input", () => { node.cond = inp.value; programChanged(); });
      hd.appendChild(inp);
      const thenWrap = document.createElement("div");
      thenWrap.className = "pg-branch";
      thenWrap.append(mkText("那么"));
      thenWrap.appendChild(mkContainer(node, path, "then"));
      blk.appendChild(thenWrap);
      if (Array.isArray(node.else)) {
        const elseWrap = document.createElement("div");
        elseWrap.className = "pg-branch";
        elseWrap.append(mkText("否则"));
        elseWrap.appendChild(mkContainer(node, path, "else"));
        blk.appendChild(elseWrap);
      } else {
        const addElse = document.createElement("ui-button");
        addElse.setAttribute("label", "+ 否则");
        addElse.addEventListener("click", () => { node.else = []; programChanged(); swap(composeView()); });
        blk.appendChild(addElse);
      }
      return blk;
    }

    // break / continue
    hd.innerHTML = `<span class="kw">${kind === "break" ? "跳出循环" : "下一轮"}</span>`;
    hd.title = "只能出现在循环块内部";
    return blk;
  }

  function containerKeyFor(node) {
    if (node.kind === "repeat") return "body";
    if (node.kind === "if") return "then";
    return null;
  }

  function mkText(s) {
    const span = document.createElement("span");
    span.className = "kw";
    span.textContent = s;
    return span;
  }

  function mkContainer(node, path, key) {
    const arr = node[key] || [];
    const body = document.createElement("div");
    body.className = "pg-blk-body";
    renderBlocks(arr, body, [...path, key]);
    if (!arr.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "（把块拖到这里）";
      body.appendChild(empty);
    }
    attachDropZone(body, () => ({ arr, idx: arr.length, mark: "pg-into", markEl: body }));
    return body;
  }

  function renderBlocks(arr, host, pathPrefix) {
    arr.forEach((node, i) => host.appendChild(renderBlock(node, [...pathPrefix, i])));
  }

  function composeView() {
    countSlots();
    const wrap = document.createElement("div");
    const panel = document.createElement("div");
    panel.className = "panel pg-steps";
    panel.style.marginBottom = "12px";
    const h = document.createElement("h3");
    h.textContent = `编排（${program.length} 个顶层块）`;
    panel.appendChild(h);

    const list = document.createElement("div");
    renderBlocks(program, list, []);
    if (!program.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "编排为空：把左侧 API 拖进来，或用下方工具条加循环/条件块。";
      list.appendChild(empty);
    }
    panel.appendChild(list);

    // 根列表 = 投放区（追加到末尾）
    attachDropZone(list, () => ({ arr: program, idx: program.length, mark: "pg-into", markEl: list }));

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0;flex-wrap:wrap";
    const add = (label, kind) => {
      const b = document.createElement("ui-button");
      b.setAttribute("label", label);
      b.addEventListener("click", () => { program.push(makeBlock(kind)); programChanged(); swap(composeView()); });
      return b;
    };
    const runBtn = document.createElement("ui-button");
    runBtn.setAttribute("label", "执行");
    runBtn.setAttribute("variant", "accent");
    runBtn.addEventListener("click", async () => {
      if (!program.length) return;
      try {
        session = await apiPost("/api/winapi/run", { program });
      } catch (err) {
        session = { id: "error", steps: [{ api: "program", error: err.message }], ok: 0, failed: 1 };
      }
      swap(composeView());
    });
    const clrBtn = document.createElement("ui-button");
    clrBtn.setAttribute("label", "清空");
    clrBtn.addEventListener("click", () => { program = []; session = null; editingPath = null; programChanged(); swap(composeView()); });
    btns.append(
      runBtn, clrBtn,
      add("+ 循环", "repeat"), add("+ 条件", "if"),
      add("+ 跳出", "break"), add("+ 下一轮", "continue"),
    );
    panel.appendChild(btns);

    wrap.appendChild(panel);

    if (asmDirty) {
      const bn = document.createElement("div");
      bn.className = "pg-desync-banner";
      bn.innerHTML = `汇编侧与积木已脱钩${asmErr ? `（第 ${asmErr.ln} 行：${esc(asmErr.msg)}）` : ""}`
        + ` — 此处任意修改将以积木为准并重新生成汇编；去「汇编」页继续编辑至可解析则自动复联。`;
      wrap.appendChild(bn);
    }
    if (session) {
      const h2 = document.createElement("h3");
      h2.textContent = "最近一次执行";
      h2.style.margin = "0 0 10px";
      wrap.append(h2, renderSession(session));
    }
    return wrap;
  }

  /* ---- ASM：同一编排的汇编形态（可编辑，自动双向同步） ---- */
  function asmView() {
    const wrap = document.createElement("div");
    const panel = document.createElement("div");
    panel.className = "panel";
    panel.style.marginBottom = "12px";
    const headRow = document.createElement("div");
    headRow.style.cssText = "display:flex;align-items:center;gap:10px;flex-wrap:wrap";
    const h = document.createElement("h3");
    h.textContent = "汇编 ⇄ 编排（同一程序的两种形态，自动同步）";
    h.style.margin = "0";
    const chip = document.createElement("span");
    chip.id = "pg-asm-chip";
    headRow.append(h, chip);
    panel.appendChild(headRow);
    const banner = document.createElement("div");
    banner.id = "pg-asm-banner";
    banner.className = "pg-desync-banner";
    banner.style.display = "none";
    panel.appendChild(banner);
    const hint = document.createElement("p");
    hint.className = "table-meta";
    hint.textContent = "x86-64（Intel 语法）：参数装 rcx/rdx/r8/r9 + call + mov [slot_N], rax；循环/条件以标签与跳转表达。"
      + "积木改动会自动重写此处；编辑此处会自动尝试还原为积木（超出规范子集则脱钩，改回可解析即复联）。仅展示与组装校验，绝不执行。";
    panel.appendChild(hint);
    const ta = document.createElement("textarea");
    ta.id = "pg-asm-ta";
    ta.className = "filter-input";
    ta.spellcheck = false;
    ta.style.cssText = "width:100%;height:420px;font-family:var(--ntlab-font-mono,'Cascadia Code',Consolas,'Microsoft YaHei',monospace);resize:vertical";
    ta.value = asmText;
    ta.addEventListener("input", () => {
      asmText = ta.value;
      if (asmTimer) clearTimeout(asmTimer);
      asmTimer = setTimeout(tryAsm, 500);
    });
    const btnRow = document.createElement("div");
    btnRow.style.cssText = "display:flex;gap:8px;margin:10px 0;flex-wrap:wrap";
    const checkBtn = document.createElement("ui-button");
    checkBtn.setAttribute("label", "组装校验");
    const out = document.createElement("div");
    out.style.marginTop = "8px";
    checkBtn.addEventListener("click", async () => {
      // 占位地址组装：跳过数据/标签/伪指令，call 与跳转目标替换为 0，内存标签替换为 [rip]
      const body = asmText.split("\n").map((l) => l.replace(/;.*$/, "").trim())
        .filter((l) => l && !/^\.L/.test(l) && !/^(entry:|ret|BITS|default|section|align|global)/.test(l)
          && !/\b(dq|du|dw|times)\b/.test(l))
        .map((l) => l.replace(/^(j\w+|call)\s+\S+$/, "$1 0").replace(/\[(slot|buf|str)[^\]]*\]/g, "[rip]"));
      if (!body.length) {
        out.innerHTML = '<p class="table-meta">（没有可校验的指令）</p>';
        return;
      }
      try {
        const r = await apiPost("/api/winapi/asm", { text: body.join("\n") });
        out.innerHTML = `<p class="table-meta">组装校验通过：${r.roundtrip.length} 条指令 / ${r.size} 字节`
          + `（占位地址组装，仅验证每条都是合法 x86-64）</p>`;
      } catch (err) {
        out.innerHTML = `<div class="error-box"><span class="code">${esc(err.code)}</span> — ${esc(err.message)}</div>`;
      }
    });
    btnRow.appendChild(checkBtn);
    panel.append(ta, btnRow, out);
    wrap.appendChild(panel);
    return wrap;
  }

  /* ---- tabs ---- */
  let currentTab = "call";
  function swap(node) { bodyEl.replaceChildren(node); }
  function showTab(v) {
    currentTab = v;
    tabsEl.setAttribute("value", v);
    if (v === "call") swap(renderCallCard());
    else if (v === "queue") swap(composeView());
    else if (v === "asm") { swap(asmView()); updateAsmStatus(); }
  }
  const tab = (value, label) => {
    const t = document.createElement("span");
    t.setAttribute("data-value", value);
    t.setAttribute("label", label);
    return t;
  };
  tabsEl.append(tab("call", "调用卡"), tab("queue", "编排"), tab("asm", "汇编"));
  tabsEl.addEventListener("ui-select", (e) => showTab(e.detail.value));

  /* ---- 会话历史入口 ---- */
  const histBtn = document.createElement("ui-button");
  histBtn.setAttribute("label", "历史会话");
  histBtn.style.cssText = "margin-top:10px;margin-bottom:8px";
  histBtn.addEventListener("click", async () => {
    const r = await apiGet("/api/winapi/sessions");
    sessions = r.sessions ?? [];
    if (!sessions.length) {
      swap(Object.assign(document.createElement("div"),
        { innerHTML: '<p class="table-meta">服务器内存中还没有历史会话。</p>' }));
      return;
    }
    const wrap = document.createElement("div");
    const h = document.createElement("h3");
    h.textContent = `历史会话（最近 ${sessions.length} 次，仅存内存）`;
    wrap.append(h, ...sessions.map((s) => renderSession(s)));
    swap(wrap);
  });
  el.querySelector(".page-head").appendChild(histBtn);

  /* ---- 启动 ---- */
  renderCatalog();
  showTab("call");

  // PE 分析器跳转：#/playground?api=CreateFileW
  const pre = query().get("api");
  if (pre) {
    const spec = apis.find((a) => a.name.toLowerCase() === pre.toLowerCase());
    if (spec) {
      selected = spec;
      renderCatalog(searchEl.value);
      tabsEl.setAttribute("value", "call");
      showTab("call");
    }
  }
}
