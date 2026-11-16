import { apiGet, apiPost, apiSend } from "../api.js";
import { query } from "../router.js";
import { swapContent, motionScale, assignIndices } from "../motion.js";

/** 汇编语法高亮：逐行 token 化为 HTML（注释/标签/伪指令/助记符/寄存器/立即数/引用）。 */
function highlightAsm(src) {
  const MN = "mov|lea|call|xor|cmp|test|jmp|j[a-z]{1,3}|push|pop|sub|add|imul|neg|cqo|idiv|dec|ret|nop";
  const TOKEN = new RegExp(
    '("(?:[^"\\\\]|\\\\.)*")'                       // 1 字符串
    + "|\\b(BITS|default|section|global|align|db|dw|du|times)\\b"  // 2 伪指令
    + "|\\b(" + MN + ")\\b"                          // 3 助记符
    + "|\\b(0x[0-9A-Fa-f]+|\\d+)\\b"                 // 4 立即数
    + "|\\b((?:slot|buf|str|mod|modp)_\\d+[\\w]*)\\b" // 5 引用标签
    + "|\\b(r[a-z0-9]{1,3}|e[a-z]{2})\\b"            // 6 寄存器
    + "|(\\.[\\w$]+:|[\\w$]+:)"                      // 7 标签定义
    + "|(\\.[\\w$]+)"                                // 8 跳转目标
    , "gi");
  const esc = (s) => s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  return src.split("\n").map((line) => {
    let code = line;
    let cmt = "";
    const ci = line.indexOf(";");
    if (ci >= 0) { code = line.slice(0, ci); cmt = line.slice(ci); }
    let html = esc(code).replace(TOKEN, (m, str, dir, mn, num, ref, reg, lbl, tgt) => {
      const cls = str ? "a-str" : dir ? "a-dir" : mn ? "a-mn" : num ? "a-num"
        : ref ? "a-ref" : reg ? "a-reg" : lbl ? "a-lbl" : tgt ? "a-ref" : "";
      return cls ? `<span class="${cls}">${m}</span>` : m;
    });
    html = html.replace(/(<span class="a-mn">call<\/span>)(\s+)([\w.?@]+)/,
      '$1$2<span class="a-api">$3</span>');
    if (cmt) html += `<span class="a-cmt">${esc(cmt)}</span>`;
    return html;
  }).join("\n");
}

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
        const spec = specOf(nd.api, nd.module);
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
    const spec = specOf(node.api, node.module);
    const params = spec ? spec.params : [];
    if (node.module) dataLines.push(`mod_${m.n}: du "${node.module}", 0`);
    if (node.path) dataLines.push(`modp_${m.n}: du "${node.path}", 0`);
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
  const modBySlot = new Map();
  const pathBySlot = new Map();
  for (const { t } of lines) {
    if (/^section\b/.test(t)) { inData = /\bdata\b/.test(t); continue; }
    if (!inData || !t) continue;
    let m = t.match(/^(mod_\d+):\s*du\s+"(.*)",\s*0$/);
    if (m) { modBySlot.set(m[1], m[2]); continue; }
    m = t.match(/^(modp_\d+):\s*du\s+"(.*)",\s*0$/);
    if (m) { pathBySlot.set(m[1], m[2]); continue; }
    m = t.match(/^(str_\d+):\s*du\s+"(.*)",\s*0$/);
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
    const module = modBySlot.get(`mod_${n}`) || null;
    const spec = specOf(api, module);
    if (!spec) return err(lines[j].ln, lines[j].t, `${api} 不在白名单或动态库中`);
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
    const node = { kind: "call", api, args };
    if (module) node.module = module;
    const pth = pathBySlot.get(`modp_${n}`);
    if (pth) node.path = pth;
    return { node, idx: j + 1 };
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
        <ui-tabs id="pg-tabs" strip>
          <span data-value="call" label="调用卡"></span>
          <span data-value="queue" label="编排"></span>
          <span data-value="asm" label="汇编"></span>
        </ui-tabs>
        <div class="pg-body" id="pg-body"></div>
      </div>
    </div>`;

  const cat = await apiGet("/api/winapi/catalog");
  const apis = cat.apis;
  // 导出库：增量累积（扫描中随到随收），模块导出按页缓存
  let libModules = [];
  let libStatus = "idle";
  let libProgress = { done: 0, total: 0 };
  let libTotalExports = 0;
  const MODULE_CAP = 250;
  const moduleCache = new Map();   // name -> {rows, total, done, loading}
  const expanded = new Set(["control", "curated", "dlls"]);

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
    refreshCompose();
  });

  let selected = null;                       // 当前选中的 spec
  let program = [];                          // 编排程序树（块：call/repeat/if/break/continue）
  let session = null;                        // 最近一次执行结果
  let sessions = [];                         // 历史会话（服务端内存）
  let editingPath = null;                    // 正在编辑参数的 call 块路径（JSON 字符串）
  let dragState = null;                      // {arr, idx, mark, markEl}

  const specByName = new Map(apis.map((a) => [a.name, a]));

  /** 自动发现导出的通用签名：4 个指针宽度参数，子进程隔离执行。 */
  function dynSpec(name, module) {
    return {
      name, module: module || "?", desc: `通用调用 ${module}!${name}`,
      side: "dynamic", ret: "int", _dynamic: true,
      params: [1, 2, 3, 4].map((i) => ({
        name: `arg${i}`, type: "int", default: "0",
        desc: `第 ${i} 个参数（整数地址 / 文本 / null）`,
      })),
    };
  }
  function specFor(node) {
    const name = typeof node === "string" ? node : node?.api;
    const module = typeof node === "string" ? null : node?.module;
    return specByName.get(name) || (module ? dynSpec(name, module) : null);
  }
  const asmSpecOf = (name, module) => specFor({ api: name, module });

  /* ---- 编排 ⇄ 汇编 同步管理（同一程序的两种形态） ---- */
  let asmText = programToAsm([], asmSpecOf).text;
  let asmDirty = false;                      // true = 汇编侧有无法还原的编辑（脱钩）
  let asmErr = null;                         // {ln, line, msg}
  let asmTimer = null;

  function syncSig(nodes) {
    for (const nd of nodes || []) {
      if ((nd.kind === "call" || !nd.kind) && !nd.sig) nd.sig = sigOf(nd.api, nd.args, nd.module);
      syncSig(nd.body); syncSig(nd.then); syncSig(nd.else);
    }
  }

  /** 积木侧发生改动：以积木为准重新生成汇编（并解除脱钩）。 */
  function programChanged() {
    asmText = programToAsm(program, asmSpecOf).text;
    asmDirty = false;
    asmErr = null;
    if (currentTab === "asm") {
      const ta = document.getElementById("pg-asm-ta");
      if (ta) {
        ta.value = asmText;
        const pre = document.getElementById("pg-asm-hl");
        if (pre) pre.innerHTML = highlightAsm(asmText) + "\n";
      }
    }
    updateAsmStatus();
  }

  /** 汇编侧编辑：尝试还原为积木；成功则整体替换并复联，失败则脱钩。 */
  function tryAsm() {
    const r = asmToProgram(asmText, asmSpecOf);
    if (r.ok) {
      syncSig(r.program);
      program = r.program;
      editingPath = null;
      asmDirty = false;
      asmErr = null;
      if (currentTab === "queue") { countSlots(); refreshCompose(); }
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

  function sigOf(name, args, module) {
    const spec = specFor({ api: name, module });
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
  function makeCall(name, module) {
    const spec = specFor({ api: name, module });
    const args = defaultArgs(spec);
    const node = { kind: "call", api: name, args, sig: sigOf(name, args, module) };
    if (module) node.module = module;
    return node;
  }
  function makeBlock(kind) {
    if (kind === "repeat") return { kind, times: 3, body: [] };
    if (kind === "if") return { kind, cond: "$1.ret.value == 1", then: [], else: [] };
    return { kind };  // break / continue
  }

  /* ---- 左侧积木库：控制流 / Windows API / DLLs{DLL{导出函数}} ---- */
  const CF_BLOCKS = [
    ["repeat", "循环", "重复 N 次"],
    ["if", "条件", "如果 / 否则"],
    ["break", "跳出", "跳出最近循环"],
    ["continue", "下一轮", "进入下一轮循环"],
  ];
  function groupHead(key, label, meta, barPct = null) {
    const h = document.createElement("button");
    h.type = "button";
    h.className = "pg-group-h";
    h.dataset.open = String(expanded.has(key));
    h.dataset.key = key;
    h.innerHTML = '<svg class="chev" viewBox="0 0 16 16" aria-hidden="true"><path d="M6 3l5 5-5 5"/></svg>'
      + `<span class="t">${esc(label)}</span><span class="pg-group-meta">${esc(meta)}</span>`
      + (barPct != null
        ? `<span class="pg-scan-bar"><i style="width:${barPct}%"></i></span>` : "");
    h.addEventListener("click", () => {
      // 原地开合（grid 0fr→1fr 过渡），不整树重绘
      const g = h.closest(".pg-group");
      if (expanded.has(key)) { expanded.delete(key); g.dataset.open = "false"; h.dataset.open = "false"; }
      else { expanded.add(key); g.dataset.open = "true"; h.dataset.open = "true"; }
    });
    return h;
  }
  /** 可折叠分组容器：header + grid 0fr/1fr body（展开收起带动画）。 */
  function groupEl(key, label, meta, barPct, build) {
    const g = document.createElement("div");
    g.className = "pg-group";
    g.dataset.key = key;
    g.dataset.open = String(expanded.has(key));
    g.appendChild(groupHead(key, label, meta, barPct));
    const body = document.createElement("div");
    body.className = "pg-group-body";
    const inner = document.createElement("div");
    inner.className = "pg-group-inner";
    body.appendChild(inner);
    build(inner);
    g.appendChild(body);
    return g;
  }
  /** 分页拉取某 DLL 的导出（每页 100 条，缓存累积到 entry.rows）。 */
  function loadModulePage(name, offset) {
    let entry = moduleCache.get(name);
    if (!entry) {
      entry = { rows: [], total: 0, done: false, loading: false };
      moduleCache.set(name, entry);
    }
    if (entry.loading || (offset > 0 && entry.done)) return Promise.resolve(entry);
    entry.loading = true;
    return apiGet(`/api/winapi/library/module?name=${encodeURIComponent(name)}&offset=${offset}&limit=100`)
      .then((r) => {
        entry.rows = entry.rows.concat(r.exports || []);
        entry.total = r.total ?? entry.rows.length;
        entry.done = entry.rows.length >= entry.total;
        return entry;
      })
      .catch(() => { entry.done = true; return entry; })
      .finally(() => { entry.loading = false; });
  }
  /** DLL 行：头 + 折叠体；首次展开时按需填充导出（分页）。 */
  function moduleEl(m) {
    const key = `dll:${m.name}`;
    const wrap = document.createElement("div");
    wrap.className = "pg-mod";
    wrap.dataset.key = key;
    wrap.dataset.open = String(expanded.has(key));
    const h = document.createElement("button");
    h.type = "button";
    h.className = "pg-mod-h";
    h.dataset.open = String(expanded.has(key));
    h.innerHTML = `<span class="chev">▸</span><span class="nm">${esc(m.name)}.dll</span>`
      + `<span class="pg-group-meta">${m.count}</span>`;
    h.addEventListener("click", () => {
      if (expanded.has(key)) { expanded.delete(key); wrap.dataset.open = "false"; h.dataset.open = "false"; }
      else { expanded.add(key); wrap.dataset.open = "true"; h.dataset.open = "true"; fillModule(wrap, m); }
    });
    const body = document.createElement("div");
    body.className = "pg-group-body";
    body.appendChild(Object.assign(document.createElement("div"), { className: "pg-group-inner" }));
    wrap.append(h, body);
    if (expanded.has(key)) fillModule(wrap, m);
    return wrap;
  }
  /** 就地填充某 DLL 的导出列表（含分页加载更多）。 */
  function fillModule(wrap, m) {
    const inner = wrap.querySelector(".pg-group-inner");
    const key = wrap.dataset.key;
    const q = searchEl.value.trim().toLowerCase();
    let entry = moduleCache.get(m.name);
    if (!entry) {
      inner.replaceChildren(Object.assign(document.createElement("div"),
        { className: "table-meta", textContent: "读取导出表…" }));
      loadModulePage(m.name, 0).then(() => {
        if (expanded.has(key)) fillModule(wrap, m);
      });
      return;
    }
    const items = [];
    for (const ex of entry.rows) {
      if (q && !ex.name.toLowerCase().includes(q) && !m.name.includes(q)) continue;
      items.push(exportItem(ex, m.name, q));
    }
    if (!entry.done) {
      const btn = document.createElement("button");
      btn.type = "button";
      btn.className = "pg-more-btn";
      btn.textContent = entry.loading
        ? "加载中…"
        : `加载更多（${entry.rows.length}/${entry.total}）`;
      btn.disabled = entry.loading;
      btn.addEventListener("click", () => {
        loadModulePage(m.name, entry.rows.length).then(() => {
          if (expanded.has(key)) fillModule(wrap, m);
        });
      });
      items.push(btn);
    }
    inner.replaceChildren(...items);
  }
  function exportItem(ex, module, filter) {
    const d = document.createElement("div");
    d.className = "pg-item pg-export" + (ex.trust === "restricted" ? " restricted" : "")
      + (ex.trust === "blocked" ? " blocked" : "");
    d.innerHTML = `<span class="nm">${esc(ex.name)}</span>`
      + `<span class="mod">#${ex.ordinal}</span>`
      + `<span class="pg-badge${ex.trust === "restricted" ? " warn" : ""}">`
      + (ex.trust === "blocked" ? "拦截" : ex.trust === "restricted" ? "谨慎" : "通用")
      + "</span>";
    d.title = ex.trust === "blocked"
      ? `${module}!${ex.name}：命中高危名称黑名单，后端拒绝执行`
      : `${module}!${ex.name}：通用指针签名（最多 4 个参数），在独立子进程执行`
        + (ex.trust === "restricted" ? "；名称无法确认只读，请谨慎" : "");
    if (ex.trust !== "blocked") {
      d.draggable = true;
      d.addEventListener("dragstart", (e) => {
        e.dataTransfer.setData("text/pg-dyn",
          JSON.stringify({ name: ex.name, module, trust: ex.trust }));
        e.dataTransfer.effectAllowed = "copy";
        showTab("queue");
      });
    }
    return d;
  }
  function renderCatalog(filter = "") {
    const q = filter.trim().toLowerCase();

    // 1) 控制流积木
    catalogEl.replaceChildren(
      groupEl("control", "控制流", `${CF_BLOCKS.length} 块`, null, (inner) => {
        for (const [kind, label, desc] of CF_BLOCKS) {
          if (q && !label.includes(q) && !kind.includes(q)) continue;
          const d = document.createElement("div");
          d.className = `pg-item pg-cf pg-cf-${kind}`;
          d.innerHTML = `<span class="nm">${label}</span><span class="mod">${desc}</span>`
            + '<span class="pg-badge">积木</span>';
          d.title = "拖到右侧编排画布；也可在画布内拖动排序、拖回左栏删除";
          d.draggable = true;
          d.addEventListener("dragstart", (e) => {
            e.dataTransfer.setData("text/pg-block", kind);
            e.dataTransfer.effectAllowed = "copy";
            showTab("queue");
          });
          inner.appendChild(d);
        }
      }),
    );

    // 2) 已声明签名的 Windows API
    const curated = q ? apis.filter((a) =>
      a.name.toLowerCase().includes(q) || a.module.toLowerCase().includes(q)) : apis;
    catalogEl.appendChild(groupEl("curated", "Windows API", `${curated.length} 个可执行`, null, (inner) => {
      for (const a of curated) {
        const d = document.createElement("div");
        d.className = "pg-item" + (selected && selected.name === a.name ? " sel" : "");
        d.innerHTML = `<span class="nm">${esc(a.name)}</span><span class="mod">${esc(a.module)}</span>`
          + `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">`
          + `${a.side === "observable" ? "有副作用" : "safe"}</span>`;
        d.addEventListener("click", () => {
          selected = a;
          renderCatalog(searchEl.value);
          if (currentTab === "call") swap(renderCallCard());
        });
        d.draggable = true;
        d.addEventListener("dragstart", (e) => {
          e.dataTransfer.setData("text/pg-api", a.name);
          e.dataTransfer.effectAllowed = "copy";
          showTab("queue");
        });
        inner.appendChild(d);
      }
    }));

    // 3) 自动发现的 DLL 导出（扫描中增量显示；导出按页懒加载）
    const mods = libModules.filter((m) => !q || m.name.includes(q));
    const cap = q ? 40 : MODULE_CAP;
    const pct = libProgress.total ? Math.round((libProgress.done / libProgress.total) * 100) : 0;
    const meta = libStatus === "scanning"
      ? `扫描中 ${libProgress.done}/${libProgress.total}`
      : `${libModules.length} DLL · ${libTotalExports} 导出`;
    catalogEl.appendChild(groupEl("dlls", "DLLs", meta, libStatus === "scanning" ? pct : null, (inner) => {
      if (!libModules.length && libStatus === "scanning") {
        inner.appendChild(Object.assign(document.createElement("div"),
          { className: "table-meta", textContent: "正在扫描 System32 的 PE 导出表…" }));
        return;
      }
      for (const m of mods.slice(0, cap)) inner.appendChild(moduleEl(m));
      if (mods.length > cap) {
        inner.appendChild(Object.assign(document.createElement("div"),
          { className: "table-meta pg-more-hint",
            textContent: `还有 ${mods.length - cap} 个 DLL，输入名称搜索可定位。` }));
      }
    }));
  }
  searchEl.addEventListener("input", () => renderCatalog(searchEl.value));
  /** 扫描中的 DLLs 分组头原地刷新（计数 + 进度条），不整树重绘。 */
  function updateDllsHead() {
    const h = catalogEl.querySelector('.pg-group-h[data-key="dlls"]');
    if (!h) return;
    const pct = libProgress.total ? Math.round((libProgress.done / libProgress.total) * 100) : 0;
    h.querySelector(".pg-group-meta").textContent =
      `扫描中 ${libProgress.done}/${libProgress.total}`;
    const bar = h.querySelector(".pg-scan-bar > i");
    if (bar) bar.style.width = `${pct}%`;
  }
  /** 把新到的模块行追加到现有树尾（cap 内），超出只更新提示。 */
  function appendModuleRows(fresh) {
    const dllsInner = catalogEl.querySelector('.pg-group[data-key="dlls"] .pg-group-inner');
    if (!dllsInner) return;
    const shown = dllsInner.querySelectorAll(".pg-mod-h").length;
    const room = Math.max(0, MODULE_CAP - shown);
    if (room > 0) {
      const frag = document.createDocumentFragment();
      fresh.slice(0, room).forEach((m, i) => {
        const w = moduleEl(m);
        w.classList.add("pg-pop");   // 新行错峰浮入
        w.style.animationDelay = `${Math.min(i, 12) * 24}ms`;
        frag.appendChild(w);
      });
      const hint = dllsInner.querySelector(".pg-more-hint");
      if (hint) dllsInner.insertBefore(frag, hint); else dllsInner.appendChild(frag);
    }
    syncMoreHint(dllsInner);
    updateDllsHead();
  }
  function syncMoreHint(dllsInner) {
    if (!dllsInner) return;
    const shown = dllsInner.querySelectorAll(".pg-mod-h").length;
    let hint = dllsInner.querySelector(".pg-more-hint");
    const overflow = libModules.length - shown;
    if (overflow > 0 && expanded.has("dlls") && !searchEl.value.trim()) {
      if (!hint) {
        hint = document.createElement("div");
        hint.className = "table-meta pg-more-hint";
        dllsInner.appendChild(hint);
      }
      hint.textContent = `还有 ${overflow} 个 DLL，输入名称搜索可定位。`;
    } else if (hint) hint.remove();
  }
  /** 拉取库状态；since 增量，扫描中随到随显示。 */
  async function refreshLibrary(deltaOnly = false) {
    const prev = libStatus;
    try {
      const r = await apiGet(`/api/winapi/library?since=${libModules.length}`);
      libStatus = r.status;
      libProgress = r.progress || libProgress;
      libTotalExports = r.total_exports ?? libTotalExports;
      const fresh = r.modules || [];
      if (fresh.length) {
        libModules = libModules.concat(fresh);
        if (deltaOnly && libStatus === "scanning" && expanded.has("dlls")
            && !searchEl.value.trim() && catalogEl.querySelectorAll(".pg-mod-h").length) {
          appendModuleRows(fresh);
        } else {
          renderCatalog(searchEl.value);
        }
      } else if (prev !== libStatus) {
        renderCatalog(searchEl.value);   // 状态翻转（如扫描完成）时整树刷新一次
      }
    } catch { /* keep last */ }
    return libStatus;
  }
  let libTimer = null;
  function pollLibrary() {
    if (libTimer) return;
    libTimer = setTimeout(async () => {
      libTimer = null;
      const st = await refreshLibrary(true);
      if (st === "scanning") pollLibrary();
    }, 800);
  }

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
    wrap.classList.add("motion-stagger");   // 步骤卡按 --ui-index 依次浮入
    assignIndices(wrap);
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
    const badge = a._dynamic
      ? '<span class="pg-badge warn">动态 · 子进程隔离</span>'
      : `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">${a.side === "observable" ? "有副作用" : "safe"}</span>`;
    head.innerHTML = `${esc(a.module)}!<b>${esc(a.name)}</b> ${badge}`;
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
      const call = { api: a.name, args };
      if (a._dynamic && a.module) call.module = a.module;
      if (a._path) call.path = a._path;
      try {
        const s = await apiPost("/api/winapi/run", { calls: [call] });
        out.replaceChildren(renderSession(s));
      } catch (err) {
        out.innerHTML = `<div class="error-box"><span class="code">${esc(err.code)}</span> — ${esc(err.message)}</div>`;
      }
    });
    addBtn.addEventListener("click", () => {
      const args = collectArgs(a, panel);
      const node = makeCall(a.name, a._dynamic ? a.module : undefined);
      if (a._path) node.path = a._path;
      program.push(node);
      programChanged();
      showTab("queue");
    });

    wrap.append(panel, out);
    return wrap;
  }

  /* ---- 调用块的紧凑参数编辑器（点击块头展开） ---- */
  function buildEditor(node, slotNo) {
    const spec = specFor(node);
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
        node.sig = sigOf(node.api, node.args, node.module);
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
    return [...e.dataTransfer.types].some((t) =>
      t === "text/pg-api" || t === "text/pg-move" || t === "text/pg-dyn" || t === "text/pg-block");
  }

  function doDrop(e) {
    const t = dragState;
    dragState = null;
    clearDropMarks();
    if (!t) return;
    let node;
    const apiName = e.dataTransfer.getData("text/pg-api");
    const dyn = e.dataTransfer.getData("text/pg-dyn");
    const blockKind = e.dataTransfer.getData("text/pg-block");
    if (apiName) {
      node = makeCall(apiName);
    } else if (dyn) {
      try {
        const info = JSON.parse(dyn);
        node = makeCall(info.name, info.module);
        node.trust = info.trust;
      } catch { /* ignore malformed drag payload */ }
    } else if (blockKind) {
      node = makeBlock(blockKind);
    } else {
      const fromPath = JSON.parse(e.dataTransfer.getData("text/pg-move"));
      node = takeAt(fromPath);
      const fromArr = fromPath.length === 1 ? program : getArr(fromPath.slice(0, -1));
      const fromIdx = fromPath[fromPath.length - 1];
      if (fromArr === t.arr && fromIdx < t.idx) t.idx--;
    }
    if (node) t.arr.splice(t.idx, 0, node);
    programChanged();
    refreshCompose();
  }

  /** 通用投放区：getTarget(e) → {arr, idx, mark, markEl}。 */
  function attachDropZone(el, getTarget) {
    el.addEventListener("dragover", (e) => {
      if (!isProgDrag(e)) return;
      e.preventDefault();
      e.stopPropagation();
      // dropEffect 必须落在 effectAllowed 内，否则 Chrome 判定不可放置：
      // pg-move 拖动是 move，其余（pg-api/pg-dyn/pg-block）都是 copy
      e.dataTransfer.dropEffect =
        e.dataTransfer.types.includes("text/pg-move") ? "move" : "copy";
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

  // 积木稳定身份：重渲染后同一节点对象仍可对应到旧 DOM 位置（FLIP 动画用）
  const uidMap = new WeakMap();
  let uidSeq = 0;
  function uidOf(node) {
    let u = uidMap.get(node);
    if (!u) { u = `u${++uidSeq}`; uidMap.set(node, u); }
    return u;
  }

  function renderBlock(node, path) {
    const kind = node.kind || "call";
    const blk = document.createElement("div");
    blk.className = "pg-blk pg-" + (kind === "call" ? "call" : kind);
    blk.dataset.path = JSON.stringify(path);
    blk.dataset.uid = uidOf(node);
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
      hd.addEventListener("click", () => {
        editingPath = isEditing ? null : JSON.stringify(path);
        refreshCompose();
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
        addElse.addEventListener("click", () => { node.else = []; programChanged(); refreshCompose(); });
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
    list.className = "pg-canvas";
    renderBlocks(program, list, []);
    if (!program.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "编排为空：从左侧积木库拖入 API 或控制流块。";
      list.appendChild(empty);
    }
    panel.appendChild(list);

    // 根列表 = 投放区（追加到末尾）
    attachDropZone(list, () => ({ arr: program, idx: program.length, mark: "pg-into", markEl: list }));

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0;flex-wrap:wrap";
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
      refreshCompose();
    });
    const clrBtn = document.createElement("ui-button");
    clrBtn.setAttribute("label", "清空");
    clrBtn.addEventListener("click", () => {
      program = []; session = null; editingPath = null;
      programChanged(); refreshCompose();
    });
    const nameInput = document.createElement("input");
    nameInput.className = "filter-input";
    nameInput.type = "text";
    nameInput.placeholder = "编排名称（保存到 SQLite）";
    nameInput.style.cssText = "width:180px;margin:0";
    const saveBtn = document.createElement("ui-button");
    saveBtn.setAttribute("label", "保存");
    saveBtn.addEventListener("click", async () => {
      const name = (nameInput.value || "").trim()
        || `编排 ${new Date().toLocaleString("zh-CN", { hour12: false })}`;
      try {
        await apiPost("/api/winapi/programs", { name, program, asm_text: asmText });
        nameInput.value = name;
        saveBtn.setAttribute("label", "已保存");
        setTimeout(() => saveBtn.setAttribute("label", "保存"), 1600);
      } catch (err) {
        saveBtn.setAttribute("label", "保存失败");
        setTimeout(() => saveBtn.setAttribute("label", "保存"), 1600);
      }
    });
    btns.append(runBtn, clrBtn, nameInput, saveBtn);
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
    ta.spellcheck = false;
    ta.setAttribute("wrap", "off");
    ta.style.cssText = "height:420px;resize:vertical";
    const pre = document.createElement("pre");
    pre.id = "pg-asm-hl";
    const editor = document.createElement("div");
    editor.className = "asm-editor";
    editor.append(pre, ta);
    const syncHl = () => {
      pre.innerHTML = highlightAsm(ta.value) + "\n";
      pre.scrollTop = ta.scrollTop;
      pre.scrollLeft = ta.scrollLeft;
    };
    ta.value = asmText;
    ta.addEventListener("input", () => {
      asmText = ta.value;
      syncHl();
      if (asmTimer) clearTimeout(asmTimer);
      asmTimer = setTimeout(tryAsm, 500);
    });
    ta.addEventListener("scroll", () => {
      pre.scrollTop = ta.scrollTop;
      pre.scrollLeft = ta.scrollLeft;
    });
    syncHl();
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
    panel.append(editor, btnRow, out);
    wrap.appendChild(panel);
    return wrap;
  }

  /* ---- tabs：横行多选一（切换走 motion.js 交叉出入场） ---- */
  let currentTab = "call";
  function swap(node, animate = true) {
    bodyEl.replaceChildren(node);
    if (animate && motionScale() > 0) {
      node.classList.add("swap-in");
      node.addEventListener("animationend", (e) => {
        if (e.target === node && e.animationName === "ntl-swap-in") node.classList.remove("swap-in");
      }, { once: true });
    }
    return node;
  }

  /** 编排内容变化：整画布不做入场，只对因插入/移除而移位的积木做 FLIP 位移。 */
  function refreshCompose() {
    const scale = motionScale();
    const oldRects = new Map();
    if (scale > 0) {
      bodyEl.querySelectorAll(".pg-blk").forEach((el) => {
        oldRects.set(el.dataset.uid, el.getBoundingClientRect());
      });
    }
    swap(composeView(), false);
    if (scale === 0) return;
    const easing = "cubic-bezier(0.16, 1, 0.3, 1)";
    bodyEl.querySelectorAll(".pg-blk").forEach((el) => {
      const prev = oldRects.get(el.dataset.uid);
      if (!prev) return;                       // 新插入的块：不做入场
      const now = el.getBoundingClientRect();
      const dx = prev.left - now.left;
      const dy = prev.top - now.top;
      if (Math.abs(dx) < 1 && Math.abs(dy) < 1) return;
      el.animate(
        [{ transform: `translate(${dx}px, ${dy}px)` }, { transform: "none" }],
        { duration: 260 * scale, easing },
      );
    });
  }
  function viewFor(v) {
    if (v === "call") return renderCallCard();
    if (v === "queue") return composeView();
    const node = asmView();
    queueMicrotask(updateAsmStatus);   // swapContent 挂载后再刷新徽标
    return node;
  }
  function showTab(v) {
    currentTab = v;
    tabsEl.setAttribute("value", v);
    swapContent(bodyEl, () => viewFor(v), { outMs: 90, inMs: 320 });
  }
  tabsEl.addEventListener("ui-select", (e) => showTab(e.detail.value));

  /* ---- 会话历史：osu ExpandingContainer 式展开/收起 ---- */
  const histToggle = document.createElement("button");
  histToggle.type = "button";
  histToggle.className = "ntl-action-btn ntl-toggle-btn";
  histToggle.setAttribute("aria-expanded", "false");
  histToggle.innerHTML = '历史会话 <svg class="chev" viewBox="0 0 16 16" aria-hidden="true"><path d="M3 6l5 5 5-5"/></svg>';
  const histPanel = document.createElement("div");
  histPanel.className = "ntl-collapse";
  const histInner = document.createElement("div");
  histInner.className = "ntl-collapse-inner";
  histPanel.appendChild(histInner);
  histToggle.addEventListener("click", async () => {
    const open = histToggle.getAttribute("aria-expanded") !== "true";
    histToggle.setAttribute("aria-expanded", String(open));
    histPanel.classList.toggle("open", open);
    if (!open) return;
    histInner.innerHTML = '<div class="loading-row"><ui-spinner></ui-spinner> 读取会话…</div>';
    try {
      const r = await apiGet("/api/winapi/sessions");
      sessions = r.sessions ?? [];
      if (!sessions.length) {
        histInner.innerHTML = '<p class="table-meta">服务器内存中还没有历史会话。</p>';
      } else {
        const h = document.createElement("h3");
        h.textContent = `历史会话（最近 ${sessions.length} 次，仅存内存）`;
        h.style.margin = "0 0 10px";
        histInner.replaceChildren(h, ...sessions.map((s) => renderSession(s)));
      }
    } catch (err) {
      histInner.innerHTML = `<div class="error-box"><span class="code">${esc(err.code ?? "error")}</span> — ${esc(err.message ?? String(err))}</div>`;
    }
  });
  el.querySelector(".page-head").append(histToggle, histPanel);

  /* ---- 我的编排：SQLite 持久化保存/载入/删除 ---- */
  const myToggle = document.createElement("button");
  myToggle.type = "button";
  myToggle.className = "ntl-action-btn ntl-toggle-btn";
  myToggle.setAttribute("aria-expanded", "false");
  myToggle.innerHTML = '我的编排 <svg class="chev" viewBox="0 0 16 16" aria-hidden="true"><path d="M3 6l5 5 5-5"/></svg>';
  const myPanel = document.createElement("div");
  myPanel.className = "ntl-collapse";
  const myInner = document.createElement("div");
  myInner.className = "ntl-collapse-inner";
  myPanel.appendChild(myInner);
  myToggle.addEventListener("click", async () => {
    const open = myToggle.getAttribute("aria-expanded") !== "true";
    myToggle.setAttribute("aria-expanded", String(open));
    myPanel.classList.toggle("open", open);
    if (!open) return;
    myInner.innerHTML = '<div class="loading-row"><ui-spinner></ui-spinner> 读取保存的编排…</div>';
    try {
      const r = await apiGet("/api/winapi/programs");
      const list = r.programs ?? [];
      if (!list.length) {
        myInner.innerHTML = '<p class="table-meta">还没有保存的编排。在「编排」页输入名称并点「保存」。</p>';
        return;
      }
      const h = document.createElement("h3");
      h.textContent = `我的编排（${list.length} 份，SQLite 持久化）`;
      h.style.margin = "0 0 10px";
      const rows = list.map((p) => {
        const row = document.createElement("div");
        row.className = "pg-item my-program-row";
        const t = new Date(p.updated_at * 1000).toLocaleString();
        row.innerHTML = `<span class="nm">${esc(p.name)}</span>`
          + `<span class="mod">${p.program.length} 块 · ${esc(t)}</span>`;
        row.title = `asm 同步文本 ${p.asm_text ? "已存" : "未存"} · id ${p.id}`;
        const btns = document.createElement("span");
        btns.style.cssText = "display:flex;gap:6px";
        const loadBtn = document.createElement("ui-button");
        loadBtn.setAttribute("label", "载入");
        loadBtn.addEventListener("click", () => {
          program = p.program.map((n) => JSON.parse(JSON.stringify(n)));
          if (p.asm_text) { asmText = p.asm_text; asmDirty = false; }
          editingPath = null; session = null;
          programChanged();
          showTab("queue");
        });
        const delBtn = document.createElement("ui-button");
        delBtn.setAttribute("label", "删除");
        delBtn.setAttribute("variant", "danger");
        delBtn.addEventListener("click", async () => {
          try {
            await apiSend("DELETE", `/api/winapi/programs/${p.id}`);
            myToggle.click(); myToggle.click();   // 收起再展开刷新列表
          } catch (err) { /* keep list */ }
        });
        btns.append(loadBtn, delBtn);
        row.appendChild(btns);
        return row;
      });
      myInner.replaceChildren(h, ...rows);
    } catch (err) {
      myInner.innerHTML = `<div class="error-box"><span class="code">${esc(err.code ?? "error")}</span> — ${esc(err.message ?? String(err))}</div>`;
    }
  });
  el.querySelector(".page-head").append(myToggle, myPanel);

  /* ---- 启动 ---- */
  renderCatalog();
  showTab("call");
  refreshLibrary().then(() => {
    renderCatalog(searchEl.value);
    if (libStatus === "scanning") pollLibrary();
  });

  // PE 分析器跳转：#/playground?api=CreateFileW
  // 或 #/playground?api=X&module=dll[&path=C:\...]（任意导出，含非系统 DLL）
  const pre = query().get("api");
  if (pre) {
    const spec = apis.find((a) => a.name.toLowerCase() === pre.toLowerCase());
    const mod = query().get("module");
    const pth = query().get("path");
    if (spec) {
      selected = spec;
    } else if (mod) {
      selected = dynSpec(pre, mod);
      selected.desc = "来自 PE 分析器的动态导出"
        + (pth ? `（按路径加载：${pth}）` : `（${mod}!${pre}，需在导出库中）`);
      if (pth) selected._path = pth;
    }
    if (selected) {
      renderCatalog(searchEl.value);
      tabsEl.setAttribute("value", "call");
      showTab("call");
    }
  }
}
