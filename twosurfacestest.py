import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

def create_grid(values, threshold):
    # Convert the list of values to a NumPy array
    values_array = np.array(values)

    # Set the desired number of columns
    num_columns = 12

    # Calculate the number of rows needed
    num_rows = int(np.ceil(len(values) / num_columns))

    # Pad the array to make its length a multiple of the number of columns
    values_array = np.pad(values_array, (0, num_rows * num_columns - len(values)), mode='constant', constant_values=0)

    # Apply the threshold to create a binary array (0 if below threshold, 1 if above)
    binary_array = (values_array >= threshold).astype(int)

    # Reshape the 1D array into a grid with 1 row and 12 columns
    binary_grid = binary_array.reshape((1, num_columns))

    return binary_grid

def plot_grid(binary_grid, values_list):
    fig, ax = plt.subplots(1, 1, figsize=(len(binary_grid[0]), 1))  # Set figsize based on the number of columns

    # Define custom colormap for 0 and 1 values
    custom_cmap = mcolors.ListedColormap(['gray', 'black'])

    # Plot the binary grid as a black-and-white image
    cax = ax.imshow(binary_grid, cmap=custom_cmap, interpolation='nearest')

    # Display the colorbar
    cbar = fig.colorbar(cax)

    # Add text to each square with corresponding value
    for i in range(len(binary_grid)):
        for j in range(len(binary_grid[0])):
            text_color = 'black' if binary_grid[i, j] == 0 else 'white'
            ax.text(j, i, f'{binary_grid[i, j]}\n{values_list[i * len(binary_grid[0]) + j]:.2f}',
                    color=text_color, ha='center', va='center')

    #save and show
    plt.savefig("grid_plot.png")
    plt.show()

if __name__ == "__main__":
    # Example list of values
    values_list = [
        72.528, 75.72, 74.0687, 75.2485, 74.2824,
        75.0183, 74.3466, 74.8412, 74.328, 74.7372,
        74.3287, 74.672
    ]

    # Set the threshold
    threshold_value = 74.5

    # Create the binary grid with 1 row and 12 columns
    binary_grid = create_grid(values_list, threshold_value)

    # Plot the binary grid with grid lines and index labels
    plot_grid(binary_grid, values_list)
