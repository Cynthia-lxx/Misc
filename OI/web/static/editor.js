/* editor.js — CodeMirror 5 wrapper for the OI cloud IDE.
   Loads the vendored UMD bundles, builds a C++ editor with a breakpoint
   gutter, local shortcuts (Ctrl+S save, Ctrl+G goto line, Ctrl+/ comment)
   and a debug-line highlight. No framework, offline only. */

const VENDOR = 'vendor/codemirror/';
const BREAK_GUTTER = 'oi-breaks';
const DEBUG_CLASS = 'oi-debug-line';

function loadScript(src) {
  return new Promise((resolve, reject) => {
    const el = document.createElement('script');
    el.src = VENDOR + src;
    el.onload = resolve;
    el.onerror = () => reject(new Error(`vendor load failed: ${src}`));
    document.head.appendChild(el);
  });
}

function loadCss(href) {
  return new Promise((resolve, reject) => {
    const el = document.createElement('link');
    el.rel = 'stylesheet';
    el.href = VENDOR + href;
    el.onload = resolve;
    el.onerror = () => reject(new Error(`vendor css load failed: ${href}`));
    document.head.appendChild(el);
  });
}

let readyPromise = null;

/** Ensure CodeMirror + addons are loaded. Resolves with window.CodeMirror. */
function ensureCodeMirror() {
  if (window.CodeMirror) return Promise.resolve(window.CodeMirror);
  if (!readyPromise) {
    readyPromise = (async () => {
      await loadCss('codemirror.min.css');
      await loadScript('codemirror.min.js');
      await Promise.all([
        loadScript('mode-clike.min.js'),
        loadScript('addon-matchbrackets.min.js'),
        loadScript('addon-closebrackets.min.js'),
        loadScript('addon-active-line.min.js'),
        loadScript('addon-comment.min.js'),
      ]);
      return window.CodeMirror;
    })();
  }
  return readyPromise;
}

function breakMarker() {
  const el = document.createElement('div');
  el.className = 'oi-break-dot';
  el.title = '断点';
  return el;
}

/**
 * Create the editor inside `container`.
 * options: {value, onSave, onToggleBreakpoint(line1), onGotoRequest}
 */
async function createEditor(container, options = {}) {
  const CM = await ensureCodeMirror();
  const editor = CM(container, {
    value: options.value || '',
    mode: 'text/x-c++src',
    lineNumbers: true,
    gutters: ['CodeMirror-linenumbers', BREAK_GUTTER],
    matchBrackets: true,
    autoCloseBrackets: true,
    styleActiveLine: true,
    indentUnit: 4,
    tabSize: 4,
    indentWithTabs: true,
    lineWrapping: false,
    extraKeys: {
      'Ctrl-S': () => options.onSave?.(),
      'Ctrl-/': 'toggleComment',
      'Ctrl-G': () => options.onGotoRequest?.(),
      Tab: (cm) => (cm.somethingSelected() ? cm.indentSelection('add') : cm.replaceSelection('\t')),
    },
  });

  // ---- breakpoint gutter ---------------------------------------------
  editor.on('gutterClick', (cm, line, gutterName) => {
    if (gutterName !== BREAK_GUTTER) return;
    const added = toggleBreakpoint(line + 1);
    options.onToggleBreakpoint?.(line + 1, added);
  });

  function clamp(line1) {
    return Math.max(1, Math.min(editor.lineCount(), line1 | 0));
  }

  /** Toggle a 1-based breakpoint marker; returns true if now set. */
  function toggleBreakpoint(line1) {
    const line = clamp(line1) - 1;
    const info = editor.lineInfo(line);
    const has = !!(info.gutterMarkers && info.gutterMarkers[BREAK_GUTTER]);
    editor.setGutterMarker(line, BREAK_GUTTER, has ? null : breakMarker());
    return !has;
  }

  /** Replace all breakpoints with the given 1-based line numbers. */
  function setBreakpoints(lines1) {
    editor.operation(() => {
      for (let i = 0; i < editor.lineCount(); i++) {
        editor.setGutterMarker(i, BREAK_GUTTER, null);
      }
      (lines1 || []).forEach((n) => {
        editor.setGutterMarker(clamp(n) - 1, BREAK_GUTTER, breakMarker());
      });
    });
  }

  /** Sorted 1-based breakpoint lines currently marked in the gutter. */
  function getBreakpoints() {
    const out = [];
    for (let i = 0; i < editor.lineCount(); i++) {
      const info = editor.lineInfo(i);
      if (info.gutterMarkers && info.gutterMarkers[BREAK_GUTTER]) out.push(i + 1);
    }
    return out;
  }

  /** Highlight the line where the debugger currently stopped (1-based, 0 clears). */
  function setDebugLine(line1) {
    if (debugLine !== null) {
      editor.removeLineClass(debugLine - 1, 'background', DEBUG_CLASS);
      debugLine = null;
    }
    if (line1 > 0) {
      const line = clamp(line1);
      editor.addLineClass(line - 1, 'background', DEBUG_CLASS);
      debugLine = line;
      editor.setCursor({ line: line - 1, ch: 0 });
    }
  }

  /** Scroll a 1-based line into view with a brief flash. */
  function gotoLine(line1) {
    const line = clamp(line1);
    editor.setCursor({ line: line - 1, ch: 0 });
    editor.scrollIntoView({ line: line - 1, ch: 0 }, 120);
    const handle = editor.addLineClass(line - 1, 'background', 'oi-flash-line');
    setTimeout(() => editor.removeLineClass(handle, 'background', 'oi-flash-line'), 800);
  }

  editor.on('cursorActivity', () => {
    options.onCursor?.(editor.getCursor().line + 1);
  });

  return {
    cm: editor,
    getValue: () => editor.getValue(),
    setValue: (v) => editor.setValue(v ?? ''),
    refresh: () => editor.refresh(),
    focus: () => editor.focus(),
    toggleBreakpoint,
    setBreakpoints,
    getBreakpoints,
    setDebugLine,
    gotoLine,
    on: (ev, fn) => editor.on(ev, fn),
  };
}

export { createEditor, ensureCodeMirror };
