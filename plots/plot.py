import numpy as np
from matplotlib import pyplot as plt

functions = ['asin', 'cos', 'exp', 'log', 'sin', 'sqrt']

def load_data(filename, diffy=None):
    x = []
    y = []
    cnt = []
    with open(filename) as f:
        for line in f:
            cx, cy, ccnt = line.split("\t")
            x.append(float(cx))
            y.append(float(cy))
            cnt.append(int(ccnt))
    x = np.array(x, dtype=np.double)
    y = np.array(y, dtype=np.double)
    if diffy is not None:
        y = np.abs(y - diffy)
    return (x, y, cnt)

def draw_plots(function, make_diff, log_scale, data_slice, plt_suffix=''):
    ref_x, ref_y, counts = load_data(f'double_std_{function}.txt') if make_diff else (None, None, None)
    plt.figure(figsize=(6,4))
    plt.plot(*load_data(f'float_std_{function}.txt', ref_y)[data_slice], label='float std', linestyle='-', alpha=0.7)
    plt.plot(*load_data(f'float_taylor_{function}.txt', ref_y)[data_slice], label='float taylor', linestyle='--', alpha=0.7)
    plt.plot(*load_data(f'double_taylor_{function}.txt', ref_y)[data_slice], label='double taylor', linestyle='-.', alpha=0.7)
    plt.plot(*load_data(f'fixed32_s_taylor_{function}.txt', ref_y)[data_slice], label='fixed32_s', linestyle=':', alpha=0.7, marker='*', markersize=5)
    plt.plot(*load_data(f'fixed32_a_taylor_{function}.txt', ref_y)[data_slice], label='fixed32_a', linestyle=(0, (3, 1, 1, 1)), alpha=0.7, marker='o', markersize=4)
    plt.plot(*load_data(f'fixed64_taylor_{function}.txt', ref_y)[data_slice], label='fixed64', linestyle=(0, (5, 2)), alpha=0.7, marker='x', markersize=4)
    plt.title(function+plt_suffix)
    if log_scale:
        plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'plot_{function}{plt_suffix}.png')
    plt.close()


for fun in functions:
    draw_plots(fun, True, True, slice(None, 2), '')
    draw_plots(fun, False, False, slice(None, None, 2), '_iterations')