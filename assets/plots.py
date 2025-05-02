import matplotlib.pyplot as plt
import numpy as np

# Data from your results (extended with more points for 4 and 8 producers)
producers = [1, 4, 8]  # Different number of producers
consumers_1 = list(range(1, 21))  # Number of consumers for each producer count (1 to 20)
avg_waiting_times_1 = [225, 107, 87, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83]  # Corresponding avg waiting times for 1 producer
avg_waiting_times_4 = [767, 681, 590, 394, 348, 314, 298, 247, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237, 237]  # Corresponding avg waiting times for 4 producers
avg_waiting_times_8 = [931, 885, 881, 869, 828, 766, 744, 642, 559, 544, 530, 518, 510, 502, 497, 493, 490, 487, 484, 465]  # Corresponding avg waiting times for 8 producers

# Create the plot
fig, ax = plt.subplots(figsize=(10, 6))

# Plot each producer count's results
ax.plot(consumers_1, avg_waiting_times_1, label='1 Producer', marker='o', color='b')
ax.plot(consumers_1, avg_waiting_times_4, label='4 Producers', marker='s', color='g')
ax.plot(consumers_1, avg_waiting_times_8, label='8 Producers', marker='^', color='r')

# Labeling the axes
ax.set_xlabel('Number of Consumers')
ax.set_ylabel('Average Waiting Time (μs)')
ax.set_title('Producer-Consumer: Average Waiting Time vs Number of Consumers')

# Show grid and legend
ax.grid(True)
ax.legend()

# Show the plot
plt.show()
