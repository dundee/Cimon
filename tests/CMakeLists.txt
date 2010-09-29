cmake_minimum_required (VERSION 2.6)
project (Cimon)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -pedantic")


find_library(CUNIT_LIB NAMES "cunit")
if (NOT CUNIT_LIB)
	MESSAGE(FATAL_ERROR "Cunit library not found.")
endif(NOT CUNIT_LIB)

if(NOT UNIX)
	MESSAGE(FATAL_ERROR "Only UNIX systems are supported.")
endif(NOT UNIX)

add_executable(run_tests run_tests.c test_config_reader.c)
target_link_libraries(run_tests cunit rrd_th microhttpd cimonlib)