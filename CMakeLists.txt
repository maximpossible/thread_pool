set(PROJECT_NAME thpool)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.5)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set("${PROJECT_NAME}_SRC"
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/main/src/main.cpp
)

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRC}
)

target_link_libraries(${PROJECT_NAME} pthread)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
