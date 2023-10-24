/** !
 * @file json/json.h 
 * @author Jacob Smith
 * 
 * Include header for json library
 */

// Include guard
#pragma once

// Standard library
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <errno.h>
#include <ctype.h>

// sync submodule
#include <sync/sync.h>

// dict submodule
#include <dict/dict.h>

// array submodule
#include <array/array.h>

// Forward declared functions
void free_token ( void *ptr );

// Platform dependent macros
#ifdef _WIN64
#define DLLEXPORT extern __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// This macro is used to free json_value_t *'s 
#define FREE_VALUE( value ) free_json_value(value)

// Set the reallocator for the dict submodule
#ifdef DICT_REALLOC
    #undef DICT_REALLOC
    #define DICT_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the array submodule
#ifdef ARRAY_REALLOC
    #undef ARRAY_REALLOC
    #define ARRAY_REALLOC(p, sz) realloc(p, sz)
#endif

// Memory management macro
#ifndef JSON_REALLOC
#define JSON_REALLOC(p, sz) realloc(p,sz)
#endif

// Enumerations
enum json_value_type_e
{
    JSON_VALUE_OBJECT,
    JSON_VALUE_ARRAY,
    JSON_VALUE_STRING,
    JSON_VALUE_BOOLEAN,
    JSON_VALUE_INTEGER,
    JSON_VALUE_NUMBER
};

// Structures
struct json_value_s
{
    union {
        char             *string;
        signed long long  integer;
        double            number;
        dict             *object;
        array            *list;
        bool              boolean;
    };
    enum json_value_type_e type;  // Type
};

// Type definitions
typedef struct json_value_s json_value;

/** !
 * Parse json text into a json_value
 * 
 * @param text pointer to json text
 * @param return_pointer null or pointer to end of json value
 * @param pp_value return
 * 
 * @return 1 on success, 0 on error
 */
DLLEXPORT int parse_json_value ( char *text, const char **const return_pointer, const json_value **const pp_value );

/** !
 * Serialize a json_value to a file
 * 
 * @param p_value pointer to json_value
 * @param f the file to write to
 * 
 * @return 1 on success, 0 on error
 */
DLLEXPORT int print_json_value ( const json_value *const p_value , FILE *f );

/** ! 
 * Free a json value, and its contents
 * 
 * @param p_value pointer to json_value
 *  
 * @return void
 */
DLLEXPORT void free_json_value ( const json_value *const p_value );