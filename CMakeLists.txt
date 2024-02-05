cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
project(calibrazione VERSION 0.1.0 LANGUAGES C CXX)


add_executable(calibrazione calibration.cpp)
add_executable(ritardo sensor_delay.cpp)
add_executable(duesuperfici twosurfacescheck.cpp)



add_subdirectory(csvlogger)
add_subdirectory(distance_sensor)
add_subdirectory(meca500_ethercat_cpp)


target_link_libraries(calibrazione PRIVATE csvlogger)
target_link_libraries(calibrazione PRIVATE distance_sensor)
target_compile_options(calibrazione PRIVATE -Wall -pthread)
target_link_libraries(calibrazione PRIVATE pigpio rt)
target_link_libraries(calibrazione PRIVATE meca500_driver)


target_link_libraries(ritardo PRIVATE csvlogger)
target_link_libraries(ritardo PRIVATE distance_sensor)
target_compile_options(ritardo PRIVATE -Wall -pthread)
target_link_libraries(ritardo PRIVATE pigpio rt)

target_link_libraries(duesuperfici PRIVATE csvlogger)
target_link_libraries(duesuperfici PRIVATE distance_sensor)
target_compile_options(duesuperfici PRIVATE -Wall -pthread)
target_link_libraries(duesuperfici PRIVATE pigpio rt)
target_link_libraries(duesuperfici PRIVATE meca500_driver)


target_compile_features(calibrazione PRIVATE cxx_std_17)
target_compile_features(ritardo PRIVATE cxx_std_17)
target_compile_features(duesuperfici PRIVATE cxx_std_17)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
