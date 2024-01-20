# distance_sensor/CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(calibration_lib)

include_directories(
    include
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(calibration_lib SHARED ${SOURCES})

add_subdirectory(csvlogger)
add_subdirectory(distance_sensor)
add_subdirectory(meca500_ethercat_cpp)

target_link_libraries(calibration_lib PRIVATE csvlogger)
target_link_libraries(calibration_lib PRIVATE distance_sensor)
target_compile_options(calibration_lib PRIVATE -Wall -pthread)
target_link_libraries(calibration_lib PRIVATE pigpio rt)
target_link_libraries(calibration_lib PRIVATE meca500_driver)

target_compile_features(calibration_lib PRIVATE cxx_std_17)

target_include_directories(calibration_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(calibration_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)