(async () => {
  window.dispatchEvent(new CustomEvent('oi-prompt-overlay'));
  await new Promise(r => setTimeout(r, 300));
  const area = document.querySelector('.ov-prompt-area');
  const clipOk = Boolean(area);
  // fill sample then generate
  document.querySelectorAll('.ov-row button').forEach(b => {
    if (b.textContent.includes('示例题面')) b.click();
  });
  await new Promise(r => setTimeout(r, 100));
  const filled = area.value.length > 50;
  document.querySelectorAll('.ov-row button').forEach(b => {
    if (b.textContent.includes('生成')) b.click();
  });
  return JSON.stringify({ clipOk, filled });
})()
