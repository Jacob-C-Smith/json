cmake_minimum_required (VERSION 3.26)

project ("json")

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set the correct paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

if ( NOT "${HAS_ARRAY}")
    message("[JSON] Adding array project")
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array ${CMAKE_SOURCE_DIR}/build/array)
    set(HAS_ARRAY true PARENT_SCOPE)
endif()

if ( NOT "${HAS_DICT}")
    message("[JSON] Adding dict project")
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict ${CMAKE_SOURCE_DIR}/build/dict)
    set(HAS_DICT true PARENT_SCOPE)
endif()

if ( NOT "${HAS_JSON}")
    set(HAS_JSON true PARENT_SCOPE)
endif()

if ( NOT "${HAS_SYNC}")
    message("[JSON] Adding sync project")
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/sync ${CMAKE_SOURCE_DIR}/build/sync)
    set(HAS_SYNC true PARENT_SCOPE)
endif()

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if (${IS_DEBUG_BUILD})
    add_compile_definitions(NDEBUG)
endif ()

# Add source to this project's executable.
add_executable (json_example "main.c")
add_dependencies(json_example array dict sync)
target_include_directories(json_example PUBLIC include include/json ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/ ${CMAKE_SOURCE_DIR}/extern/sync/include/)
target_link_libraries(json_example json array dict sync)

## Add source to the tester
add_executable (json_test "json_test.c")
add_dependencies(json_test array dict sync)
target_include_directories(json_test PUBLIC include include/json ${CMAKE_SOURCE_DIR}/extern/array/include/ ${CMAKE_SOURCE_DIR}/extern/dict/include/ ${CMAKE_SOURCE_DIR}/extern/sync/include/)
target_link_libraries(json_test json array dict sync)

# Add source to this project's library
add_library (json SHARED "json.c")
add_dependencies(json array dict sync)
target_include_directories(json PUBLIC include include/json)
target_link_libraries(json PRIVATE array dict sync)
