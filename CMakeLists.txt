set(PROJECT_NAME thpool)
set(UTIL_NAME util)

project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.5)

# Set compile properties
add_compile_options(-std=c++11 -Wall -Wextra -pedantic -Werror)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
set(${PROJECT_NAME}_MAIN_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/main/src/main.cpp
)

set(${PROJECT_NAME}_TASK_QUEUE_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/task_queue/src/task_queue.cpp
)

set(${PROJECT_NAME}_THREAD_POOL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/thread_pool/src/thread_pool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/thread_pool/src/thread_object.cpp
)

# Include files
set(${PROJECT_NAME}_TASK_QUEUE_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/task_queue/task_queue.h
)

set(${PROJECT_NAME}_THREAD_POOL_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/thread_pool/thread_pool.h
)

file(COPY ${${PROJECT_NAME}_TASK_QUEUE_HEADERS} DESTINATION include/task_queue)
file(COPY ${${PROJECT_NAME}_THREAD_POOL_HEADERS} DESTINATION include/thread_pool)

# Create static library
add_library(${PROJECT_NAME} STATIC
  ${${PROJECT_NAME}_TASK_QUEUE_SRC}
  ${${PROJECT_NAME}_THREAD_POOL_SRC}
)

# Create executable
add_executable(${UTIL_NAME}
  ${${PROJECT_NAME}_MAIN_SRC}
)

# Link libraries
target_link_libraries(${UTIL_NAME} ${PROJECT_NAME} pthread)

# Static analysis
find_program(CPP_CHECK cppcheck)
if(NOT CPP_CHECK)
    message(WARNING "${CPP_CHECK} not found!")
else()
    add_custom_target(check ${UTIL_NAME}
      COMMAND ${CPP_CHECK} --suppress=missingIncludeSystem --enable=all --inconclusive --check-config ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
    )
endif()

# Documentation
find_program(DOXYGEN doxygen)
if(NOT DOXYGEN)
    message(WARNING "${DOXYGEN} not found!")
else()
    add_custom_target(docs ${UTIL_NAME}
      COMMAND ${DOXYGEN} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/Doxyfile
    )
endif()
