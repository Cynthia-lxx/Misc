(async () => {
  // 1. command palette via Ctrl+K
  document.dispatchEvent(new KeyboardEvent('keydown', { key: 'k', ctrlKey: true, bubbles: true }));
  await new Promise(r => setTimeout(r, 300));
  const paletteOpen = !document.getElementById('ov-layer').classList.contains('is-hidden');
  const paletteTitle = (document.querySelector('.ov-head h2') || {}).textContent || '';
  document.dispatchEvent(new KeyboardEvent('keydown', { key: 'Escape', bubbles: true }));
  await new Promise(r => setTimeout(r, 200));
  const paletteClosed = document.getElementById('ov-layer').classList.contains('is-hidden');
  // 2. theme switch
  const sel = document.getElementById('theme-select');
  sel.value = 'purple';
  sel.dispatchEvent(new Event('change'));
  const theme = document.documentElement.dataset.theme;
  sel.value = 'blue';
  sel.dispatchEvent(new Event('change'));
  // 3. goto overlay via event
  window.dispatchEvent(new CustomEvent('oi-goto'));
  await new Promise(r => setTimeout(r, 200));
  const gotoTitle = (document.querySelector('.ov-head h2') || {}).textContent || '';
  document.dispatchEvent(new KeyboardEvent('keydown', { key: 'Escape', bubbles: true }));
  return JSON.stringify({ paletteOpen, paletteTitle, paletteClosed, theme, gotoTitle });
})()
