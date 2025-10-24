cmake_minimum_required(VERSION 3.18)
project("Movement Detection")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(USE_STATIC)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;${CMAKE_FIND_LIBRARY_SUFFIXES}")
endif()
##--------------------------------------------------------------------

if(NDEBUG)
	add_definitions(-DNDEBUG=1)
endif() 

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
	add_definitions(-DDEBUG)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_compile_options("-Wno-comment")
	add_definitions(-DDEBUG_DIR="${CMAKE_SOURCE_DIR}")
endif()

add_definitions(-DALLOW_DEBUG)

##--------------------------------------------------------------------

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_definitions(-D_WIN32)
	add_compile_options("/O2")
else()
   if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	endif()

	add_definitions(-D_LINUX)

	add_compile_options(
	#-Os 
	-O3
	#-Ofast
	#-fno-math-errno 
	#-fno-rtti 
	-march=native 
	-mtune=native
	
	#-Wall
	-flto=auto
	#-fno-threadsafe-statics
	)
endif()

##--------------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

##--------------------------------------------------------------------

add_library(Profiler_lib INTERFACE)

target_include_directories(
	Profiler_lib
	INTERFACE
	"${CMAKE_SOURCE_DIR}/../../time_profiler/include"
)

message("${CMAKE_SOURCE_DIR}/../../time_profiler/include")

######################################################################

set(JSON_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

find_library(
	JSON_LIB
	NAMES easyjson
	PATHS "${JSON_LIB_DIR}" 
	REQUIRED
)

######################################################################

add_definitions(-DTEST_DATA_PATH="${CMAKE_SOURCE_DIR}/data")

##--------------------------------------------------------------------

set(App json_tests)

add_executable(
	"${App}"
	simple_json_tests.cpp
)

target_link_libraries(
	"${App}"
	PRIVATE
	Profiler_lib
	Debug_lib
	"${JSON_LIB}"
)

target_include_directories(
	"${App}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_compile_definitions(
	"${App}"
	PRIVATE
	TEST_FILE_PATH="${CMAKE_SOURCE_DIR}/data/test_data.txt"
)

########################################################################

set(App file_tests)

add_executable(
	"${App}"
	file_test.cpp
)

target_link_libraries(
	"${App}"
	PRIVATE
	Profiler_lib
	Debug_lib
	"${JSON_LIB}"
)

target_include_directories(
	"${App}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

#----------------------------

set(App file_tests_one)

add_executable(
	"${App}"
	file_test.cpp
)

target_link_libraries(
	"${App}"
	PRIVATE
	Profiler_lib
	Debug_lib
)

target_include_directories(
	"${App}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_compile_definitions(
	"${App}"
	PRIVATE
	-DONE_FILE_LIB
)

########################################################################

find_package(Threads REQUIRED)

set(App big_file_test)

add_executable(
	"${App}"
	big_file_test.cpp
)

target_link_libraries(
	"${App}"
	PRIVATE
	Threads::Threads
	Profiler_lib
	Debug_lib
	"${JSON_LIB}"
)

target_include_directories(
	"${App}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

##====================================================================

set(App big_file_test_one)

add_executable(
	"${App}"
	big_file_test.cpp
)

target_link_libraries(
	"${App}"
	PRIVATE
	Threads::Threads
	Profiler_lib
	Debug_lib
)

target_include_directories(
	"${App}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_compile_definitions(
	"${App}"
	PRIVATE
	-DONE_FILE_LIB
)

######################################################################
######################################################################

