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
    JSONobject,   // Anything within a "{ ... }"
    JSONarray,    // Any array
    JSONstring,   // Any string
    JSONprimative // Numbers, true, false, null
};

// Structures
struct JSONToken_s
{
    char                *key;   // The key
    union {
        void  *n_where; // Pointer to object
        void **a_where; // Double pointer array of objects
    }                    value;
    enum JSONValueType_e type;  // The type.
};

// Type definitions
typedef struct JSONToken_s JSONToken_t;

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
DLLEXPORT int parse_json ( char *object_text, size_t len, dict   **pp_dict );
