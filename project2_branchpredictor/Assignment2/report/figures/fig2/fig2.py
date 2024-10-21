import matplotlib.pyplot as plt
import numpy as np

# Function to read misprediction data from file
def read_data(filename):
    data = {}
    with open(filename, 'r') as f:
        for line in f:
            m, n, misprediction = line.strip().split()
            m = int(m)
            n = int(n)
            misprediction = float(misprediction)
            if n not in data:
                data[n] = []
            data[n].append((m, misprediction))
    return data

# Function to plot the data
def plot_data(data, benchmark_name, output_file):
    plt.figure(figsize=(10, 6))
    
    for n, values in data.items():
        values.sort(key=lambda x: x[0])  # Sort by m
        m_values = [x[0] for x in values]
        mispredictions = [x[1] for x in values]
        plt.plot(m_values, mispredictions, marker='o', label=f"n={n}")
    
    plt.title(f'Misprediction Rate vs m for {benchmark_name}', fontsize=20)
    plt.xlabel('m (Global History Register Size)', fontsize=18)
    plt.ylabel('Misprediction Rate (%)', fontsize=18)
    plt.xticks(fontsize=18)
    plt.yticks(fontsize=18)
    plt.legend(fontsize=18)
    plt.grid(True)
    # Tighten the layout
    plt.tight_layout()
    plt.savefig(output_file)
    plt.show()

# Read and plot the data for gcc_trace
gcc_data = read_data('gcc_misprediction.txt')
plot_data(gcc_data, 'gcc_trace', 'gcc_misprediction_plot.png')

# Read and plot the data for jpeg_trace
jpeg_data = read_data('jpeg_misprediction.txt')
plot_data(jpeg_data, 'jpeg_trace', 'jpeg_misprediction_plot.png')
