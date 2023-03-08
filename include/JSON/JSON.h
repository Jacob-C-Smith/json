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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <errno.h>
#include <ctype.h>

// Dictionary submodule
#include <dict/dict.h>

// Queue submodule
#include <queue/queue.h>

// Forward declared functions
void free_token ( void *ptr );

// Platform dependent macros
#ifdef _WIN64
#define DLLEXPORT extern __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// This is how you should free dictionaries created with the 'parse_json' function, otherwise you will leak a small amount of memory
#define FREE_JSON_DICT( dict ) dict_free_clear(dict, free_token); dict_destroy(dict);

// Calling this macro evaluates to the value of the JSON property, if the property is not a null pointer and if the type matches the parameter 't'
#define JSON_VALUE( property, t ) (property) ? (property->type==t) ? property->value.n_where : 0 : 0;

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
        queue            *array;
        bool              boolean;
    };
    enum JSONValueType_e type;  // Type
};

// Type definitions
typedef struct JSONValue_s JSONValue_t;
int parse_json_whitespace ( char *pointer, char **return_pointer );
int parse_json_object     ( char *pointer, char **return_pointer);
int parse_json_array      ( char *pointer, char **return_pointer );
int parse_json_value      ( char *text, size_t len, JSONValue_t **pp_value );
void free_value           ( JSONValue_t **pp_value );