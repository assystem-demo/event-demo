project(event-client)

add_executable(${PROJECT_NAME}
	inc/client.hpp
	src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Qt5::Core
)
