(() => {
  window.__oiIde.editor.toggleBreakpoint(9);
  const bps = window.__oiIde.editor.getBreakpoints();
  document.getElementById('ide-debug').click();
  return 'bps=' + JSON.stringify(bps);
})()
