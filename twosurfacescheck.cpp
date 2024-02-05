/*
MECA moves sensor, measuring the same distance between two different surfaces: 
- alluminium
- black flimsy paper

the target is to check the dipendence from the surface
*/

#include "distance_sensor/include/DistanceSensor.hpp"
#include "distance_sensor/include/UltrasonicSensor.hpp"
#include "distance_sensor/include/InfraredSensor.hpp"
#include "meca500_ethercat_cpp/Robot.hpp"
#include <stdio.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include <cstring>
#include <string>

using namespace std;

void make_measurements(DistanceSensor &sensor, int number_of_measurements, vector<float> &measurements, unsigned int delay_us);


int main(int argc, char *argv[])
{
    /*setup meca*/
    InfraredSensor sensor(InfraredSensor::ENABLE_ALL);
    Robot robot(100, 200, 5000, "eth0", 0.0, 10);
    robot.reset_error();
    // robot.main();
    robot.set_conf(1, 1, -1);
    robot.move_pose(130, -180, 230, 90, 90, 0);

    robot.print_pose();
    float velocity[6] = {0, 0, 0, 0, 0, 0};
    float position[6];

    /*start process*/
    float values[12];
    float height = 230;
    vector<float> measurements;

    for (int i = 0; i < 12; i++)
    {
        height = 230 - (i % 2) * 180; //pos1: 230, pos2: 50
        robot.move_pose(130, -180, height, 90, 90, 0);

        make_measurements(sensor, 500, measurements, 200);
        float average = 0;
        for (float m : measurements)
        {
            average += m;
        }
        average /= measurements.size();
        values[i] = average;
    }

    //print results
    for (float v : values)
    {
        cout << v << endl;
    }
    return 0;
}

void make_measurements(DistanceSensor &sensor, int number_of_measurements, vector<float> &measurements, unsigned int delay_us)
{
    float distance;
    for (int i = 0; i < number_of_measurements; i++)
    {
        distance = sensor.getDistanceInMillimeters();
        measurements.push_back(distance);
        usleep(delay_us);
    }
}
