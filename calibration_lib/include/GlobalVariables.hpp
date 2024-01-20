#ifndef GLOBAL_VARIABLES_HPP
#define GLOBAL_VARIABLES_HPP

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

#include "OptionHandlers.hpp"
#include "Constants.hpp"

using namespace std;

extern useconds_t measurement_delay = MEASUREMENT_DELAY_US_DEFAULT; // delay between measurements in microseconds

extern unsigned int number_of_measurements = MEASUREMENTS_PER_CYCLE_DEFAULT; // Number of measurements per cycle

extern vector<float> default_robot_position = {200, -170, 120, 90, 90, 0}, // default robot starting position
    robot_position(default_robot_position);                                // Robot starting position

extern bool use_robot = false; // Flag to enable meca500 usage

extern string sensor_type, // sensor name to be used i.e. [infrared, ultrasonic]
    surface_name = "",     // surface name for saving measurements
    config_file_path = ""; // path to config file

extern float min_measurement = 0, // minimum distance to be measured
    max_measurement = 170,        // maximum distance to be measured
    step_size = 5,                // step interval between measurements
    current_measurement;          // current distance to be measured

extern DistanceSensor *sensor = nullptr; // Sensor to use for measuring
extern Robot *robot = nullptr;           // Robot object to handle meca500 communication

extern map<string, OptionHandler> optionHandlers;


#endif

