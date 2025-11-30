import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MaxNLocator
from mpl_toolkits.axes_grid1.inset_locator import inset_axes, mark_inset
from sympy.printing.pretty.pretty_symbology import line_width

def plot_furthest_log(standard_file, wrap_file, output_filename):
    try:
        data_std = np.loadtxt(standard_file)
        data_wrap = np.loadtxt(wrap_file)
    except OSError:
        print(f"error: files not found.")
        return
    plt.figure(figsize=(10, 6))

    plt.hist(data_std, bins=100, density=True, histtype='step',
             color='blue', label='standard geometry', linewidth=2.5, zorder=2)
    plt.hist(data_std, bins=100, density=True, histtype="stepfilled",
             color='blue', alpha=0.1, zorder=1)

    plt.hist(data_wrap, bins=100, density=True, histtype='step',
             color='orange', label='wraparound geometry', linewidth=2.5, zorder=3)

    plt.yscale('log')
    plt.title("distribution of distance to furthest neighbour (log scale)")
    plt.xlabel("distance")
    plt.ylabel("probability density (log scale)")

    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.legend()
    plt.tight_layout()

    plt.savefig(output_filename, dpi=300)
    print(f"save graph to {output_filename}")
    plt.show()
def plot_histogram(standard_file, wrap_file, title, output_filename):
    try:
        data_std = np.loadtxt(standard_file)
        data_wrap = np.loadtxt(wrap_file)
    except OSError:
        print(f"Could not find files: {standard_file} or {wrap_file}")
        return

    fig, (ax1, ax2) =  plt.subplots(1, 2, figsize=(12, 5))

    ax1.hist(data_std, bins=50, color='blue', alpha=0.7)
    ax1.set_title("standard geometry")
    ax1.set_xlabel("distance")
    ax1.set_ylabel("frequency")
    ax1.grid(True, alpha=0.3)

    ax2.hist(data_wrap, bins=50, color='orange', alpha=0.7)
    ax2.set_title("wraparound geometry")
    ax2.set_xlabel("distance")
    ax2.grid(True, alpha=0.3)

    plt.suptitle(title)
    plt.tight_layout()

    plt.savefig(output_filename, dpi=300)
    print(f"successfully saved graph to {output_filename}")
    plt.show()

def plot_nearest_overlaid(standard_file, wrap_file, output_filename):
    try:
        data_std = np.loadtxt(standard_file)
        data_wrap = np.loadtxt(wrap_file)

    except OSError:
        print(f"Error: Could not find files. Check if '{standard_file} exists.")
        return

    plt.figure(figsize=(10, 6))
    plt.hist(data_std, bins=100, color='blue', alpha=0.6, density=True, label='standard geometry')
    plt.hist(data_wrap, bins=100, color='orange', alpha=0.6, density=True, label="wraparound geometry")

    plt.title("distribution of distance to nearest neighbour")
    plt.xlabel("distance")
    plt.ylabel("probability density")
    plt.legend()
    plt.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(output_filename, dpi=300)
    print(f"saved graph to {output_filename}")
    plt.show()

def plot_nearest_observable(standard_file, wrap_file, output_filename):
    try:
        data_std = np.loadtxt(standard_file)
        data_wrap = np.loadtxt(wrap_file)
    except OSError:
        print(f"error: file not found.")
        return

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.hist(data_std, bins=500, density=True, histtype='step',
            color='blue', label='standard', linewidth=1.5)

    ax.hist(data_wrap, bins=500, density=True, histtype='step',
            color='orange', label='wraparound', linewidth=1.5)

    ax.set_title("nearest neighbour distribution (high resolution)")
    ax.set_xlabel("distance")
    ax.set_ylabel("probability density")
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.2)

    axins = inset_axes(ax, width="40%", height="40%", loc='center right')

    axins.hist(data_std, bins=500, density=True, histtype='step',
               color='blue', linewidth=2)
    axins.hist(data_wrap, bins=500, density=True, histtype='step',
               color='orange', linewidth=2)

    axins.set_xlim(8e-4, 1.5e-3)
    axins.set_ylim(600, 720)
    axins.xaxis.set_major_locator(MaxNLocator(nbins=4))
    plt.setp(axins.get_xticklabels(), rotation=0, fontsize=9)
    axins.grid(False)

    mark_inset(ax, axins, loc1=2, loc2=4, fc="none", ec="0.5")

    plt.savefig(output_filename, dpi=300)
    print(f"saved enhanced graph to {output_filename}")
    plt.show()


if __name__ == "__main__":

    plot_furthest_log(
        "data/furthest_standard_opt.txt",
        "data/furthest_wraparound_opt.txt",
        "plot_furthest_comparison.png"
    )
    plot_histogram(
        "data/furthest_standard_opt.txt",
        "data/furthest_wraparound_opt.txt",
        "distribution of dist to furthest neighbour",
        "plot_furthest.png"
    )

    plot_nearest_overlaid(
        "data/nearest_standard_opt.txt",
        "data/nearest_wraparound_opt.txt",
        "plot_nearest.png"
    )

    plot_nearest_observable(
        "data/nearest_standard_opt.txt",
        "data/nearest_wraparound_opt.txt",
        "plot_nearest_observable.png"
    )
