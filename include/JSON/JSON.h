#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Enumerations

// Enumeration for the types of JSON objects
enum JSONValueType_e
{
    JSONobject    = 0, // Anything within a "{ ... }"
    JSONarray     = 1, // Any array
    JSONstring    = 2, // Any string
    JSONprimative = 3  // Any other value
};

// Unions and structs

// This union is used to differentiate pointers form double pointers, 
// or more simply, arrays from non arrays
union JSONContent_u {
    void  *n_where;       // Pointer to object
    void **a_where;       // Double pointer array of objects
};

// Container for the JSON key/value pair
struct JSONToken_s
{
    char                 *key;   // The key
    union JSONContent_u   value; // The token
    enum JSONValueType_e  type;  // The type.
};

// Type definitions
typedef union  JSONContent_u   JSONContent_t;
typedef enum   JSONValueType_e JSONValueType_t;
typedef struct JSONToken_s     JSONToken_t;

// Function definitions
int parse_json ( char *token_text, size_t len, size_t count, JSONToken_t *tokens ); // ✅ Parses a JSON file and places tokens at an array of JSONToken_t* tokens. 
                                                                                    //     If the function is called when the tokens parameter is nullptr, the
                                                                                    //     function will return the number of tokens that need to be allocated for.