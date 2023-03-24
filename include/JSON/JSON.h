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

// Dictionary submodule
#include <dict/dict.h>

// Array submodule
#include <array/array.h>

#define DICT_SIZE 0

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

/* 
 * Parse json text into a JSONValue
 * 
 * @param text pointer to JSON text
 * @param return_pointer null or pointer to end of JSON value
 * @param pp_value return
 * 
 * @return 1 on success, 0 on error
 */
int  parse_json_value      ( char *text, char **return_pointer, JSONValue_t **pp_value );

/* 
 * Serialize a JSONValue to a file
 * 
 * @param p_value pointer to JSONValue
 * @param f the file to write to
 * 
 * @return 1 on success, 0 on error
 */
int  print_json_value      ( JSONValue_t *p_value , FILE *f );

/* 
 * Free a JSON value, and its contents
 * 
 * @param p_value pointer to JSONValue
 *  
 * @return void
 */
void free_json_value       ( JSONValue_t *p_value );