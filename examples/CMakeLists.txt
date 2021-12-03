project(examples)
include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ../)
add_executable(beginner StopWatch.cpp)
add_executable(composite CompositeExample.cpp)
