cmake_minimum_required (VERSION 3.8)

project ("json")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add the dictionary submodule
add_subdirectory(${PROJECT_SOURCE_DIR}/dict)
find_library(dict NAMES dict HINTS ${PROJECT_SOURCE_DIR}/dict/build/Debug/*)

# Add the array submodule
add_subdirectory(${PROJECT_SOURCE_DIR}/array)
find_library(array NAMES array HINTS ${PROJECT_SOURCE_DIR}/array/build/Debug/*)


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
