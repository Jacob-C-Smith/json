# Need CMake 3.16
cmake_minimum_required (VERSION 3.16)

# This is the name of the repository
project ("json")

# Set the correct paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/extern/)

# Set compiler warnings
if(MSVC)
    # TODO
else()

    # For clang
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lrt -std=gnu99 -Wall -Wextra  -Weverything -Wno-declaration-after-statement -Ofast")

    # For GCC
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wextra -O0 -pthread -lpthread")
    add_compile_definitions(__USE_POSIX199309)
    add_compile_options(-Wall -Wextra -Wpointer-arith -Wstrict-prototypes -Wformat-security -Wfloat-equal -Wshadow -Wconversion -pthread -lpthread -Wlogical-not-parentheses -Wnull-dereference)
endif()

# Comment out for Debug mode
set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

# Build sync with timer
add_compile_definitions(BUILD_SYNC_WITH_TIMER)

# Build sync with mutex
add_compile_definitions(BUILD_SYNC_WITH_MUTEX)

# Find the hash cache module
if ( NOT "${HAS_HASH_CACHE}")

    # Add the hash cache directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/hash-cache ${CMAKE_SOURCE_DIR}/build/hash-cache)

    # Let any parent project know that this project has hash cache
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_HASH_CACHE true PARENT_SCOPE)
        set(HASH_CACHE_INCLUDE_DIR ${HASH_CACHE_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()

# Find the array module
if ( NOT "${HAS_ARRAY}")
    
    # Add the array directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/array ${CMAKE_SOURCE_DIR}/build/array)

    # Let any parent project know that this project has array
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_ARRAY true PARENT_SCOPE)
        set(ARRAY_INCLUDE_DIR ${ARRAY_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()

# Find the dict module
if ( NOT "${HAS_DICT}")
    
    # Add the dict directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/dict ${CMAKE_SOURCE_DIR}/build/dict)

    # Let any parent project know that this project has dict
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_DICT true PARENT_SCOPE)
        set(DICT_INCLUDE_DIR ${DICT_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()

# Find the sync module
if ( NOT "${HAS_SYNC}")

    # Add the sync directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/sync ${CMAKE_SOURCE_DIR}/build/sync)

    # Let any parent project know that this project has sync
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_SYNC true PARENT_SCOPE)
        set(SYNC_INCLUDE_DIR ${SYNC_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()

# Find the log module
if ( NOT "${HAS_LOG}")

    # Add the log directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/extern/log ${CMAKE_SOURCE_DIR}/build/log)

    # Let any parent project know that this project has log
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_LOG true PARENT_SCOPE)
        set(LOG_INCLUDE_DIR ${LOG_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()

# Find the JSON module
if ( NOT "${HAS_JSON}")

    # Log
    message("[json] Adding json project")

    # Set the json include
    set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

    # Let any parent project know that it has the json project
    if(NOT PROJECT_IS_TOP_LEVEL)
        set(HAS_JSON true PARENT_SCOPE)
        set(JSON_INCLUDE_DIR ${JSON_INCLUDE_DIR} PARENT_SCOPE)
    endif()
endif()


# Add source to this project's executable.
add_executable (json_example "main.c")
add_dependencies(json_example json)
target_include_directories(json_example PUBLIC ${JSON_INCLUDE_DIR} ${ARRAY_INCLUDE_DIR} ${DICT_INCLUDE_DIR})
target_link_libraries(json_example json array dict sync)

## Add source to the tester
add_executable (json_test "json_test.c")
add_dependencies(json_test json array dict sync log)
target_include_directories(json_test PUBLIC ${JSON_INCLUDE_DIR} ${LOG_INCLUDE_DIR} ${ARRAY_INCLUDE_DIR} ${CRYPTO_INCLUDE_DIR} ${DICT_INCLUDE_DIR} ${SYNC_INCLUDE_DIR})
target_link_libraries(json_test json array dict sync log m)

# Add source to this project's library
add_library (json SHARED "json.c")
add_dependencies(json array dict sync)
target_include_directories(json PUBLIC ${JSON_INCLUDE_DIR} ${ARRAY_INCLUDE_DIR} ${CRYPTO_INCLUDE_DIR} ${DICT_INCLUDE_DIR} ${SYNC_INCLUDE_DIR})
target_link_libraries(json PUBLIC array dict sync m)

# Add source to this project's tester library
add_library (json_test_lib SHARED "json_test.c")
add_dependencies(json_test_lib json array dict sync log)
target_include_directories(json_test_lib PUBLIC ${JSON_INCLUDE_DIR} ${LOG_INCLUDE_DIR} ${ARRAY_INCLUDE_DIR} ${CRYPTO_INCLUDE_DIR} ${DICT_INCLUDE_DIR} ${SYNC_INCLUDE_DIR})
target_link_libraries(json_test_lib PUBLIC json array dict sync log m)
