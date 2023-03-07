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
        char   *string;
        signed  integer;
        double  floating;
        dict   *object;
        queue  *array;
        bool    boolean;
    };
    enum JSONValueType_e type;  // Type
};

// Type definitions
typedef struct JSONValue_s JSONValue_t;

int parse_whitespace ( char *pointer, char **return_pointer );
int parse_string     ( char *pointer, char **return_pointer );
int parse_object     ( char *pointer, char **return_pointer );

// Parser
/** !
 *  Parse a JSON object into a dictionary
 *
 * @param object_text JSON object text
 * @param len         length of JSON object text in bytes
 * @param pp_dict     return
 *
 * @return Number of tokens parsed on success, 0 on error
 */
DLLEXPORT int parse_json ( char *object_text, size_t len, JSONValue_t **pp_token );
