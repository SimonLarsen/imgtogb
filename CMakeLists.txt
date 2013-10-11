cmake_minimum_required(VERSION 2.8)
project(imgtogb)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_SOURCE_DIR "src")
set(CMAKE_BINARY_DIR "bin")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

set(SOURCES
	"${CMAKE_SOURCE_DIR}/imgtogb.cpp"
	"${CMAKE_SOURCE_DIR}/Tileset.cpp"
	"${CMAKE_SOURCE_DIR}/stb_image.c"
)

add_executable(
	"${CMAKE_BINARY_DIR}/imgtogb"
	${SOURCES}
)
