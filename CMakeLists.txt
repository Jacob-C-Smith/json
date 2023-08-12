# Need CMake 3.20
cmake_minimum_required (VERSION 3.20)

# This is the name of the repository
project ("json")

# Set the correct paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set for debug mode
if (${IS_DEBUG_BUILD})
    add_compile_definitions(NDEBUG)
endif ()

# Find the array module
if ( NOT "${HAS_ARRAY}")
    
    # Log
    message("[JSON] Adding array project")
    
    # Add the array directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array ${CMAKE_SOURCE_DIR}/build/array)

    # Let any parent project know that this project has array
    set(HAS_ARRAY true PARENT_SCOPE)
endif()

# Find the dict module
if ( NOT "${HAS_DICT}")
    
    # Log
    message("[JSON] Adding dict project")
    
    # Add the dict directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict ${CMAKE_SOURCE_DIR}/build/dict)

    # Let any parent project know that this project has dict
    set(HAS_DICT true PARENT_SCOPE)
endif()

# Find the sync module
if ( NOT "${HAS_SYNC}")

    # Log
    message("[JSON] Adding sync project")

    # Add the sync directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/sync ${CMAKE_SOURCE_DIR}/build/sync)

    # Let any parent project know that this project has sync
    set(HAS_SYNC true PARENT_SCOPE)
endif()

# Find the JSON module
if ( NOT "${HAS_JSON}")

    # Log
    message("[JSON] Adding json project")

    # Let any parent project know that it has the json project
    set(HAS_JSON true PARENT_SCOPE)
endif()

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

# Add source to this project's tester library
add_library (lib_json_test SHARED "json_test.c")
add_dependencies(lib_json_test json array dict sync)
target_include_directories(lib_json_test PUBLIC include include/json)
target_link_libraries(lib_json_test PRIVATE json array dict sync)