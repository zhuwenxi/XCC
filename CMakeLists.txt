cmake_minimum_required (VERSION 2.6)

# project name: XCC
project(XCC C)

# options
include(options.cmake)

# "include"
include_directories(include include/util include/scanner)

# "src"
add_subdirectory(src)

add_executable(xcc src/main.c ${CORE_SRC})


if (WIN32)
	# to make Visual studio happy...
	add_definitions(-Dinline=__inline)
	add_definitions(-DWINDOWS)
endif()

# build test binary
if (BUILD_TEST)
	FILE(GLOB TEST_SRC test/util/*.c test/*.c)
	add_executable(test_all ${TEST_SRC} ${CORE_SRC})
endif()