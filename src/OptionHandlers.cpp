#include "OptionHandlers.hpp"
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
#include "distance_sensor/include/DistanceSensor.hpp"
#include "distance_sensor/include/UltrasonicSensor.hpp"
#include "distance_sensor/include/InfraredSensor.hpp"
#include "meca500_ethercat_cpp/Robot.hpp"

#include "GlobalVariables.hpp"
#include "HelperFunctions.hpp"

using namespace std;


string handleHelp()
{
    display_usage();
    return 1;
    return option_message.str();
}

string handleMeasurementsOption(string value)
{
    stringstream option_message;
    vector<float> measurement_options = parse_string_to_vector(value);
    if (measurement_options.size() < 3)
    {
        cerr << "Invalid measurement options, not enough arguments" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    if (measurement_options[0] >= measurement_options[1])
    {
        cerr << "Invalid measurement options, min_measurement can't be higher or equal to max_measurement" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    min_measurement = measurement_options[0];
    max_measurement = measurement_options[1];
    step_size = measurement_options[2];
    current_measurement = step_size > 0 ? min_measurement : max_measurement;
    option_message << left << "Measurement options:\n"
                   << setw(message_length) << "Minimum measurement:" << min_measurement << endl
                   << setw(message_length) << "Maximum measurement:" << max_measurement << endl
                   << setw(message_length) << "Step size:" << step_size << endl;
    return option_message.str();
}

string handleRobotStartingPosition(string value)
{
    stringstream option_message;
    vector<float> robot_position_values = parse_string_to_vector(value);
    if (robot_position_values.size() < 6)
    {
        cerr << "Invalid robot position, not enough arguments" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    option_message << left << setw(message_length) << "Starting robot position: {";
    for (float pos : robot_position_values)
        option_message << pos << ", ";
    option_message << "}" << endl;
    robot_position = robot_position_values;
    return option_message.str();
}

string handleSensor(string value)
{
    stringstream option_message;
    option_message << left << setw(message_length) << "Sensor used: " << value << "\n";
    if (value == INFRARED_SENSOR_VALUE)
    {
        sensor = new InfraredSensor(InfraredSensor::USER_INPUT);
        // sensor->useCalibrationCurve(1.0,13.01);
    }

    else if (value == ULTRASONIC_SENSOR_VALUE)
        sensor = new UltrasonicSensor(TRIG_PIN, ECHO_PIN);
    else
    {
        cerr << "Invalid sensor type. Supported types: ultrasonic, infrared" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    sensor_type = value;
    return option_message.str();
}

string handleNumberOfMeasurements(string value)
{
    stringstream option_message;
    int int_value = stoi(value);
    if (int_value < 0)
    {
        cerr << "Invalid number of measurements value" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    option_message << left << setw(message_length) << "Number of measurements per cycle: " << value << "\n";
    number_of_measurements = int_value;
    return option_message.str();
}

string handleRobot(string value)
{
    stringstream option_message;
    option_message << "Using Meca500 robot\n"
                   << "Initialising robot EtherCAT interface\n";
    initialise_robot();
    return option_message.str();
}

string handleSurface(string value)
{
    stringstream option_message;
    option_message << left << setw(message_length) << "Surface used: " << value << "\n";
    surface_name = value;
    return option_message.str();
}

string handleDelay(string value)
{
    stringstream option_message;
    int int_value = stoi(value);
    if (int_value < 0)
    {
        cerr << "Invalid number of measures value" << endl;
        cerr << "Program will now exit..." << endl;
        return 1;
    }
    option_message << left << setw(message_length) << "Measurement delay in us used: " << value << "\n";
    measurement_delay = int_value;
    return option_message.str();
}

string handleConfig(string value)
{
    stringstream option_message;
    // value corresponds to the path of the config file given
    setup_options(parse_config_file(value));
    return option_message.str();
}

string handleCalibration(string value)
{
    stringstream option_message;
    vector<float> calibration_values = parse_string_to_vector(value);

    if (sensor != nullptr)
    {
        sensor->useCalibrationCurve(calibration_values[0], calibration_values[1]);
    }

    return option_message.str();
}

string handleNotFound(string value)
{
    stringstream option_message;
    option_message << left << setw(message_length) << "Unknown option: " << command << "\n";
}
