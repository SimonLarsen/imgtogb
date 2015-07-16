add_executable(imgtogb
	gb.cpp
	Main.cpp
	Tilemap.cpp
	rle.cpp
)

target_link_libraries(imgtogb
	pthread
	${Boost_LIBRARIES}
)
