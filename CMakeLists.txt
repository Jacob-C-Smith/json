cmake_minimum_required (VERSION 3.8)

project ("json")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

add_subdirectory("extern/dict" dict)
add_subdirectory("extern/array" array)

find_package(dict REQUIRED CONFIG REQUIRED COMPONENTS dict)
find_package(array REQUIRED CONFIG REQUIRED COMPONENTS array)

include_directories(${ARRAY_INCLUDE_DIRS})
include_directories(${DICT_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (json_example "json.c" "main.c")
add_dependencies(json_example array)
add_dependencies(json_example dict)

# Add source to this project's tester.
add_executable (json_test "json.c" "json_test.c")
add_dependencies(json_test array)
add_dependencies(json_test dict)

# Add source to this project's library
add_library (json SHARED "json.c")
add_dependencies(json array)
add_dependencies(json dict)

target_link_libraries(json_example LINK_PUBLIC  ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})
target_link_libraries(json_test LINK_PUBLIC  ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})
target_link_libraries(json LINK_PUBLIC  ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})

# Define targets
target_include_directories(json_example PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
target_include_directories(json_test PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
target_include_directories(json PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
