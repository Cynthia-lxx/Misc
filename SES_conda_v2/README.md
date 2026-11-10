
# SES Conda 2.1.0 自述文件/README
> 中英双语 | Bilingual

---

## 1. 30 秒上手 / 30-Second Start

```python
# 深度学习回归 / Deep regression
from ses_conda.dlkit import RegressionKit
X, y = np.loadtxt('X.txt'), np.loadtxt('y.txt')
model_fn = RegressionKit('numpy', X_path='X.npy', y_path='y.npy').run()
print(model_fn([[0.5, 1.2]]))

# 经典多项式拟合 / Classic polynomial fit
from ses_conda import expo
e = expo()
e.train([(0, 1), (1, 2.7), (2, 7.4)])
e.display()
```

---

## 2. 功能总览 / Features Overview

| 模块 / Module | 能力 / Capability | 适用场景 / Use-Case |
|---|---|---|
| **RegressionKit** | 任意非线性黑盒逼近 / Universal nonlinear approximation | 大数据、未知解析式 / Big data & unknown formula |
| **expo** | 最优多项式分段拟合 / Optimal polynomial segment fit | 小样本、快速可视化 / Small sample & quick plot |

---

## 3. API 速查 / Quick API

### 3.1 RegressionKit（深度学习 / DL）

```python
kit = RegressionKit(source, **kwargs)
```

| 数据源 / Source | 示例 / Example |
|---|---|
| csv | `RegressionKit('csv', path='data.csv', x_cols=[0,1], y_col=2)` |
| numpy | `RegressionKit('numpy', X_path='X.npy', y_path='y.npy')` |
| sklearn | `RegressionKit('sklearn', name='california')` |
| function | `RegressionKit('func', func=lambda x: np.sin(x))` |

可调关键参数 / Key args  
```python
model_fn = kit.run(
    hidden_dims=(256, 256, 128),
    epochs=800,
    patience=50,
    lr=1e-3,
    batch_size=128,
    standardize=True
)
```

### 3.2 expo（经典 / Classic）

```python
e = expo()
e.train([(x1, y1), (x2, y2), ...])  # 支持批量 / Batch input
e.display()                           # 自动选阶 & 绘图 / Auto degree & plot
```

---

## 4. 更新日志 / Changelog

| 版本 / Version | 日期 / Date | 亮点 / Highlights |
|---|---|---|
| 2.1.0 | 2025-08-13 | 并入 dlkit，新增 RegressionKit / Integrated dlkit, added RegressionKit |
| 2.0.1 | 2025-07-xx | Bug 修复 / Bug fixes |
| 2.0.0-dev | 2025-06-xx | scikit-learn 情感分析优化 / Sentiment analysis improvement |
| 1.x | … | 早期多项式与关系解析 / Early polynomial & relation parse |

---

## 5. 安装 / Installation

```bash
pip install -U ses-conda
```

依赖 / Requirements  
`torch ≥ 1.9`, `scikit-learn ≥ 1.3`, `numpy`, `matplotlib`, `pandas`

---

## 6. 许可证 / License

MIT © 2024-End of the Time EricasZ & 街角的猫_wjz  
详见 LICENSE.txt / See LICENSE.txt for details.

---
