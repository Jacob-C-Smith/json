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

// This is how you should free values created with the 'parse_json_value' function, otherwise you will leak memory
#define FREE_VALUE( dict ) dict_free_clear(dict, free_token); dict_destroy(dict);

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

int  parse_json_value      ( char         *text    , char **return_pointer, JSONValue_t **pp_value );
int  print_json_value      ( JSONValue_t  *p_value , FILE *f );
void free_json_value       ( JSONValue_t  *p_value );