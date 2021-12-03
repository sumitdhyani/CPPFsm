project(examples)
add_executable("${PROJECT_NAME}" StopWatch.h
				 StopWatch.cpp
				 CompositeExample.h
				 CompositeExample.cpp
				 main.cpp
	      )
target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ../)
