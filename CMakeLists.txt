cmake_minimum_required (VERSION 2.6)
project (cimon)
set(CPACK_PACKAGE_CONTACT "Daniel Milde <info@milde.cz>")
#set(CPACK_GENERATOR "TGZ;DEB")
#include(CPack)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -pedantic")

add_subdirectory(tests)

configure_file("config.h.cmake" "${CMAKE_SOURCE_DIR}/config.h")
set(CONFIG_FILE "/etc/cimon.conf" CACHE FILEPATH "Where configuration should be stored")
option(DEBUG_MODE "Enable verbose mode" ON)

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
set(CORELIBS rrd_th microhttpd)
set(SOURCES cimond.c memory.c cpu.c net.c swap.c log.c rrd.c utils.c http_server.c cache.c render_index.c config_reader.c)

add_executable(cimond ${SOURCES})
add_library(cimonlib SHARED ${SOURCES})

foreach(program ${PROGRAMS})
    target_link_libraries(${program} ${CORELIBS})
endforeach(program)

install(TARGETS cimond RUNTIME DESTINATION "/usr/sbin/")