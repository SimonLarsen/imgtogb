project(imgtogbmap)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_SOURCE_DIR "src")
set(CMAKE_BINARY_DIR "bin")

set(GBMAP_CPP
	"${CMAKE_SOURCE_DIR}/imgtogbmap.cpp"
	"${CMAKE_SOURCE_DIR}/common.cpp"
	"${CMAKE_SOURCE_DIR}/stb_image.c"
)

set(GBSPR_CPP
	"${CMAKE_SOURCE_DIR}/imgtogbspr.cpp"
	"${CMAKE_SOURCE_DIR}/common.cpp"
	"${CMAKE_SOURCE_DIR}/stb_image.c"
)

add_executable(
	${CMAKE_BINARY_DIR}/imgtogbmap
	${GBMAP_CPP}
)

add_executable(
	${CMAKE_BINARY_DIR}/imgtogbspr
	${GBSPR_CPP}
)
