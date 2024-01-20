#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

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

const stringstream helpMessage;
helpMessage << "  --" << setw(optionWidth) << HELP_COMMAND << setw(descriptionWidth) << "Display this help message" << endl;

const string configMessage;
configMessage << "  --" << CONFIG_FROM_FILE_COMMAND << setw(optionWidth - strlen(CONFIG_FROM_FILE_COMMAND)) << "=\"path/to/configfile.txt\""
              << "Parse options from text file" << endl;

const string delayMessage;
delayMessage << "  --" << MEASURE_DELAY_US_COMMAND << setw(optionWidth - strlen(MEASURE_DELAY_US_COMMAND)) << "=DELAY_VALUE_US";

const string measurementOptionsMessage;
measurementOptionsMessage << "  --" << MEASUREMENTS_OPTIONS_COMMAND << setw(optionWidth - strlen(MEASUREMENTS_OPTIONS_COMMAND)) << "=\"{min_measurement, max_measurement, step_size}\""
                          << "Specify the measurement options [default {0, 170, 10} ]" << endl;

const string numberOfMeasurementsMessage;
numberOfMeasurementsMessage << "  --" << NUMBER_OF_MEASUREMENTS_COMMAND << setw(optionWidth - strlen(NUMBER_OF_MEASUREMENTS_COMMAND)) << "=COUNT"
                            << "Specify the number of measurements to take" << endl;

const string robotStartingPositionMessage;
robotStartingPositionMessage << "Specify the starting pose of the meca500 [default {200, -170, 120, 90, 90, 0} ]" << endl
                             << endl;

const string sensorMessage;
sensorMessage
    << "  --" << SENSOR_COMMAND << setw(optionWidth - strlen(SENSOR_COMMAND)) << "=TYPE"
    << "Specify sensor type (e.g., infrared, ultrasonic)" << endl;

const string surfaceMessage;
surfaceMessage << "  --" << SURFACE_TYPE_COMMAND << setw(optionWidth - strlen(SURFACE_TYPE_COMMAND)) << "=TYPE"
               << "Specify surface type for measurements" << endl;

const string robotMessage;
robotMessage << "  --" << setw(optionWidth) << USE_ROBOT_COMMAND << setw(descriptionWidth) << "Use robot for measurements" << endl;

const string calibrationMessage;
calibrationMessage << "Specify the calibration parameters of the sensor [default {1, 0} ]" << endl;

#endif