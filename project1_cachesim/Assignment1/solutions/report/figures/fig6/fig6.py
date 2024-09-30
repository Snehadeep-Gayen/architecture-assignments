import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Load the data from the CSV
data = pd.read_csv('./data.csv')

# Extract the x, y, z values from the data
x = np.log2(data['l1 cache size'])  # log2(L1 cache size)
y = np.log2(data['l2 cache size'])  # log2(L2 cache size)
z = data['aat']/1e6  # AAT

# Create a 3D plot
fig = plt.figure(figsize=(10, 7))  # Adjust the figure size for a better fit
ax = fig.add_subplot(111, projection='3d')

# Plot the surface
ax.plot_trisurf(x, y, z, cmap='viridis', edgecolor='none')

# Set the view angle (elevation and azimuth)
ax.view_init(elev=10, azim=210)  # Adjust the values for different angles

# Set custom ticks for L1 and L2 cache sizes (actual cache sizes in KB and MB)
l1_ticks = ([12, 13, 14, 15, 16])  # L1 cache sizes from 4KB to 64KB
l1_labels = ['4', '8', '16', '32', '64']

l2_ticks = [15, 16, 17, 18, 19, 20]  # L2 cache sizes from 32KB to 1MB
l2_labels = ['32', '64', '128', '256', '512', '1024']

ax.set_xticks(l1_ticks)
ax.set_xticklabels(l1_labels)

ax.set_yticks(l2_ticks)
ax.set_yticklabels(l2_labels)

# Label the axes
ax.set_xlabel('L1 Cache Size (KB)')
ax.set_ylabel('L2 Cache Size (KB)')
ax.set_zlabel('Energy-Delay Product (mJ)')

# Show the plot
# plt.show()

plt.title('Varitation of energy-delay product with L1 & L2 cache sizes', fontsize=14)

# Save the figure
plt.savefig('./fig6.png')
