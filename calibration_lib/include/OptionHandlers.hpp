#ifndef OPTION_HANDLERS_HPP
#define OPTION_HANDLERS_HPP

#include <stdio.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <string>
#include <map>
#include "distance_sensor/include/DistanceSensor.hpp"
#include "distance_sensor/include/UltrasonicSensor.hpp"
#include "distance_sensor/include/InfraredSensor.hpp"
#include "meca500_ethercat_cpp/Robot.hpp"
#include "Constants.hpp"

using namespace std;



struct OptionHandler
{
    using Handler = string (*)(string);

    OptionHandler(Handler func, const string &helpMsg)
        : handler(func), helpMessage(helpMsg) {}

    Handler handler;
    string helpMessage;
};

void display_usage();
string handleHelp();
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

#endif