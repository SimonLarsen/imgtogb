add_executable(imgtogb
	gb.cpp
	Main.cpp
	Tilemap.cpp
)

target_link_libraries(imgtogb
	pthread
	${Boost_LIBRARIES}
)
