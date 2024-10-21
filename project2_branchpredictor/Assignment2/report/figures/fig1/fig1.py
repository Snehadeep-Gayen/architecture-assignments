import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Function to read the CSV file and plot lines
def plot_csv_data(file_name, plot_name, idx):
    # Read the CSV file into a pandas DataFrame
    data = pd.read_csv(file_name)
    
    # Plot each column in the DataFrame (excluding the 'size' column)
    plt.figure(figsize=(10, 6))
    
    plt.plot(data['m'], (data[data.columns[idx]]), marker='o', label=data.columns[idx])
    
    # Adding labels and title
    plt.xlabel('$m$', fontsize=18)
    plt.ylabel('Misprediction Rate (%)', fontsize=18)
    plt.title(f'Effect of $m$ on Miss Prediction Rate for {data.columns[idx]} trace', fontsize=20)
    plt.grid(True)


    # Custom xticks
    xtick_labels = data['m']  # Cache size in KB
    xtick_positions = xtick_labels  # Corresponding positions in log2 scale

    plt.xticks(ticks=xtick_positions, labels=xtick_labels.astype(int), fontsize=18)
    
    plt.yticks(fontsize=18)
    # Save the plot to a file
    plt.savefig(f'{plot_name}.png', dpi=300, bbox_inches='tight')
    
    # Show the plot
    plt.show()

# Call the function with the CSV file name
plot_csv_data('data.csv', "fig1a1", 1)
plot_csv_data('data.csv', "fig1a2", 2)

