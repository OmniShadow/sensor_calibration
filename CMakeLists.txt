cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
project(calibrate VERSION 0.1.0 LANGUAGES C CXX)

add_executable(calibrate calibration.cpp)

target_link_libraries(calibrate PRIVATE csvlogger)
target_link_libraries(calibrate PRIVATE distance_sensor)
target_link_libraries(calibrate PRIVATE meca500_driver)
target_link_libraries(calibrate PRIVATE calibration_lib)

target_compile_options(calibrate PRIVATE -Wall -pthread)
target_link_libraries(calibrate PRIVATE pigpio rt)

target_compile_features(calibrate PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
