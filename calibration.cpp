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
#include "distance_sensor/include/DistanceSensor.hpp"
#include "distance_sensor/include/UltrasonicSensor.hpp"
#include "distance_sensor/include/InfraredSensor.hpp"
#include "meca500_ethercat_cpp/Robot.hpp"

#define HELP_COMMAND "help"
#define CONFIG_FROM_FILE_COMMAND "config"
#define SENSOR_COMMAND "sensor" // REQUIRED sensor specifier command [--sensor]
#define CALIBRATION_COMMAND "calibration"
#define INFRARED_SENSOR_VALUE "infrared"              // infrared sensor specifier [--sensor=infrared]
#define ULTRASONIC_SENSOR_VALUE "ultrasonic"          // ultrasonic sensor specifier [--sensor=ultrasonic]
#define SURFACE_TYPE_COMMAND "surface"                // surface specifier [--surface="surface_name"]
#define NUMBER_OF_MEASUREMENTS_COMMAND "measurements" // number of measurements per cycle [--measurements=number_of_measurements]
#define USE_ROBOT_COMMAND "userobot"                  // command flag to use meca500 robot [--userobot]
#define MEASURE_DELAY_US_COMMAND "delay"              // delay between measurements in micro seconds [--delay=microseconds]
#define ROBOT_STARTING_POSITION_COMMAND "position"    // starting pose of the meca500
#define MEASUREMENTS_OPTIONS_COMMAND "options"        // command to set the measurement options [min_measurement,max_measurement,step_size]

/* DEFAULT VALUES */
#define MEASUREMENTS_PER_CYCLE_DEFAULT 100   // default number of measurements per cycle
#define ECHO_PIN 23                          // default ECHO GPIO PIN for the ultrasonic sensor
#define TRIG_PIN 22                          // default TRIG GPIO PIN for the ultrasonic sensor
#define MEASUREMENT_DELAY_US_DEFAULT 0.02e+6 // default delay between measurements in micro seconds

#define optionWidth 60
#define descriptionWidth 60
#define message_length 30

using namespace std;

stringstream helpMessage;

stringstream configMessage;

stringstream delayMessage;

stringstream measurementOptionsMessage;

stringstream numberOfMeasurementsMessage;

stringstream robotStartingPositionMessage;

stringstream sensorMessage;

stringstream surfaceMessage;

stringstream robotMessage;

stringstream calibrationMessage;

struct OptionHandler
{
    using Handler = string (*)(string);

    OptionHandler() : handler(nullptr), helpMessage("") {}
    OptionHandler(Handler func, const string &helpMsg)
        : handler(func), helpMessage(helpMsg) {}

    Handler handler;
    string helpMessage;
};

/*** HELPER FUNCTIONS ***/
void display_usage();
void setup_help_messages();
void setup_handlers();
int setup_options(map<string, string> options);
map<string, string> parse_command_line(int argc, char *argv[]);
map<string, string> parse_config_file(string config_file_path);
vector<float> parse_string_to_vector(string input);
void move_robot_to_position(vector<float> robot_position);
void initialise_robot();
void make_measurements(DistanceSensor &sensor, int number_of_measurements, vector<float> &measurements, unsigned int delay_us); // function to measure the distance with the given sensor
void write_measurements_to_csv(vector<float> measurments, string file_path);
/************************************************/

/*** COMMAND HANDLERS ***/
string handleHelp(string value);
string handleMeasurementsOption(string value);
string handleRobotStartingPosition(string value);
string handleSensor(string value);
string handleNumberOfMeasurements(string value);
string handleRobot(string value);
string handleSurface(string value);
string handleDelay(string value);
string handleConfig(string value);
string handleNotFound(string value);
string handleCalibration(string value);
/************************************************/

/*** GLOBAL VARIABLES ***/
useconds_t measurement_delay = MEASUREMENT_DELAY_US_DEFAULT; // delay between measurements in microseconds

unsigned int number_of_measurements = MEASUREMENTS_PER_CYCLE_DEFAULT; // Number of measurements per cycle

vector<float> default_robot_position = {200, -170, 120, 90, 90, 0}, // default robot starting position
    robot_position(default_robot_position);                         // Robot starting position

bool use_robot = false; // Flag to enable meca500 usage

string sensor_type,        // sensor name to be used i.e. [infrared, ultrasonic]
    surface_name = "",     // surface name for saving measurements
    config_file_path = ""; // path to config file

float min_measurement = 0, // minimum distance to be measured
    max_measurement = 170, // maximum distance to be measured
    step_size = 5,         // step interval between measurements
    current_measurement;   // current distance to be measured

DistanceSensor *sensor = nullptr; // Sensor to use for measuring
Robot *robot = nullptr;           // Robot object to handle meca500 communication
/************************************************/

map<string, OptionHandler> optionHandlers;

int main(int argc, char *argv[])
{
    /*INITIAL SETUP*/
    setup_help_messages();
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
            getchar();
        }

        cout << "Measuring distance..." << endl;
        make_measurements(*sensor, number_of_measurements, measurements, measurement_delay);
        cout << "Writing measurements to csv file\n\n";
        write_measurements_to_csv(measurements, file_path + csv_file_name);
        current_measurement += step_size;
    }
    return 0;
}

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
        cout << option.second.helpMessage;
    }
}

void setup_help_messages()
{
    helpMessage
        << left
        << "  --" << setw(optionWidth) << HELP_COMMAND << setw(descriptionWidth) << "Display this help message" << endl;

    configMessage
        << left
        << "  --" << CONFIG_FROM_FILE_COMMAND << setw(optionWidth - strlen(CONFIG_FROM_FILE_COMMAND))
        << "=\"path/to/configfile.txt\""
        << "Parse options from text file" << endl;

    delayMessage
        << left
        << "  --" << MEASURE_DELAY_US_COMMAND << setw(optionWidth - strlen(MEASURE_DELAY_US_COMMAND))
        << "=DELAY_VALUE_US";

    measurementOptionsMessage
        << left
        << "  --" << MEASUREMENTS_OPTIONS_COMMAND << setw(optionWidth - strlen(MEASUREMENTS_OPTIONS_COMMAND))
        << "=\"{min_measurement, max_measurement, step_size}\""
        << "Specify the measurement options [default {0, 170, 10} ]" << endl;

    numberOfMeasurementsMessage
        << left
        << "  --" << NUMBER_OF_MEASUREMENTS_COMMAND << setw(optionWidth - strlen(NUMBER_OF_MEASUREMENTS_COMMAND))
        << "=COUNT"
        << "Specify the number of measurements to take" << endl;

    robotStartingPositionMessage
        << left
        << "  --" << ROBOT_STARTING_POSITION_COMMAND << setw(optionWidth - strlen(ROBOT_STARTING_POSITION_COMMAND))
        << "=\"{x,y,z,alpha,beta,gamma}\""
        << "Specify the starting pose of the meca500 [default {200, -170, 120, 90, 90, 0} ]" << endl
        << endl;

    sensorMessage
        << left
        << "  --" << SENSOR_COMMAND << setw(optionWidth - strlen(SENSOR_COMMAND))
        << "=TYPE"
        << "Specify sensor type (e.g., infrared, ultrasonic)" << endl;

    surfaceMessage
        << left
        << "  --" << SURFACE_TYPE_COMMAND << setw(optionWidth - strlen(SURFACE_TYPE_COMMAND))
        << "=TYPE"
        << "Specify surface type for measurements" << endl;

    robotMessage
        << left
        << "  --" << setw(optionWidth) << USE_ROBOT_COMMAND << setw(descriptionWidth) << "Use robot for measurements" << endl;

    calibrationMessage
        << left
        << "  --" << CALIBRATION_COMMAND << setw(optionWidth - strlen(CALIBRATION_COMMAND))
        << "=\"{m, q}\""
        << "Specify the calibration parameters of the sensor [default {1, 0} ]" << endl
        << endl;
}

void setup_handlers()
{
    optionHandlers[HELP_COMMAND] = OptionHandler(handleHelp, helpMessage.str());
    optionHandlers[CONFIG_FROM_FILE_COMMAND] = OptionHandler(handleConfig, configMessage.str());
    optionHandlers[MEASURE_DELAY_US_COMMAND] = OptionHandler(handleDelay, delayMessage.str());
    optionHandlers[MEASUREMENTS_OPTIONS_COMMAND] = OptionHandler(handleMeasurementsOption, measurementOptionsMessage.str());
    optionHandlers[NUMBER_OF_MEASUREMENTS_COMMAND] = OptionHandler(handleNumberOfMeasurements, numberOfMeasurementsMessage.str());
    optionHandlers[ROBOT_STARTING_POSITION_COMMAND] = OptionHandler(handleRobotStartingPosition, robotStartingPositionMessage.str());
    optionHandlers[SENSOR_COMMAND] = OptionHandler(handleSensor, sensorMessage.str());
    optionHandlers[SURFACE_TYPE_COMMAND] = OptionHandler(handleSurface, surfaceMessage.str());
    optionHandlers[USE_ROBOT_COMMAND] = OptionHandler(handleRobot, robotMessage.str());
    optionHandlers[CALIBRATION_COMMAND] = OptionHandler(handleCalibration, calibrationMessage.str());
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
        else
        {
            handleNotFound(optionName);
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

string handleHelp(string value)
{
    display_usage();

    return "";
}

string handleMeasurementsOption(string value)
{
    stringstream option_message;
    vector<float> measurement_options = parse_string_to_vector(value);
    if (measurement_options.size() < 3)
    {
        cerr << "Invalid measurement options, not enough arguments" << endl;
        cerr << "Program will now exit..." << endl;
        exit(1);
    }
    if (measurement_options[0] >= measurement_options[1])
    {
        cerr << "Invalid measurement options, min_measurement can't be higher or equal to max_measurement" << endl;
        cerr << "Program will now exit..." << endl;
        exit(1);
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
        exit(1);
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
        exit(1);
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
        exit(1);
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
        exit(1);
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
    option_message << left << setw(message_length) << "Unknown command: " << value << "\n";
    return option_message.str();
}
