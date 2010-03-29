cmake_minimum_required (VERSION 2.6)
project (Cimon)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -pedantic")

set(PROGRAMS cimond cimon-graph cimon-web)
set(CORELIBS rrd_th)

add_executable(cimond cimond.c memory.c cpu.c net.c log.c rrd.c)
add_executable(cimon-graph cimon-graph.c memory.c cpu.c net.c log.c rrd.c)
add_executable(cimon-web cimon-web.c memory.c cpu.c net.c log.c rrd.c render-index.c)

foreach(program ${PROGRAMS})
    target_link_libraries(${program} ${CORELIBS})
endforeach(program)
