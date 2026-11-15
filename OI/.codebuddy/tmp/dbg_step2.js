(async () => {
  const cmd = (action, extra = {}) =>
    fetch('/api/debug/command', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ session: window.__dbgSid, action, ...extra }),
    }).then(r => r.json());
  // get sid from the DOM state via a state call is impossible without sid;
  // debug.js keeps it private — use the watch UI instead
  document.getElementById('watch-input').value = 's + 100';
  document.getElementById('watch-add').click();
  await new Promise(r => setTimeout(r, 800));
  return JSON.stringify({
    watch: document.getElementById('dbg-watch').textContent,
    line: document.getElementById('dock-meta').textContent,
  });
})()
