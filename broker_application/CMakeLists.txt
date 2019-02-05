project(event-broker)

add_executable(${PROJECT_NAME}
	inc/broker.hpp
	src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Qt5::Core
)
