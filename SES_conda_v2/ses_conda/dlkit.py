import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
import numpy as np
import matplotlib.pyplot as plt
from typing import Callable, Tuple, Union
import os
import urllib.request
import io
import sys

# ------------------ 1. 通用工具 ------------------

DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")

def set_seed(seed=42):
    torch.manual_seed(seed)
    np.random.seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

set_seed()


class MLPRegressor(nn.Module):
    """
    通用 MLP：支持任意维度输入输出、任意隐藏层。
    """
    def __init__(self, input_dim: int, output_dim: int,
                 hidden_dims=(64, 64), activation=nn.ReLU):
        super().__init__()
        dims = [input_dim, *hidden_dims, output_dim]
        layers = []
        for i in range(len(dims)-2):
            layers += [nn.Linear(dims[i], dims[i+1]), activation()]
        layers += [nn.Linear(dims[-2], dims[-1])]
        self.net = nn.Sequential(*layers)

    def forward(self, x):
        return self.net(x)

def load_data(source, **kwargs) -> Tuple[np.ndarray, np.ndarray]:
    """
    统一数据入口，支持多种 source：
    - 'csv'   ->  load_data('csv', path='data.csv', x_cols=[0,1], y_col=2)
    - 'numpy' ->  load_data('numpy', X_path='X.npy', y_path='y.npy')
    - 'sklearn'-> load_data('sklearn', name='diabetes')  # 或 'california', 'make_reg', ...
    - 'func'  ->  load_data('func', func=my_func, n_samples=1000, noise=0.1, x_dim=2)
    返回 (X, y) : ndarray
    """
    if source == 'csv':
        import pandas as pd
        df = pd.read_csv(kwargs['path'])
        x_cols = kwargs.get('x_cols', slice(None))
        y_col = kwargs.get('y_col', -1)
        X = df.iloc[:, x_cols].values.astype(np.float32)
        y = df.iloc[:, y_col].values.astype(np.float32)
        if y.ndim == 1:
            y = y[:, None]
        return X, y

    elif source == 'numpy':
        X = np.load(kwargs['X_path']).astype(np.float32)
        y = np.load(kwargs['y_path']).astype(np.float32)
        if y.ndim == 1:
            y = y[:, None]
        return X, y

    elif source == 'sklearn':
        name = kwargs['name']
        if name == 'make_reg':
            from sklearn.datasets import make_regression
            X, y = make_regression(n_samples=kwargs.get('n_samples', 1000),
                                   n_features=kwargs.get('n_features', 2),
                                   noise=kwargs.get('noise', 0.1),
                                   random_state=42)
            y = y[:, None]
        else:
            if name == 'diabetes':
                from sklearn.datasets import load_diabetes
                data = load_diabetes()
            elif name == 'california':
                from sklearn.datasets import fetch_california_housing
                data = fetch_california_housing()
            else:
                raise ValueError('unknown sklearn dataset')
            X, y = data.data.astype(np.float32), data.target.astype(np.float32)
            if y.ndim == 1:
                y = y[:, None]
        return X, y

    elif source == 'func':
        func = kwargs['func']
        n_samples = kwargs.get('n_samples', 1000)
        noise = kwargs.get('noise', 0.1)
        x_dim = kwargs.get('x_dim', 1)
        X = np.random.uniform(-1, 1, size=(n_samples, x_dim)).astype(np.float32)
        y_clean = func(X)
        y = y_clean + noise * np.random.randn(*y_clean.shape).astype(np.float32)
        return X, y
    else:
        raise ValueError('unknown source')


def train(model: nn.Module, X: np.ndarray, y: np.ndarray,
          batch_size=64, epochs=300, lr=1e-3, val_split=0.2,
          patience=30, verbose=True) -> nn.Module:
    X, y = torch.from_numpy(X), torch.from_numpy(y)
    idx = torch.randperm(len(X))
    split = int(len(idx) * (1 - val_split))
    train_idx, val_idx = idx[:split], idx[split:]
    ds_train = TensorDataset(X[train_idx], y[train_idx])
    ds_val   = TensorDataset(X[val_idx], y[val_idx])
    dl_train = DataLoader(ds_train, batch_size=batch_size, shuffle=True)
    dl_val   = DataLoader(ds_val, batch_size=batch_size)

    model.to(DEVICE)
    opt = torch.optim.Adam(model.parameters(), lr=lr)
    sched = torch.optim.lr_scheduler.ReduceLROnPlateau(opt, 'min', patience=10, factor=0.5)
    loss_fn = nn.MSELoss()

    best_val = float('inf')
    best_state = None
    bad_epochs = 0

    for epoch in range(epochs):
        # train
        model.train()
        for xb, yb in dl_train:
            xb, yb = xb.to(DEVICE), yb.to(DEVICE)
            opt.zero_grad()
            pred = model(xb)
            loss = loss_fn(pred, yb)
            loss.backward()
            opt.step()
        # val
        model.eval()
        with torch.no_grad():
            val_loss = 0
            for xb, yb in dl_val:
                xb, yb = xb.to(DEVICE), yb.to(DEVICE)
                val_loss += loss_fn(model(xb), yb).item() * len(xb)
            val_loss /= len(ds_val)
        sched.step(val_loss)

        if val_loss < best_val - 1e-6:
            best_val = val_loss
            best_state = model.state_dict().copy()
            bad_epochs = 0
        else:
            bad_epochs += 1
        if verbose and epoch % 50 == 0:
            print(f'Epoch {epoch:>4d} | train MSE {loss.item():.4f} | val MSE {val_loss:.4f}')
        if bad_epochs >= patience:
            if verbose:
                print(f'Early stop at epoch {epoch} (best val MSE: {best_val:.4f})')
            break

    model.load_state_dict(best_state)
    model.eval()
    return model

# ------------------ 5. 一键封装 ------------------

class RegressionKit:
    def __init__(self, source, **kwargs):
        self.X, self.y = load_data(source, **kwargs)
        self.input_dim  = self.X.shape[1]
        self.output_dim = self.y.shape[1]
        self.model = None
        self.scaler_x = None
        self.scaler_y = None

    def _standardize(self):
        from sklearn.preprocessing import StandardScaler
        self.scaler_x = StandardScaler().fit(self.X)
        self.scaler_y = StandardScaler().fit(self.y)
        self.X = self.scaler_x.transform(self.X)
        self.y = self.scaler_y.transform(self.y)

    def _build_model(self, hidden_dims=(128, 128), **net_kwargs):
        return MLPRegressor(self.input_dim, self.output_dim,
                            hidden_dims=hidden_dims, **net_kwargs)

    def run(self, standardize=True, hidden_dims=(128, 128),
            batch_size=64, epochs=300, lr=1e-3, val_split=0.2,
            patience=30, verbose=True):
        if standardize:
            self._standardize()
        self.model = self._build_model(hidden_dims=hidden_dims)
        train(self.model, self.X, self.y,
              batch_size=batch_size, epochs=epochs, lr=lr,
              val_split=val_split, patience=patience, verbose=verbose)

        # 导出推理函数
        def model_fn(x_np: Union[np.ndarray, list]) -> np.ndarray:
            """
            输入 numpy 或 list，返回 numpy。
            已自动完成同样的标准化。
            """
            if isinstance(x_np, list):
                x_np = np.array(x_np, dtype=np.float32)
            if x_np.ndim == 1 and self.input_dim == 1:
                x_np = x_np[:, None]
            if standardize:
                x_np = self.scaler_x.transform(x_np)
            with torch.no_grad():
                x_torch = torch.from_numpy(x_np).to(DEVICE)
                y_torch = self.model(x_torch)
                y_np = y_torch.cpu().numpy()
            if standardize:
                y_np = self.scaler_y.inverse_transform(y_np)
            return y_np.squeeze()

        return model_fn

    def quick_plot(self, model_fn, n_points=1000):
        """
        一维或二维输入时快速可视化。
        """
        if self.input_dim == 1:
            x_plot = np.linspace(self.X.min(), self.X.max(), n_points)[:, None]
            y_pred = model_fn(x_plot)
            plt.scatter(self.X if not self.scaler_x else self.scaler_x.inverse_transform(self.X),
                        self.y if not self.scaler_y else self.scaler_y.inverse_transform(self.y),
                        s=8, label='data', alpha=0.5)
            plt.plot(x_plot if not self.scaler_x else self.scaler_x.inverse_transform(x_plot),
                     y_pred, color='red', label='pred')
            plt.legend()
            plt.show()
        elif self.input_dim == 2:
            from mpl_toolkits.mplot3d import Axes3D
            fig = plt.figure()
            ax = fig.add_subplot(111, projection='3d')
            x1 = np.linspace(self.X[:, 0].min(), self.X[:, 0].max(), 50)
            x2 = np.linspace(self.X[:, 1].min(), self.X[:, 1].max(), 50)
            X1, X2 = np.meshgrid(x1, x2)
            X_plot = np.column_stack([X1.ravel(), X2.ravel()])
            if self.scaler_x:
                X_plot = self.scaler_x.transform(X_plot)
            Y_plot = model_fn(X_plot)
            ax.scatter(self.X[:, 0], self.X[:, 1], self.y[:, 0], alpha=0.3)
            ax.plot_surface(X1, X2, Y_plot.reshape(X1.shape), color='r', alpha=0.5)
            plt.show()

if __name__ == '__main__':
    # python nonlinear_regression_kit.py
    print("Demo: 训练 y = sin(3x) + 0.1*noise")
    kit = RegressionKit('func', func=lambda x: np.sin(3*x), n_samples=1000, noise=0.1, x_dim=1)
    model_fn = kit.run(verbose=True)
    kit.quick_plot(model_fn)