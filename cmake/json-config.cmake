# JSON CMake configuration file:
# This file is meant to be placed in a cmake subfolder of JSON-devel-2.x.y-VC
cmake_minimum_required(VERSION 3.26)

include(FeatureSummary)
set_package_properties(JSON PROPERTIES
    URL "https://www.g10.app/status#parser_serializer"
    DESCRIPTION "JSON parser/serializer"
)

# Copied from `configure_package_config_file`
macro(set_and_check _var _file)
    set(${_var} "${_file}")
    if(NOT EXISTS "${_file}")
        message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
    endif()
endmacro()

# Copied from `configure_package_config_file`
macro(check_required_components _NAME)
    foreach(comp ${${_NAME}_FIND_COMPONENTS})
        if(NOT ${_NAME}_${comp}_FOUND)
            if(${_NAME}_FIND_REQUIRED_${comp})
                set(${_NAME}_FOUND FALSE)
            endif()
        endif()
    endforeach()
endmacro()

set(JSON_FOUND TRUE)

# For compatibility with autotools JSON-config.cmake, provide JSON_* variables.

set_and_check(JSON_PREFIX       "${CMAKE_CURRENT_LIST_DIR}/..")
set_and_check(JSON_EXEC_PREFIX  "${CMAKE_CURRENT_LIST_DIR}/..")
set_and_check(JSON_INCLUDE_DIR  "${JSON_PREFIX}/include")
set(JSON_INCLUDE_DIRS           "${JSON_INCLUDE_DIR}")
set_and_check(JSON_BINDIR       "${CMAKE_CURRENT_LIST_DIR}/../build/Debug/")
set_and_check(JSON_LIBDIR       "${CMAKE_CURRENT_LIST_DIR}/../build/Debug/")

set(JSON_LIBRARIES json::json)

# All targets are created, even when some might not be requested though COMPONENTS.
# This is done for compatibility with CMake generated JSON-target.cmake files.

set(_JSON_library     "${JSON_LIBDIR}/json.lib")
set(_JSON_dll_library "${JSON_BINDIR}/json.dll")
if(EXISTS "${_JSON_library}" AND EXISTS "${_JSON_dll_library}")
    if(NOT TARGET json::json)
        add_library(json::json SHARED IMPORTED)
        set_target_properties(json::json
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${JSON_INCLUDE_DIRS}"
                IMPORTED_IMPLIB "${_JSON_library}"
                IMPORTED_LOCATION "${_JSON_dll_library}"
                COMPATIBLE_INTERFACE_BOOL "JSON_SHARED"
                INTERFACE_JSON_SHARED "ON"
        )
    endif()
    set(JSON_JSON_FOUND TRUE)
else()
    set(JSON_JSON_FOUND FALSE)
endif()
unset(_JSON_library)
unset(_JSON_dll_library)

check_required_components(JSON)
