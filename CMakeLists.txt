cmake_minimum_required(VERSION 2.8)
project(imgtogb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/tclap/include
	${CMAKE_SOURCE_DIR}/CImg
	${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(src/imgtogb)
