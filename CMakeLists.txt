cmake_minimum_required(VERSION 3.23)
project(opencl_example)

# list(APPEND CMAKE_MODULE_PATH "/mnt/workspace/cgz_workspace/INSTALL/share/cmake/OpenCL")
list(INSERT CMAKE_PREFIX_PATH 0 "/mnt/workspace/cgz_workspace/INSTALL/share/cmake")

find_package(OpenCL REQUIRED NO_MODULE)
message(STATUS "Found OpenCL: ${OpenCL_FOUND}")
message(STATUS "OpenCL_DIR : ${OpenCL_DIR}")
message(STATUS "OpenCL_INCLUDE_DIRS: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL_LIBRARIES: ${OpenCL_LIBRARIES}")

set(CMAKE_CXX_STANDARD 14)

add_executable(opencl_example src/main.cpp)
target_link_libraries(
  opencl_example PUBLIC 
  OpenCL::Headers
  OpenCL::OpenCL
  OpenCL::HeadersCpp
  OpenCL::Utils
  OpenCL::UtilsCpp
)