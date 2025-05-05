import numpy as np
from matplotlib import pyplot as plt

functions = ['asin', 'cos', 'exp', 'log', 'sin', 'sqrt']

def load_data(filename, diffy=None):
    x = []
    y = []
    with open(filename) as f:
        for line in f:
            cx, cy = line.split("\t")
            x.append(float(cx))
            y.append(float(cy))
    x = np.array(x, dtype=np.double)
    y = np.array(y, dtype=np.double)
    if diffy is not None:
        y = np.abs(y - diffy)
    return (x, y)

def draw_plots(function):
    ref_x, ref_y = load_data(f'double_std_{function}.txt')
    plt.figure(figsize=(6,4))
    plt.plot(*load_data(f'float_std_{function}.txt', ref_y), label='float std', linestyle='-', alpha=0.7)
    plt.plot(*load_data(f'float_taylor_{function}.txt', ref_y), label='float taylor', linestyle='--', alpha=0.7)
    plt.plot(*load_data(f'double_taylor_{function}.txt', ref_y), label='double taylor', linestyle='-.', alpha=0.7)
    plt.plot(*load_data(f'fixed32_s_taylor_{function}.txt', ref_y), label='fixed32_s', linestyle=':', alpha=0.7, marker='*', markersize=5)
    plt.plot(*load_data(f'fixed32_a_taylor_{function}.txt', ref_y), label='fixed32_a', linestyle=(0, (3, 1, 1, 1)), alpha=0.7, marker='o', markersize=4)
    plt.plot(*load_data(f'fixed64_taylor_{function}.txt', ref_y), label='fixed64', linestyle=(0, (5, 2)), alpha=0.7, marker='x', markersize=4)
    plt.title(function)
    plt.yscale('log')
    # plt.ylim(1e-10, 1)
    plt.legend()
    plt.grid(True)
    plt.savefig(f'plot_{function}.png')

for fun in functions:
    draw_plots(fun)