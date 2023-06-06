cmake_minimum_required (VERSION 3.8)

project ("json")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

add_subdirectory("extern/dict")
add_subdirectory("extern/array")

find_package(array REQUIRED CONFIG REQUIRED COMPONENTS array)
find_package(dict REQUIRED CONFIG REQUIRED COMPONENTS dict)

include_directories(${ARRAY_INCLUDE_DIRS})
include_directories(${DICT_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (json_example "json.c" "main.c")

# Add source to this project's tester.
add_executable (json_test "json.c" "json_test.c")

# Add source to this project's library
add_library    (json SHARED "json.c")

target_link_libraries(json_example PUBLIC ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})
target_link_libraries(json_test PUBLIC ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})
target_link_libraries(json PUBLIC ${ARRAY_LIBRARIES} ${DICT_LIBRARIES})

# Define targets
target_include_directories(json_example PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
target_include_directories(json_test PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
target_include_directories(json PUBLIC "include/" ${DICT_INCLUDE_DIRS} ${ARRAY_INCLUDE_DIRS})
