# sensor_calibration

This is a folder where u can find tools to study a sensor, including realizing calibration, extimating execution time ecc..

## How to compile

If you are using cmake, after you've cloned the repository into your disposal, digit (by cmd) the command "make"

## How to execute 1st program: calibration

If you desire to find calibration curve for your sensor, set the parameters in "config.txt" and run "calibrazione --config=config.txt".
Put the obstacle in zero position and wait for Meca500 to take data. It will write some csv files in ./measurements/..

Then to analyze execute the python script "analyze.py" passing the specified path. It will generates some plots in the analyzed folders.

If you want to study the sensor precision, you can study the dev_std executing the python script "plot_dev_std.py"


## How to execute 2nd program: two surface

If you desire to compare the measurements between to surface, place them opposite to the sensor, one above the other and run "duesuperfici"
It will write a txt file.

To analyze the datas and to create the grid you can find in two_surface_check, just put in the code the measurements and execute "twosurfacetest.py" 


## How to execute 3rd program: sensor delay

If you desire to extimate extimation time of your sensor, put the obstacle opposite to the sensor and run "ritardo".
It will write a csv file in ./sensor_delay

