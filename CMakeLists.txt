cmake_minimum_required(VERSION 3.11.0)
project(csgo-autoaccept-cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_executable(csgo-autoaccept-cpp main.cpp)

find_package(OpenCV REQUIRED)
target_link_libraries(csgo-autoaccept-cpp ${OpenCV_LIBS})

find_package(X11 COMPONENTS X11 Xtst)
target_link_libraries(csgo-autoaccept-cpp ${X11_LIBRARIES} ${X11_Xtst_LIB})