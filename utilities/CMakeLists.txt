project(utilities)

add_library(${PROJECT_NAME}
  inc/handshaker.hpp
  src/handshaker.cpp
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC Qt5::Network
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
