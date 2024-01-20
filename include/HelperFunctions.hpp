#ifndef HELPER_FUNCTIONS_HPP
#define HELPER_FUNCTIONS_HPP

#include <stdio.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <string>
#include <map>

using namespace std;

void display_usage();
void setup_handlers();
int setup_options(map<string, string> options);
map<string, string> parse_command_line(int argc, char *argv[]);
map<string, string> parse_config_file(string config_file_path);
vector<float> parse_string_to_vector(string input);
void move_robot_to_position(vector<float> robot_position);
void initialise_robot();
void make_measurements(DistanceSensor &sensor, int number_of_measurements, vector<float> &measurements, unsigned int delay_us); // function to measure the distance with the given sensor
void write_measurements_to_csv(vector<float> measurments, string file_path);                                                    // unction to write to taken measurements to a csv file

#endif