project(tclap_example)

add_executable(tclap_example tclap_example.cpp)
target_include_directories(tclap_example PUBLIC ${tclap_INCLUDE_DIRS})
target_link_libraries(tclap_example ${tclap_LIBRARIES})