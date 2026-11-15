(async () => {
  document.getElementById('dbg-next').click();
  await new Promise(r => setTimeout(r, 2500));
  const afterNext = document.getElementById('dock-meta').textContent;
  document.getElementById('dbg-next').click();
  await new Promise(r => setTimeout(r, 2500));
  const afterNext2 = document.getElementById('dbg-reason').textContent
    + ' | ' + document.getElementById('dbg-vars').textContent.slice(0, 60);
  document.getElementById('dbg-continue').click();
  await new Promise(r => setTimeout(r, 3000));
  return JSON.stringify({
    afterNext,
    afterNext2,
    final: document.getElementById('dbg-status').textContent,
    stdout: document.getElementById('dock-stdout').textContent,
    debugLine: Boolean(document.querySelector('.CodeMirror .oi-debug-line')),
  });
})()
