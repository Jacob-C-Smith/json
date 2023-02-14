cmake_minimum_required (VERSION 3.8)

project ("json")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(${CMAKE_SOURCE_DIR}/dict)
find_library(dict NAMES dict HINTS ${CMAKE_SOURCE_DIR}/dict/build/Debug/*)

if(dict)
  message("Found dict")
else()
  message("${CMAKE_SOURCE_DIR}/dict/build/Debug/")
endif()


# Add source to this project's executable.
add_executable (json_example "JSON.c" "main.c")

# Add source to this project's library
add_library    (json SHARED "JSON.c")

target_link_libraries(json_example PUBLIC dict)
target_link_libraries(json PUBLIC dict)


# Define targets
target_include_directories(json_example PUBLIC "include/")
target_include_directories(json PUBLIC "include/")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET json PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
