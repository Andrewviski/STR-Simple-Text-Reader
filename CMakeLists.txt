cmake_minimum_required(VERSION 2.8)
project( STR )
find_package( OpenCV REQUIRED )
add_executable( STR src/STR.cpp )
target_link_libraries( STR ${OpenCV_LIBS} )

