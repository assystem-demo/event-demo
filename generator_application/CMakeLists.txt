project(event-generator)

add_executable(${PROJECT_NAME}
	inc/generator.hpp
	src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Qt5::Core
)
