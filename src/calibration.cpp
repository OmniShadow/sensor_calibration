/*LIBRARIES*/
#include <stdio.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include <cstring>
#include <string>
#include <map>

#include "csvlogger/CsvLogger.hpp"
#include "include/distance_sensor/include/DistanceSensor.hpp"
#include "include/distance_sensor/include/UltrasonicSensor.hpp"
#include "include/distance_sensor/include/InfraredSensor.hpp"
#include "include/meca500_ethercat_cpp/Robot.hpp"

#include "Constants.hpp"
#include "HelperFunctions.hpp"
#include "OptionHandlers.hpp"
#include "GlobalVariables.hpp"

using namespace std;


int main(int argc, char *argv[])
{
    /*INITIAL SETUP*/

    setup_handlers();

    if (setup_options(parse_command_line(argc, argv)) != 0)
        return 1; // error

    vector<float> measurements;                                                  // vector storing all the measurements
    string file_path = "measurements/" + sensor_type + "/" + surface_name + "/"; // output file path
    string csv_file_name;                                                        // name of output file

    /*MEASUREMENT*/
    cout << "Setup complete\nStarting measurements\n\n";

    if (use_robot)
    {
        cout << "Please position the obstacle in front of the sensor" << endl
             << "Press enter to continue..." << endl;
        getchar();
    }

    getchar(); // hack to fix infrared sensor user input (endl in buffer)

    while (current_measurement <= max_measurement && current_measurement >= min_measurement)
    {
        // setup for next set of measurements
        measurements.clear();
        csv_file_name = "";
        csv_file_name += (current_measurement < 100 ? "0" : "");
        csv_file_name += (current_measurement < 10 ? "0" : "");
        csv_file_name += to_string((int)current_measurement) + "mm.csv";

        cout << "Currently measuring: " << current_measurement << " mm\n";
        if (use_robot)
        {
            cout << "Moving robot to position..." << endl;

            if (current_measurement != min_measurement)
                // if not first movement move robot by step size
                robot_position[0] -= step_size;
            else
                // if first measurement move robot to starting position
                robot_position[0] -= step_size > 0 ? min_measurement : max_measurement;

            move_robot_to_position(robot_position);
        }
        else
        {
            cout << "Please position the obstacle in front of the sensor" << endl
                 << "Press enter to continue..." << endl;
            c = getchar();
        }

        cout << "Measuring distance..." << endl;
        make_measurements(*sensor, number_of_measurements, measurements, measurement_delay);
        cout << "Writing measurements to csv file\n\n";
        write_measurements_to_csv(measurements, file_path + csv_file_name);
        current_measurement += step_size;
    }
    return 0;
}


