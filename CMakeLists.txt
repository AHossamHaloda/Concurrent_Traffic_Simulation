cmake_minimum_required(VERSION 3.11.3)

# Set the project name
project(traffic_simulation)

# Set the C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)          # Use C++17 standard
set(CMAKE_CXX_STANDARD_REQUIRED True) # Require C++17. if the compiler does not support c++17 generate a cmake error 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # Add pthread support

# Find the OpenCV package
find_package(OpenCV 4.1 REQUIRED)

# Include OpenCV directories and link directories
include_directories(${OpenCV_INCLUDE_DIRS}) # Include OpenCV header files
link_directories(${OpenCV_LIBRARY_DIRS})    # Link OpenCV libraries
add_definitions(${OpenCV_DEFINITIONS})      # Add any OpenCV-specific preprocessor definitions

# Find all source files in the 'src' directory
file(GLOB project_SRCS src/*.cpp) # Include all .cpp files in the src directory

# Add an executable target
add_executable(traffic_simulation ${project_SRCS}) # Create the executable from the source files

# Link OpenCV libraries to the target
target_link_libraries(traffic_simulation ${OpenCV_LIBRARIES}) # Link OpenCV libraries to the executable

# Set the RPATH for the target to locate the OpenCV libraries at runtime
# Use BUILD_RPATH to set RPATH during the build process
set_target_properties(traffic_simulation PROPERTIES BUILD_RPATH "/usr/local/lib")

# Uncomment the following line to set RPATH when the target is installed
# set_target_properties(traffic_simulation PROPERTIES INSTALL_RPATH "/usr/local/lib")