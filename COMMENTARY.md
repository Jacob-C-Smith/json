# Commentary

This file documents the improvements I make to this project over time. Major changes break compatability. Minor changes do not. 

> 1 [v1.0](#v10)
>
> 2 [v2.0](#v20)

# v1.0
## Problem solving process, investigation, and comentary 
### > Background
I needed a JSON parser for my game engine. 

### > The problem
I wanted to parse a JSON file into something that I could interface with using C code.

### > The solution, design choices, and code commentary
There are really only three pieces of data that constitute a JSON token. A ```key```, a ```value```, and a ```type```. A struct was defined to contain this information, along with a typedef for convenience

```c
// Container for the value
struct JSONValue_s
{
    char                 *key;   // The key
    union JSONContent_u   value; // The token
    enum  JSONValueType_e type;  // The type.
};
typedef struct JSONValue_s JSONValue_t;
```
This is a nice, small metadata struct. Only 24 bytes, but probably 32 after structure packing. (12 byte, 16 byte on 32-bit)

With this data structure in mind, I had a good idea of what I needed to do at a high level.

1. Find a key/value pair
2. Extract the pertinent information into a ```JSONValue``` data type
3. Repeat steps 1 and 2 until the parser runs out of data to parse

From this, we can derive the four parameters of the function, and create the signature.

```c
void parse_json ( char *token_text, size_t len, size_t count, JSONToken_t *tokens );
```

Where ```token_text``` is the pointer to the JSON data, ```len``` is the length, in characters, of ```token_text```, ```count``` is the number of ```JSONValue```s to parse, and ```tokens``` is where the ```JSONValue```s are stored. Here lies the first problem. In many situations, you don't know how much memory you need to allocate for a particular token array. To that end, I decided to change the function signature to return an ```int```. 

```c
int parse_json ( char *token_text, size_t len, size_t count, JSONToken_t *tokens );
```

When the funciton is run, it will check ```count```. If ```count``` is zero the parser goes into a "preparsing" mode, where the data is scanned. When the preparsing is complete, it will return the number of ```JSONValue```s that should be allocated for. This means that the function must be run twice, if you don't know how many ```JSONValue```s you are working with. Luckily, preparsing is a relatively fast process. The funciton will behave normally if you call it with a nonzero count. The return value in parsing mode is the number of objects the parser was able to parse. If the C macro ```NDEBUG``` is undefined (i.e. the program is in debug mode), an error message will be printed to stdout if the parameter ```count``` is not equal to the number of parsed objects when the function exits. You can also check it yourself using the return value, if you like.

The following code shows a typical use case for the parser, assuming data is already defined and holds some JSON data.
```c
    ...
    size_t       len   = strlen(data);                         // Find the length of the data ...
    int          count = parse_json(data, len, 0, (void*)0);   // ... and use it to preparse the data.
    JSONValue_t* tokens = calloc(count, sizeof(JSONValue_t));  // Use the return from the preparse to allocate some JSONValues
    int          check = parse_json(data, len, count, tokens); // Then actually parse the data ...
    ...
    free(tokens);                                              // ... and make sure to free the data so you don't cause a memory leak.
    ...
```

Parsing every single key / value pair in a JSON object can be a waste of CPU time, depending on your use case. This could be a lot of unneccisary overhead, both in terms of memory, and computing time. For a while I thought of how I could reduce the overhead. I had two flashes of insignt. 

The first was that you don't have to parse the entire thing in one go. You really only have to parse the "top level" tokens. For instance, in example.json, ```"dog"``` is a top level token, but ```"breed"``` is not. When an object is parsed as a ```JSONValue```, the value part of the struct points to the entire text of the object.
```json
{
    "name"  : "Eddie",
    "type"  : "Dog",
    "breed" : "Terrier"
}
```
The token can then be parsed in the same way its parent token was. The tokens that were once one level under top level, are now top level. The function generates data that it can use again in some other separate instance. This can save quite a bit of memory and processing power depending on your application. Also, you can parse your data insofar as you like, as oppose to having to parse it all in one go... But we can do better.

The second flash of insignt was that there are a lot of commas and quotation marks in JSON that arent important for the computer to know about. They are, however, very conveniently placed to be overwritten by [null terminators](https://en.wikipedia.org/wiki/Null-terminated_string). I realized that while populating the ```char *```'s in a particular ```JSONValue```, inserting null terminators in precisely the right spot in data could create valid, null terminated strings within the JSON data. The good part is that you won't end up using much more memory than the file occupies. Only 24 bytes per parsed object, and half as much if you are on 32-bit for whatever reason. There is also no memory allocating and string copying and allocating a bunch of memory to exfiltrate the tokens like in the old version. The bad part is that the original JSON data is kinda mangled by the insertion of the null pointers. This is a sacrifice that I am willing to make. **If you are going to need the original JSON file after parsing it, make a copy of your data**.

NOTE: *Preparsing the data will not insert null terminators, and will not mangle the memory*.

### > The implementation
Please see [JSON.c](JSON.c)

### > Types
```c
// Enumeration for the types of JSON objects
enum JSONValueType_e
{
    JSONobject    = 0, // Anything within a "{ ... }"
    JSONarray     = 1, // Any array
    JSONstring    = 2, // Any string
    JSONprimative = 3  // Any other value
};
```
```JSONobject``` is anything within a { ... }.

```JSONarray```  is anything within a [ ... ].

```JSONstring``` is anything within a " "

```JSONprimative``` is any number, true, false, or null.

```c
// This union is used to differentiate pointers form double pointers, 
// or more simply, arrays from non arrays
union JSONContent_u {
    void  *n_where;       // Pointer to object
    void **a_where;       // Double pointer array of objects
};
```

```n_where``` is a normal pointer to the text of a value

```a_where``` is a double pointer to an array of values

```c
// Container for the value
struct JSONToken_s
{
    char                 *key;   // The key
    union JSONContent_u   value; // The token
    enum JSONValueType_e  type;  // The type.
};
```

```key``` is the key in a key / value pair

```type``` is the type of the value

```value``` is a union which can be either a regular pointer, or a double pointer.

# v2.0
### > Background
The original JSON parser worked fine, but I did have some issues with it. Searching for the right item is slow, and would often result in ugly code.

From [G10](https://github.com/Jacob-C-Smith/G10/blob/main/G10.c)
```c
// Load the file
g_load_file(path, token_text, false);

token_text_len = strlen(token_text);

// Preparse the JSON
token_count = parse_json(token_text, token_text_len, 0, 0);
tokens      = calloc(token_count, sizeof(JSONToken_t));
parse_json(token_text, token_text_len, token_count, tokens);

// Parse the JSON
for (i = 0; i < token_count; i++)
{
    if (strcmp(tokens[i].key, "name")                 == 0)
    ...
    if (strcmp(tokens[i].key, "window width")         == 0)
    ...
    if (strcmp(tokens[i].key, "window height")        == 0)
    ...
    if (strcmp(tokens[i].key, "window title")         == 0)
    ...
    if (strcmp(tokens[i].key, "fullscreen")           == 0)
    ...
    if (strcmp(tokens[i].key, "input")                == 0)
    ...
    if (strcmp(tokens[i].key, "initial scene")        == 0)
    ...
    if (strcmp(tokens[i].key, "cache part count")     == 0)
    ...
    if (strcmp(tokens[i].key, "cache material count") == 0)
    ...
    if (strcmp(tokens[i].key, "cache shader count")   == 0)
    ...
}

free(tokens);
free(token_text);
...
```
This was fine when my game engine was simple, and I only had to maintain a few loader functions. This is no longer the case. Hash tables present a much faster solution.

### > The solution, design choices, and code commentary

This was straightforward. I changed the function signature to write to a dictionary. 
```c
DLLEXPORT int parse_json ( char* token_text, size_t len, dict** dictionary );
```
After counting JSON tokens, a hash table is constructed. I made the arbitrary choice to make the hash table twice as big as the property count. This was done in an effort to minimize collisions. 
```c
    // Initialized data.
    ...
    dict  *i_dictionary   = 0;
    ...
    dict_construct(dictionary, count*2);
    i_dictionary = *dictionary;
```
The parser allocates a new token after every iteration. After the token is populated, it is added to the dictionary.
```c
// Populate key, value, and type in token
...

token_iterator++;

dict_add(i_dictionary, token->key, token);

// Iterate or exit
...
```
And in the exit branch, the dictionary is written to the double pointer parameter. It was really that simple. 

### > The implementation
Please see [JSON.c](JSON.c)
