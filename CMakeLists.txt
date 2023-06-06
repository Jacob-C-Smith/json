cmake_minimum_required (VERSION 3.8)

project ("json")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/array)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/array)

# Add the dictionary submodule
find_library(dict NAMES dict HINTS ${CMAKE_SOURCE_DIR}/extern/dict/build/Debug/*)

# Add the array submodule
find_library(array NAMES array HINTS ${CMAKE_SOURCE_DIR}/extern/array/build/Debug/*)

# Add source to this project's executable.
add_executable (json_example "json.c" "main.c")

# Add source to this project's tester.
add_executable (json_test "json.c" "json_test.c")

# Add source to this project's library
add_library    (json SHARED "json.c")

target_link_libraries(json_example PUBLIC dict array)
target_link_libraries(json_test PUBLIC dict array)
target_link_libraries(json PUBLIC dict array)

# Define targets
target_include_directories(json_example PUBLIC "include/")
target_include_directories(json_test PUBLIC "include/")
target_include_directories(json PUBLIC "include/")
