(async () => {
  document.getElementById('dbg-continue').click();
  await new Promise(r => setTimeout(r, 2500));
  document.getElementById('dbg-continue').click();
  await new Promise(r => setTimeout(r, 2500));
  document.getElementById('dbg-continue').click();
  await new Promise(r => setTimeout(r, 2500));
  return JSON.stringify({
    status: document.getElementById('dbg-status').textContent,
    meta: document.getElementById('dock-meta').textContent,
    stdout: document.getElementById('dock-stdout').textContent,
    stderr: document.getElementById('dock-stderr').textContent,
  });
})()
