# PURPOSE: this script analyse data-sensor

import os  # interation with operating system
import sys  # interation with operating system
import pandas as pd  # draw graph
import matplotlib.pyplot as plt  # draw graph
import re  # support for working with regular expression (string)
import numpy as np  # support for working with multidimension variables (array)
from scipy.stats import norm  # probability and statistic
from sklearn.linear_model import LinearRegression  # for best fit

marker_size = 2


def is_analysed(path):
    return os.path.exists(path+"/.analysed")


def create_folder(folder_path):
    # Check if the folder exists
    if not os.path.exists(folder_path):
        # If not, create the folder
        os.makedirs(folder_path)
        print(f"Folder '{folder_path}' created successfully.")
    else:
        print(f"Folder '{folder_path}' already exists.")


def remove_trailing_commas(file_path):
    # Open the input file in read mode
    with open(file_path, "r") as input_file:
        # Read lines from the input file
        lines = input_file.readlines()

    # Open the output file in write mode
    with open(file_path, "w") as output_file:
        # Iterate through each line and remove the last comma if it exists
        for line in lines:
            # Remove trailing whitespace and check if the last character is a comma
            if line.rstrip().endswith(","):
                # Remove the last comma and write the updated line to the output file
                # Remove last character (comma) and add newline
                updated_line = line.rstrip()[:-1] + "\n"
                output_file.write(updated_line)
            else:
                # If there is no trailing comma, write the line as it is to the output file
                output_file.write(line)


def extract_number_from_string(input_string):
    # Define a regular expression pattern to match numbers
    pattern = r'\d+'

    # Use re.findall to extract all numbers from the input string
    numbers = re.findall(pattern, input_string)

    # If there are numbers, convert the first one to an integer and return it
    if numbers:
        return int(numbers[0])
    else:
        return None  # Return None if no numbers are found


def get_file_names(folder_path):
    # Check if the provided path is a directory
    if not os.path.isdir(folder_path):
        print(f"{folder_path} is not a valid directory.")
        return []

    # Get all files in the directory
    files = os.listdir(folder_path)

    # Filter out directories, keep only files
    file_names = [file for file in files if os.path.isfile(
        os.path.join(folder_path, file))]

    return file_names


def analyse_files(folder_path):

    contents = os.listdir(folder_path)

    # Filter out only directories
    surface_folders = [item for item in contents if os.path.isdir(os.path.join(folder_path, item))]

    # array to fill with datas of best fit
    surface_slopes = []
    surface_intercept = []

    for surface_name in surface_folders:

        sub_folder_path = folder_path + "/" + surface_name
        if (is_analysed(sub_folder_path)):
           continue

        # initializing stats file
        create_folder(sub_folder_path + "/stats")
        create_folder(sub_folder_path + "/plots")

        stats = open(sub_folder_path+"/stats/stats.csv", "w")

        stats.write("valore,media,devstd,error\n")

        for measurements_csv_file_name in get_file_names(sub_folder_path):

            #if(measurements_csv_file_name == ".analysed"):
            #    continue

            measurements_csv_file_path = sub_folder_path + "/" + measurements_csv_file_name

            # STEP 1: REMOVE TRAILING COMMAS
            remove_trailing_commas(file_path=measurements_csv_file_path)

            # Step 2: Read the CSV File
            print("Analysing: " + measurements_csv_file_path)
            measurements_data_frame = pd.read_csv(measurements_csv_file_path)

            # Step 3: Create Plots
            plt.plot(
                range(len(measurements_data_frame["distance"])),
                measurements_data_frame["distance"].astype(float),
                marker="o",
                linestyle="-",
                linewidth=0.5,
                markersize=marker_size,
                color="b",
            )
            plt.title(measurements_csv_file_name)
            plt.xlabel("Indice misura")
            plt.ylabel("Distanza misurata in mm")
            plt.ylim(0, 255)
            plt.tight_layout()
            plt.grid(True)

            distance_stats = measurements_data_frame["distance"].describe()
            min_value = distance_stats["min"]
            max_value = distance_stats["max"]
            mean_distance = distance_stats["mean"]
            std_dev_distance = distance_stats["std"]
            tag_text = f'Mean: {mean_distance:.2f}\nStd Dev: {std_dev_distance:.2f}\nMin: {min_value:.2f}\nMax: {max_value:.2f}'

            plt.annotate(tag_text,
                         xy=(1, 1), xycoords='axes fraction',
                         xytext=(-10, -10), textcoords='offset points',
                         ha='right', va='top',
                         bbox=dict(boxstyle='round,pad=0.3',
                                   edgecolor='black', facecolor='white'),
                         fontsize=10)

            plt.savefig(sub_folder_path + "/plots/plot_" +
                        measurements_csv_file_name + ".png")

            misura = extract_number_from_string(measurements_csv_file_name)

            stats.write(str(misura) + "," +
                        str(mean_distance) + "," + str(std_dev_distance) + "," + str(mean_distance-misura) + '\n')
            plt.close()

            x_values = np.linspace(0, 255, 5000)

            # Calculate the corresponding y values for the Gaussian distribution
            desired_tollerance = 1
            #y_values = norm.pdf(x_values, misura, desired_tollerance/3)
            y_values_m = norm.pdf(x_values, mean_distance, std_dev_distance)

            plt.hist(measurements_data_frame["distance"].astype(float), bins=20, color="skyblue",
                     edgecolor="black", density=True, stacked=True)
            #plt.plot(x_values, y_values, 'r-', label='Desired distribution')
            plt.plot(x_values, y_values_m, 'b-', label='Measured distribution')
            plt.legend()

            plt.xlim(min_value, max(misura, max_value) + desired_tollerance)

            # Add labels and title
            plt.xlabel("Valore misura mm")
            plt.ylabel("Probabilità")
            plt.title("Distribuzione dei valori")
            plt.tight_layout()

            plt.savefig(sub_folder_path + "/plots/histogram_" +
                        measurements_csv_file_name + ".png")
            plt.close()
            # plt.show()

        stats.close()
        stats_df = pd.read_csv(sub_folder_path + "/stats/stats.csv")
        errors = stats_df['error']

        # best fit
        model = LinearRegression()
        model.fit(X=stats_df["valore"].values.reshape(-1, 1),
                  y=stats_df["media"].values,)
        slope = model.coef_[0]
        intercept = model.intercept_
        predicted_values = model.predict(
            stats_df["valore"].values.reshape(-1, 1))

        surface_slopes.append(slope)
        surface_intercept.append(intercept)

        plt.figure(figsize=(10, 6))  # Optional: Set the figure size
        equation = f'Y = {slope:.2f}X + {intercept:.2f}'
        plt.text(0.8, 0.9, equation, ha='center', va='center', transform=plt.gca(
        ).transAxes, fontsize=10, bbox=dict(facecolor='white', alpha=0.5))

        plt.plot(
            stats_df["valore"],
            predicted_values,
            color='brown',
            label='Linear Regression Curve',
            linestyle="-",
            linewidth=2,
        )
        plt.plot(
            stats_df["valore"].astype(float),
            stats_df["valore"].astype(float),
            linestyle="-",
            linewidth=2,
            markersize=marker_size,
            color="black",
            label="valore misurato mm",
        )
        plt.plot(
            stats_df["valore"].astype(float),
            stats_df["media"].astype(float),
            marker="o",
            linestyle="",
            linewidth=0.5,
            markersize=marker_size,
            color="b",
            label="media misure mm",
        )
        plt.plot(
            stats_df["valore"].astype(float),
            stats_df["devstd"].astype(float),
            marker="o",
            linestyle="",
            linewidth=0.5,
            markersize=marker_size,
            color="r",
            label="deviazione standard mm",
        )
        plt.plot(
            stats_df["valore"].astype(float),
            errors,
            marker="o",
            linestyle="",
            linewidth=0.5,
            markersize=marker_size,
            color="green",
            label="errore tra media e riferimento  mm",
        )
        plt.xlabel("misura aspettata mm")
        plt.ylabel("media misurata mm")
        plt.legend()
        plt.grid(True)  # Optional: Add grid lines

        # Step 5: Show or Save the Plot (Optional)
        plt.savefig(sub_folder_path+"/plots/stats_plot.png")
        # plt.show()
        plt.close()


        # Step 6: creation of dev_std_media.txt and dev_std_max.txt

        # initializing stats
        stats_df = pd.read_csv(sub_folder_path + "/stats/stats.csv")

        #open file and write mean of devstd
        with open(sub_folder_path + '/stats/dev_std_media.txt', 'w') as file:
            # Write content to the file
            file.write(str(stats_df["devstd"].describe()["mean"]))
            #print("scritto: " + surface_name)
        
        # creation of dev_std_max.txt
        with open(sub_folder_path + '/stats/dev_std_max.txt', 'w') as file:
            # Write content to the file
            file.write(str(stats_df["devstd"].describe()["max"]))
            #print("scritto: " + surface_name)
        
        # Step 7: add a flag of 'already analysed'
        flag = open(sub_folder_path+"/.analysed", "w")
        flag.close()


def main():
    # Check if a folder path is provided as a command-line argument
    if len(sys.argv) != 2:
        print("Usage: python script.py /path/to/your/folder")
        sys.exit(1)
    # Get the folder path from the command-line argument
    folder_path = sys.argv[1]
    analyse_files(folder_path)


if __name__ == "__main__":
    main()
