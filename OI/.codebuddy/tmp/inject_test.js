(() => {
  const code = [
    '#include <bits/stdc++.h>',
    'int main(){',
    '    int n;',
    '    std::cin >> n;',
    '    int s = 0;',
    '    for (int i = 0; i < n; ++i) {',
    '        int x;',
    '        std::cin >> x;',
    '        s += x;',
    '    }',
    '    std::cout << s << std::endl;',
    '    std::cerr << 789 << n << std::endl;',
    '    return 0;',
    '}',
  ].join('\n');
  window.__oiIde.setCode(code);
  document.getElementById('dock-stdin').value = '4\n1 2 3 4\n';
  return 'ok';
})()
