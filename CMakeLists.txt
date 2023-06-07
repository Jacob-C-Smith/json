cmake_minimum_required (VERSION 3.26.0)

project ("json")

add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array)
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict)

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if (${IS_DEBUG_BUILD})
    add_compile_definitions(NDEBUG)
endif ()

# Add source to this project's executable.
add_executable (json_example "main.c")
add_dependencies(json_example array dict)
target_include_directories(json_example PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/)
target_link_libraries(json_example json)

## Add source to the tester
add_executable (json_test "json_test.c")
add_dependencies(json_test array dict)
target_include_directories(json_test PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/)
target_link_libraries(json_test json)

# Add source to this project's library
add_library (json "json.c")
add_dependencies(json array dict)
target_include_directories(json PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/)
target_link_libraries(json PRIVATE array dict)
