cmake_minimum_required (VERSION 3.26)

project ("json")

if ( HAS_ARRAY )
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array ${CMAKE_SOURCE_DIR}/build/array)
endif()

if ( HAS_DICT )
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict ${CMAKE_SOURCE_DIR}/build/dict)
endif()

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if (${IS_DEBUG_BUILD})
    add_compile_definitions(NDEBUG)
endif ()

# Add source to this project's executable.
add_executable (json_example "main.c")
add_dependencies(json_example array dict)
target_include_directories(json_example PUBLIC include include/json ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/)
target_link_libraries(json_example json array dict)

## Add source to the tester
add_executable (json_test "json_test.c")
add_dependencies(json_test array dict)
target_include_directories(json_test PUBLIC include include/json ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/)
target_link_libraries(json_test json array dict)

# Add source to this project's library
add_library (json SHARED "json.c")
add_dependencies(json array dict)
target_include_directories(json PUBLIC include include/json)
target_link_libraries(json PRIVATE array dict)
