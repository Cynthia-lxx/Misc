#Author:街角的猫_wjz
_debug = False
_suppress_errors = False
_skip_modules = []  # 用户可以在此列表中添加想要跳过的模块名

def Import():
    import pkgutil
    import importlib
    if debug:
        print("Enumerating modules")
    # 获取所有已安装的包
    installed_modules = {name for _, name, _ in pkgutil.iter_modules()}
    if debug:
        print(f"{len(installed_modules)} module(s) found")
    # 导入所有模块，跳过 skip_modules 列表中的模块
    for module_name in installed_modules:
        if module_name in skip_modules:
            if debug:
                print(f"Skipped importing {module_name}")
            continue
        try:
            globals()[module_name] = importlib.import_module(module_name)
            if debug:
                print(f"Successfully imported {module_name}")
        except Exception as e:
            if not suppress_errors:
                print(f"Failed to import {module_name}: {e}")

# 指定 __all__，只导出 Import 函数
__all__ = ['Import']
