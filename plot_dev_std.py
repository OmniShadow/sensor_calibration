import matplotlib.pyplot as plt
import pandas as pd

# Write path to your file
surface = "cartoncinonerocalibratoCopia"
csv_file_path = 'measurements/infrared/-CARTONCINI/'+ surface + '/stats/stats.csv'

# Read the CSV file into a pandas DataFrame
df = pd.read_csv(csv_file_path)

# Extract the columns for plotting
x_values = df['valore']
y_values = df['devstd']

# Plot the data
plt.plot(x_values, y_values, marker='o', linestyle='-')

# Add labels and title
plt.xlabel('Valore atteso[mm]')
plt.ylabel('Dev_std')

# Show the plot
plt.savefig('dev_std_' + surface + '.png')
plt.show()
