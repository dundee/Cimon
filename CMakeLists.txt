cmake_minimum_required (VERSION 2.6)
project (Cimon)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -pedantic")

set(DATA_DIR "\"/var/lib/cimon\"" CACHE PATH "Where databases and images should be stored")

configure_file("config.h.cmake" "${CMAKE_SOURCE_DIR}/config.h")

find_library(RRD_LIB NAMES "rrd_th")
if (RRD_LIB)
	MESSAGE(STATUS "RRD library found in ${RRD_LIB}")
else(RRD_LIB)
	MESSAGE(FATAL_ERROR "Library RRD not found.")
endif(RRD_LIB)

if(NOT UNIX)
	MESSAGE(FATAL_ERROR "Only UNIX systems are supported.")
endif(NOT UNIX)

set(PROGRAMS cimond cimon-graph cimon-web)
set(CORELIBS rrd_th)

add_executable(cimond cimond.c memory.c cpu.c net.c log.c rrd.c utils.c)
add_executable(cimon-graph cimon-graph.c memory.c cpu.c net.c log.c rrd.c utils.c)
add_executable(cimon-web cimon-web.c memory.c cpu.c net.c log.c rrd.c utils.c render-index.c cache.c)

foreach(program ${PROGRAMS})
    target_link_libraries(${program} ${CORELIBS})
endforeach(program)
