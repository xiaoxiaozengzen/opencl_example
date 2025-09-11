cmake_minimum_required(VERSION 3.23)
project(opencl_example)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# list(APPEND CMAKE_MODULE_PATH "/mnt/workspace/cgz_workspace/INSTALL/share/cmake/OpenCL")
list(INSERT CMAKE_PREFIX_PATH 0 "/mnt/workspace/cgz_workspace/INSTALL/share/cmake")

find_package(OpenCL REQUIRED NO_MODULE)
message(STATUS "Found OpenCL: ${OpenCL_FOUND}")
message(STATUS "OpenCL_DIR : ${OpenCL_DIR}")
message(STATUS "OpenCL_INCLUDE_DIRS: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL_LIBRARIES: ${OpenCL_LIBRARIES}")

find_package(tclap REQUIRED)
message(STATUS "Found tclap: ${tclap_FOUND}")
message(STATUS "tclap_DIR : ${tclap_DIR}")
message(STATUS "tclap_INCLUDE_DIRS: ${tclap_INCLUDE_DIRS}")
message(STATUS "tclap_LIBRARIES: ${tclap_LIBRARIES}")

add_executable(opencl_platform src/platform.cpp)
target_link_libraries(
  opencl_platform PUBLIC 
  OpenCL::Headers
  OpenCL::OpenCL
  OpenCL::HeadersCpp
  OpenCL::Utils
  OpenCL::UtilsCpp
)

add_executable(opencl_reduce src/reduce.cpp)
target_link_libraries(
  opencl_reduce PUBLIC 
  OpenCL::Headers
  OpenCL::OpenCL
  OpenCL::HeadersCpp
  OpenCL::Utils
  OpenCL::UtilsCpp
  tclap::tclap
)
target_include_directories(opencl_reduce PUBLIC ${tclap_INCLUDE_DIRS})

add_subdirectory(tclap)