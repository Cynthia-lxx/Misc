import os
import hashlib

A = r'p:\Dev\ComfyUI_Refs\ComfyUI-original'
B = r'p:\Dev\ComfyUI_Refs\ComfyDL_UI'
EXTS = ('.py', '.json', '.yaml', '.yml', '.md', '.txt', '.toml', '.ini', '.cfg', '.mako', '.pyi')

def sha(p):
    h = hashlib.sha256()
    with open(p, 'rb') as f:
        for chunk in iter(lambda: f.read(65536), b''):
            h.update(chunk)
    return h.hexdigest()

diff = 0
missing = 0
total = 0
for root, dirs, files in os.walk(A):
    if '__pycache__' in root:
        continue
    relroot = os.path.relpath(root, A)
    for f in files:
        if not f.endswith(EXTS):
            continue
        total += 1
        rel = os.path.join(relroot, f) if relroot != '.' else f
        src = os.path.join(root, f)
        dst = os.path.join(B, rel)
        if not os.path.exists(dst):
            print('MISSING in ComfyDL_UI:', rel)
            missing += 1
            continue
        if sha(src) != sha(dst):
            print('DIFF:', rel)
            diff += 1
print(f'checked={total} diffs={diff} missing={missing}')
