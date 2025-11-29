import matplotlib.pyplot as plt
import numpy as np
from streamlit import title


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

if __name__ == "__main__":
    plot_histogram(
        "furthest_standard_opt.txt",
        "furthest_wraparound_opt.txt",
        "distribution of dist to furthest neighbour",
        "plot_furthest.png"
    )

    plot_nearest_overlaid(
        "nearest_standard_opt.txt",
        "nearest_wraparound_opt.txt",
        "plot_nearest.png"
    )
