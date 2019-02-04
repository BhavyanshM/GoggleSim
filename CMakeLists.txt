cmake_minimum_required(VERSION 3.0)
project(flightgoggles_client_bindings)

################
# USER SETTINGS
################

# Turn this on to compile ROS bindings.
set(COMPILE_ROSCLIENT ON)

################
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set(CMAKE_CXX_STANDARD 11)

# Allow for optimized code with debug symbols.
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# Fix cuda linking issues
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

# Find CUDA
find_package(CUDA)

# Find OpenCV
find_package( OpenCV REQUIRED )

# Find Eigen
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Only compile ROS bindings if asked.
if(NOT COMPILE_ROSCLIENT)
  message(STATUS "Not building ROSClient since COMPILE_ROSCLIENT=OFF. Edit the top level CMakeLists.txt to compile ROS bindings.")
  set(COMPILE_ROSCLIENT OFF)
else()
  message(STATUS "COMPILE_ROSCLIENT=ON, so building ROSClient.")
  
  # Init Catkin
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    tf
    tf2
    nav_msgs
    sensor_msgs
    geometry_msgs
  )
  catkin_package(
    LIBRARIES simulator
    CATKIN_DEPENDS roscpp rospy std_msgs
    # DEPENDS system_lib
    tf
    tf2
  )
endif()

add_subdirectory(src)


