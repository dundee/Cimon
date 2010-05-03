cmake_minimum_required (VERSION 2.6)
project (Cimon)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -pedantic")

set(DATA_DIR "\"/var/lib/cimon\"" CACHE PATH "Where databases and images should be stored")
set(REFRESH_GRAPH_INTERVAL 600 CACHE STRING "How often in seconds will be graphs refreshed")
set(PORT 8080 CACHE STRING "Port for HTTP server")

configure_file("config.h.cmake" "${CMAKE_SOURCE_DIR}/config.h")
option(DEBUG_MODE "Enable verbose mode" OFF)

find_library(RRD_LIB NAMES "rrd_th")
if (RRD_LIB)
	MESSAGE(STATUS "RRD library found in ${RRD_LIB}")
else(RRD_LIB)
	MESSAGE(FATAL_ERROR "Library RRD not found.")
endif(RRD_LIB)

if(NOT UNIX)
	MESSAGE(FATAL_ERROR "Only UNIX systems are supported.")
endif(NOT UNIX)

set(PROGRAMS cimond)
set(CORELIBS rrd_th)

add_executable(cimond cimond.c memory.c cpu.c net.c log.c rrd.c utils.c web.c cache.c render-index.c)

foreach(program ${PROGRAMS})
    target_link_libraries(${program} ${CORELIBS})
endforeach(program)
