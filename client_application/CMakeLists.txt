project(event-client)

add_executable(${PROJECT_NAME}
  inc/client.hpp
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt5::Core
  PUBLIC utilities
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
