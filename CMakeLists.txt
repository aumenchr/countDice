cmake_minimum_required(VERSION 2.8)
project( CountDice )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} include)
add_executable( CountDice src/main.cpp src/Result.cpp include/Result.hpp include/CountDice.hpp)
target_link_libraries( CountDice ${OpenCV_LIBS} )
