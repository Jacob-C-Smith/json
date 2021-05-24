#pragma once	

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Enumeration for the types of JSON objects.
enum JSONValue_e
{
	JSONobject    = 0, // Anything within a "{ ... }".
	JSONarray     = 1, // Any array.
	JSONstring    = 2, // Any string.
	JSONprimative = 3  // Any other value.
};

// This union is used to differentiate pointers form double pointers, 
// or more simply, arrays from non arrays.
union JSONContent_u {
	void*  nWhere;       // Pointer to object.
	void** aWhere;       // Double pointer array of objects.
};
typedef union JSONContent_u JSONContent_t;

// Container for the value
struct JSONValue_s
{
	char*            name;    // The key.
	enum JSONValue_e type;    // The type.
	JSONContent_t    content; // Where the value appears, if type is array, then where is double pointer to text in array.
};
typedef struct JSONValue_s JSONValue_t;

int parseJSON ( char* text, size_t len, size_t count, JSONValue_t* where );   // Parses a JSON file and places tokens at an array of JSONValue_t* tokens. 
                                                                              // If the function is called when the where parameter is nullptr, the
                                                                              // function will return the number of entities that need to be allocated for.
