project(imgtogbmap)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-Wall -ggdb")
set(CMAKE_SOURCE_DIR "src")
set(CMAKE_BINARY_DIR "bin")

set(SRC_CPP
	"${CMAKE_SOURCE_DIR}/imgtogbmap.cpp"
)

add_executable(
	${CMAKE_BINARY_DIR}/imgtogbmap ${SRC_CPP}
)
