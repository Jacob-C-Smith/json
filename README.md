# GXJSON
###### Disclaimer: This is a hobby project. This code may not meet commercial expectations for efficiency, safety, or features. This piece of software is currently incomplete

GXJSON is a low memory footprint JSON parser written for [G10](https://github.com/Jacob-C-Smith/G10)

## Example Code

[example.json](example.json)
```json
{
    "name"      : "Jacob Smith",
    "age"       : 19,
    "height"    : 1.775,
    "interests" : [
        "Computer science",
        "Organic chemistry",
        "Mathematics",
        "Computer games"
    ],
    "dog"       : {
        "name"  : "Eddie",
        "type"  : "Dog",
        "breed" : "Terrier"
    }
}
```
[main.c](main.c)
```c
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/JSON.h"

int main ( int argc, const char* argv[])
{
    // Uninitialized data
    JSONValue_t* tokens;                        // JSON tokens

    // Initialized data
    FILE*  f     = fopen("example.json", "r"); // File
    size_t len   = 0,                           // File length
           count = 0;                           // Number of tokens
    char*  data  = 0;                           // File contents


    // Load the file 
	{

		// Find file size
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fseek(f, 0, SEEK_SET);

		// Allocate data and read file into memory
		data = malloc(len);

		// Read to data
		fread(data, 1, len, f);

		// We no longer need the file
		fclose(f);
	}

    // Figure out how much memory we need to allocate, and allocate it
    count  = parseJSON(data, len, 0, (void*)0);
    tokens = calloc(count, sizeof(JSONValue_t));

    // Parse the JSON data
    parseJSON(data,len,count,tokens);

    // Dump the contents of the file
    for (size_t i = 0; i < count; i++)
    {
        // Check for an array
        if(tokens[i].type == JSONarray)
        {
            // Uninitialized data
            size_t j = 0;

            // Print out the first part of the output
            printf("Key:    \"%s\"\nValues:\n", tokens[i].name);

            // Print out all members of the double pointer
            while (tokens[i].content.aWhere[j])
            {
                printf("        \"%s\"\n", tokens[i].content.aWhere[j]);
                j++;
            }
            
            putchar('\n');

            continue;
        }

        // Print the key/value pair
        printf("Key:    \"%s\" \nValue:  \"%s\"\n\n",tokens[i].name, tokens[i].content.nWhere);
    }

    return 0;
}
```

Terminal output

```
Key:    "name" 
Value:  "Jacob Smith"

Key:    "age"
Value:  "19"

Key:    "height"
Value:  "1.775"

Key:    "interests"
Values:
        "Computer science"
        "Organic chemistry"
        "Mathematics"
        "Computer games"

Key:    "dog"
Value:  "{
        "name"  : "Eddie",
        "type"  : "Dog",
        "breed" : "Terrier"
    }"
```


## Background
I needed a parser for my game engine for loading textures, materials, meshes, and other game assets. I chose JSON as a metadata format because it is human readable, very simple, and, very popular. 

JSON is also advantageous to XML because JSON lacks end tags. This choice was made in an effort to save memory and proceessing time.

## The Problem
I wanted to parse a JSON file into something that I could interface with using C code.

## The Solution, Design choices, and Code Commentary
There are really only three pieces of data that represent a moiety of JSON data. A ```key```, a ```value```, and a ```type```. A struct was defined to contain this information, along with a typedef for convenience

```c
// Container for the value
struct JSONValue_s
{
	char*            name;    // The key.
	enum JSONValue_e type;    // The type.
	JSONContent_t    content; // Where the value appears, if type is array, then where is double pointer to text in array.
};
typedef struct JSONValue_s JSONValue_t;
```
This is a nice, small metadata struct. Only 24 bytes, but probably 32 after structure packing. (12 byte, 16 byte on 32-bit)

With this in mind, I had a good idea of what I needed to do at a high level.

1. Find a key/value pair
2. Extract the pertinent information into a ```JSONValue``` data type
3. Repeat steps 1 and 2 until the parser runs out of data

From this, we can derive the four parameters of the function, and create the signature.

```c
void parseJSON ( char* data, size_t len, size_t count, JSONValue_t* where );
```

Where ```data``` is the pointer to the JSON data, ```len``` is the length, in characters, of ```data```, ```count``` is the number of ```JSONValue```s to parse, and ```where``` is where the ```JSONValue```s are stored. Here lies the first problem. In many situations, you don't know how much memory you need to allocate for a particular token array. To that end, I decided to change the function signature to return an ```int```. 

```c
int parseJSON ( char* data, size_t len, size_t count, JSONValue_t* where );
```

When the funciton is run, it will check ```count```. If ```count``` is zero the parser goes into a "preparsing" mode, where the data is scanned. When the preparsing is complete, it will return the number of ```JSONValue```s that should be allocated for. This means that the function must be run twice, if you don't know how many ```JSONValue```s you are working with. Luckily, preparsing is a relatively fast process. The funciton will behave normally if you call it with a nonzero count. The return value in parsing mode is the number of objects the parser was able to parse. If the C macro ```NDEBUG``` is undefined (i.e. the program is in debug mode), an error message will be printed to stdout if the parameter ```count``` is not equal to the number of parsed objects when the function exits. You can also check it yourself using the return value, if you like.

The following code shows a typical use case for the parser, assuming data is already defined and holds some JSON data.
```c
    ...
    size_t       len   = strlen(data);                       // Find the length of the data ...
    int          count = parseJSON(data, len, 0, (void*)0);  // ... and use it to preparse the data.
    JSONValue_t* where = calloc(count, sizeof(JSONValue_t)); // Use the return from the preparse to allocate some JSONValues
    int          check = parseJSON(data, len, count, where); // Then actually parse the data ...
    ...
    free(where);                                             // ... and make sure to free the data so you don't cause a memory leak.
    ...
```

Parsing every single key / value pair in a JSON object can be a waste of CPU time, depending on your use case. This could be a lot of unneccisary overhead, both in terms of memory, and computing time. For a while I thought of how I could reduce the overhead. I had two flashes of insignt. 

The first was that you don't HAVE to parse the entire thing in one go. You really only have to parse the "top level" values. For instance, in example.json, ```"dog"``` is a top level value, but ```"breed"``` is not. When an object is parsed as a ```JSONValue```, the value part of the struct points to the entire text of the object.
```json
{
    "name"  : "Eddie",
    "type"  : "Dog",
    "breed" : "Terrier"
}
```
The value part can then be parsed again, as if it were the original data. The values that were once one level under top level, are now top level. The function generates data that it can use again in some other separate instance. This can save quite a bit of memory and processing power depending on your application. Also, you can parse your data insofar as you like, as oppose to having to parse it all in one go... But we can do better.

The second flash of insignt was that there are a lot of commas and quotation marks in JSON that arent important for the computer to know about. They are, however, very conveniently placed to act as [null terminators](https://en.wikipedia.org/wiki/Null-terminated_string). I realized that while populating the ```char*```s in a particular ```JSONValue```, inserting null terminators in precisely the right spot in data could create valid, null terminated strings within the JSON data. The good part is that you won't end up using much more memory than the file occupies. Only 24 bytes per parsed object, and half as much if you are on 32-bit for whatever reason. There is also no memory allocating and string copying to exfiltrate the keys and values like in the old version. The bad part is that the original JSON data is kinda mangled by the insertion of the null pointers. This is a sacrifice that I am willing to make. **If you are going to need the original JSON file after parsing it, either make a copy of your data or don't use GXJSON**.

NOTE: *Preparsing the data will not insert null terminators, and will not mangle the file*.

## The Implementation
Please see [JSON.c](JSON.c)

## Types
```c
// Enumeration for the types of JSON objects.
enum JSONValue_e
{
	JSONobject    = 0,  
	JSONarray     = 1,
	JSONstring    = 2,
	JSONprimative = 3 
};
```
```JSONobject``` is anything within a { ... }.

```JSONarray```  is anything within a [ ... ].

```JSONstring``` is anything within a " "

```JSONprimative``` is any number, true, false, or null.

```c
// This union is used to differentiate pointers form double pointers, or more simply, arrays from non arrays.
union JSONContent_u {
	void*  nWhere;
	void** aWhere;
};
typedef union JSONContent_u JSONContent_t;
```

```nWhere``` is a normal pointer to the text of a value

```aWhere``` is a double pointer to an array of values

```c
// Container for the value
struct JSONValue_s
{
	char*            name;    
	enum JSONValue_e type;    
	JSONContent_t    content;
};
typedef struct JSONValue_s JSONValue_t;
```

```name``` is the key in a key / value pair

```type``` is the type of the value

```content``` is a union which, depending on how it is accessed, can be either a regular pointer, or a double pointer.
