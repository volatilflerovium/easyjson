cmake_minimum_required(VERSION 3.18)
project("Movement Detection")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;${CMAKE_FIND_LIBRARY_SUFFIXES}")

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
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	if(ALLOW_DEBUG)
		add_definitions(-DALLOW_DEBUG)
	endif()
endif()

if(MEM_BLOCK_SIZE)
		add_definitions(-DJSON_MEMPOOL_BLOCK_SIZE=${MEM_BLOCK_SIZE})
endif()

if(RESERVED_BLOCKS)
	add_definitions(-DJSON_INITIAL_RESERVED_BLOCKS=${RESERVED_BLOCKS})
endif()

##--------------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

######################################################################

option(BUILD_SHARED_LIBS "Build the static library" ON)
option(BUILD_TEST "Build test" OFF)

if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
else()
	set(LIB_TYPE STATIC)
endif()

##--------------------------------------------------------------------

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_definitions(-D_WIN32)
	add_compile_options("/O2")
else()
	add_definitions(-D_LINUX)

		add_compile_options(
		-Os 
		-O3
		#-Ofast
		-march=native 
		-mtune=native
		-fno-math-errno 
		-fno-rtti
		
		-Wall
		-flto=auto
		#-flto
		-Wno-unused-function
	)

	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		add_compile_options(
			-g0
		)
	else()
		add_compile_options(
			# to generate asamble file:
			#-save-temps 
			#-fverbose-asm
			#-masm=intel
		)
	endif()
	
	if(BUILD_SHARED_LIBS)
		add_compile_options(
			-fPIC # for shared lib
		)
	endif()
endif()

#=====================================================================

set(EASYJSON_LIB easyjson)

add_library(
	"${EASYJSON_LIB}"
	"${LIB_TYPE}"
	src/json_core.cpp
	src/easyjson.cpp
)


target_include_directories(
	"${EASYJSON_LIB}"
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(
	"${EASYJSON_LIB}"
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

######################################################################
