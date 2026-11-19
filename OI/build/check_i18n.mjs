/* i18n 键一致性静态核对（无浏览器、无服务）：
   1) index.html 的 data-i18n-key / data-i18n-ph / data-i18n-title
   2) JS 模块里 t('...') / i18n.t('...') 用到的键
   对照 i18n.js en 词典：缺键（仅告警，回退中文原文也能用）、en 缺失即 FAIL */
import { readFileSync, readdirSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const STATIC = 'P:/Dev/OI/web/static';
let fails = 0;

const i18nSrc = readFileSync(join(STATIC, 'i18n.js'), 'utf8');
const enStart = i18nSrc.indexOf('en: {');
const zhStart = i18nSrc.indexOf('zh:');
const enBody = i18nSrc.slice(enStart, zhStart > enStart ? zhStart : undefined);
const dictKeys = new Set([...enBody.matchAll(/'((?:[^'\\]|\\.)*)'\s*:/g)].map((m) => m[1]));
console.log(`i18n.js en 词典键数: ${dictKeys.size}`);

const used = new Set();
const html = readFileSync(join(STATIC, 'index.html'), 'utf8');
for (const m of html.matchAll(/data-i18n(?:-key|-ph|-title)="((?:[^"\\]|\\.)*)"/g)) used.add(m[1]);

const jsFiles = readdirSync(STATIC).filter((f) => f.endsWith('.js') && f !== 'i18n.js');
for (const f of jsFiles) {
  const src = readFileSync(join(STATIC, f), 'utf8');
  for (const m of src.matchAll(/\bt\('((?:[^'\\]|\\.)*)'/g)) used.add(m[1]);
  for (const m of src.matchAll(/\bi18n\.t\('((?:[^'\\]|\\.)*)'/g)) used.add(m[1]);
}

const missing = [...used].filter((k) => !dictKeys.has(k));
const unused = [...dictKeys].filter((k) => !used.has(k) && !k.includes('{') && !['Theme'].includes(k));

if (missing.length) {
  fails += missing.length;
  console.log(`FAIL 缺词典键 ×${missing.length}:`);
  missing.forEach((k) => console.log('  -', k));
} else {
  console.log('PASS 所有使用键均在 en 词典中');
}
console.log(`INFO 未被引用的词典键 ×${unused.length}（多为服务端下发的状态词，回退中文原文）`);
console.log(fails ? `\nFAIL 总计 ${fails}` : '\nPASS i18n 键一致性');
process.exit(fails ? 1 : 0);
