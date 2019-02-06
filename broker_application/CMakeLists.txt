project(event-broker)

add_executable(${PROJECT_NAME}
  inc/broker.hpp
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt5::Core
  PRIVATE utilities
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
