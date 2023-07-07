/** !
 * @file JSON/JSON.h 
 * @author Jacob Smith
 * 
 * Include header for JSON parser library
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

#define DICT_SIZE 16

// Forward declared functions
void free_token ( void *ptr );

// Platform dependent macros
#ifdef _WIN64
#define DLLEXPORT extern __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// This macro is used to free JSONValue_t *'s 
#define FREE_VALUE( value ) free_json_value(value)

// This macro will evaluate to the value of the JSON property, if the property is not a null pointer and if the type matches the parameter 't'
#define JSON_VALUE( property, t ) (property) ? (property->type==t) ? property->integer : 0 : 0;
#define JSON_EVALUATE( value, variable, type ) evaluate_json_value(value, &variable, type)

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
enum JSONValueType_e
{
    JSONobject,
    JSONarray,
    JSONstring,
    JSONboolean,
    JSONinteger,
    JSONfloat
};

// Structures
struct JSONValue_s
{
    union {
        char             *string;
        signed long long  integer;
        double            floating;
        dict             *object;
        array            *list;
        bool              boolean;
    };
    enum JSONValueType_e type;  // Type
};

// Type definitions
typedef struct JSONValue_s JSONValue_t;

/** !
 * Parse json text into a JSONValue
 * 
 * @param text pointer to JSON text
 * @param return_pointer null or pointer to end of JSON value
 * @param pp_value return
 * 
 * @return 1 on success, 0 on error
 */
DLLEXPORT int parse_json_value ( char *text, char **return_pointer, JSONValue_t **pp_value );

/** !
 * Serialize a JSONValue to a file
 * 
 * @param p_value pointer to JSONValue
 * @param f the file to write to
 * 
 * @return 1 on success, 0 on error
 */
DLLEXPORT int print_json_value ( JSONValue_t *p_value , FILE *f );

/** !
 * Evaluate a JSON value
 * 
 * @param p_value pointer to JSONValue
 * @param pp_ret  pointer to return pointer
 * @param type the type of the token to be evaluated 
 * 
 * @return 1 on success, 0 on error
 */
DLLEXPORT int evaluate_json_value (JSONValue_t *p_value, void **pp_ret, enum JSONValueType_e type );

/** ! 
 * Free a JSON value, and its contents
 * 
 * @param p_value pointer to JSONValue
 *  
 * @return void
 */
DLLEXPORT void free_json_value ( JSONValue_t *p_value );