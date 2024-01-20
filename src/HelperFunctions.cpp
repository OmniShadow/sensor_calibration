#include <stdio.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <string>
#include <map>

#include "csvlogger/CsvLogger.hpp"
#include "distance_sensor/include/DistanceSensor.hpp"
#include "distance_sensor/include/UltrasonicSensor.hpp"
#include "distance_sensor/include/InfraredSensor.hpp"
#include "meca500_ethercat_cpp/Robot.hpp"

#include "Constants.hpp"
#include "HelperFunctions.hpp"
#include "OptionHandlers.hpp"
#include "GlobalVariables.hpp"

using namespace std;

void make_measurements(DistanceSensor &sensor, int number_of_measurements, vector<float> &measurements, unsigned int delay_us)
{
    float distance;
    for (int i = 0; i < number_of_measurements; i++)
    {
        distance = sensor.getDistanceInMillimeters() - 12.04;
        measurements.push_back(distance);
        usleep(delay_us);
    }
}
void write_measurements_to_csv(vector<float> measurments, string file_path)
{
    CsvLogger measurements_logger(file_path);
    measurements_logger.write("distance\n");
    for (float measurement : measurments)
    {
        measurements_logger << measurement;
        measurements_logger.end_row();
    }

    measurements_logger.close();
}

void move_robot_to_position(vector<float> robot_position)
{
    robot->move_pose(
        robot_position[0],
        robot_position[1],
        robot_position[2],
        robot_position[3],
        robot_position[4],
        robot_position[5]);
}

void initialise_robot()
{
    use_robot = true;
    robot = new Robot(-30, 230, 5000, "eth0", 0.0, 10);
    robot->reset_error();
    robot->set_conf(1, 1, -1);
    // robot->print_pose();
    move_robot_to_position(robot_position);
}

void display_usage()
{
    cout << left << setw(optionWidth) << "Usage: ./calibrazione [OPTIONS]" << endl
         << "Options:" << endl;
    for (auto option : optionHandlers)
    {
        cout << option.second.helpMessage << endl;
    }
}

void setup_handlers()
{
    optionHandlers[HELP_COMMAND] = OptionHandler(handleHelp, helpMessage);
    optionHandlers[CONFIG_FROM_FILE_COMMAND] = OptionHandler(handleConfig, configMessage);
    optionHandlers[MEASURE_DELAY_US_COMMAND] = OptionHandler(handleDelay, delayMessage);
    optionHandlers[MEASUREMENTS_OPTIONS_COMMAND] = OptionHandler(handleMeasurementsOption, measurementOptionsMessage);
    optionHandlers[NUMBER_OF_MEASUREMENTS_COMMAND] = OptionHandler(handleNumberOfMeasurements, numberOfMeasurementsMessage);
    optionHandlers[ROBOT_STARTING_POSITION_COMMAND] = OptionHandler(handleRobotStartingPosition, robotStartingPositionMessage);
    optionHandlers[SENSOR_COMMAND] = OptionHandler(handleSensor, sensorMessage);
    optionHandlers[SURFACE_TYPE_COMMAND] = OptionHandler(handleSurface, surfaceMessage);
    optionHandlers[USE_ROBOT_COMMAND] = OptionHandler(handleRobot, robotMessage);
    optionHandlers[CALIBRATION_COMMAND] = OptionHandler(handleCalibration, calibrationMessage);
}

int setup_options(map<string, string> options)
{
    for (auto option : options)
    {
        string optionName = option.first;
        string value = option.second;
        if (optionHandlers.count(optionName))
        {
            cout << optionHandlers[optionName].handler(value) << endl;
        }
    }

    if (sensor == nullptr)
    {
        cerr << "Sensor type not set!!" << endl
             << "Please specify the sensor type with" << endl
             << "--" << SENSOR_COMMAND << "=[infrared, ultrasonic]" << endl
             << "Program will now exit..." << endl;
        return 1;
    }

    return 0;
}

map<string, string> parse_command_line(int argc, char *argv[])
{
    map<string, string> options;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg.substr(0, 2) == "--")
        {
            string command;
            // Found an option
            size_t pos = arg.find('=');
            if (pos != string::npos)
                command = arg.substr(2, pos - 2);
            else
                command = arg.substr(2);
            string value = (pos != string::npos) ? arg.substr(pos + 1) : "";

            options[command] = value;
        }
    }

    return options;
}
map<string, string> parse_config_file(string config_file_path)
{
    map<string, string> options;
    ifstream config_file(config_file_path);
    if (!config_file.is_open())
    {
        cerr << "Error opening config file!" << endl;
        cerr << "Program will now exit..." << endl;
        exit(1); // Return an error code
    }
    string config_row;

    while (getline(config_file, config_row))
    {
        size_t pos = config_row.find('=');
        string command;
        if (pos != string::npos)
            command = config_row.substr(0, pos);
        else
            command = config_row.substr(0);
        string value = (pos != string::npos) ? config_row.substr(pos + 1) : "";
        options[command] = value;
    }

    return options;
}

vector<float> parse_string_to_vector(string input)
{
    vector<float> result;

    // Check if the string starts with a curly brace
    if (input.empty() || input[0] != '{')
    {
        cerr << "Error: Input string does not start with a curly brace.\n";
        return result;
    }

    stringstream ss(input);

    // Remove curly braces from the input string
    char discard;
    ss >> discard; // Discard the opening curly brace

    // Read the values into the vector
    float value;
    while (ss >> value)
    {
        result.push_back(value);

        // Check for comma and discard if present
        if (ss.peek() == ',')
            ss.ignore();
    }
    return result;
}