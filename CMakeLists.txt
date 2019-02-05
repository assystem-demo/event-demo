cmake_minimum_required (VERSION 3.4)

project (EventDemo)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O -Werror -Wall -fPIC ")
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)

enable_testing()

add_subdirectory(client_application)
add_subdirectory(generator_application)
add_subdirectory(broker_application)
