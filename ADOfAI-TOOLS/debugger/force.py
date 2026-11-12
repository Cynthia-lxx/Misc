import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

# Define sigmoid function for method two
def sigmoid_func(x, L, k, x0):
    return L / (1 + np.exp(-k*(x-x0)))

# Sample data points based on problem statement
x_data = [520, 970, 1040]
y_data = [0.95, None, None]

# Estimate parameters using known point and constraints
popt, pcov = curve_fit(sigmoid_func, x_data[:1], y_data[:1],
                       bounds=((1., 0., min(x_data)),
                               (2., 10., max(x_data))))

# Generate full range of bpm values to plot
x_plot = np.linspace(min(x_data), max(x_data)*2, 1000)
y_plot = sigmoid_func(x_plot, *popt)

plt.figure(figsize=(8,5))
plt.plot(x_plot, y_plot, label="Fitted Curve", color='blue')
plt.scatter(x_data, y_data, color='red', zorder=3, label="Data Points")
plt.axhline(y=0.6, linestyle='--', color='green', linewidth=.8, label="Min Force Limit")
plt.axhline(y=2, linestyle='-.', color='orange', linewidth=.8, label="Max Force Limit")

plt.title("BPM vs Force Relationship with Sigmoid Function")
plt.xlabel("Beats Per Minute (BPM)")
plt.ylabel("Force Applied")
plt.legend()
plt.grid(alpha=.4)
plt.show()
