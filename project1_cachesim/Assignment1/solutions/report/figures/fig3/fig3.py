import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Function to read the CSV file and plot lines
def plot_csv_data(file_name):
    # Read the CSV file into a pandas DataFrame
    data = pd.read_csv(file_name)
    
    # Plot each column in the DataFrame (excluding the 'size' column)
    plt.figure(figsize=(10, 6))
    
    for column in data.columns[1:]:
        plt.plot(np.log2(data['size']/1024), (data[column]), marker='o', label=column)
    
    # Adding labels and title
    plt.xlabel('Cache Size (KB)', fontsize=12)
    plt.ylabel('Average Access Time (ns)', fontsize=12)
    plt.title('Effect of Associativity on Average Access Time with L2 Cache', fontsize=14)
    plt.grid(True)
    plt.legend(title='Associativity', fontsize=10)


    # Custom xticks
    xtick_labels = data['size'] / 1024  # Cache size in KB
    xtick_positions = np.log2(xtick_labels)  # Corresponding positions in log2 scale

    plt.xticks(ticks=xtick_positions, labels=xtick_labels.astype(int), fontsize=10)
    
    
    # Save the plot to a file
    plt.savefig('fig3.png', dpi=300, bbox_inches='tight')
    
    # Show the plot
    plt.show()

# Call the function with the CSV file name
plot_csv_data('data.csv', dpi=300)
