# Version 3 is still in development. This notice will be removed when version 3 releases are posted. 

# JSON
 A JSON object parser written in C. 
 
 > 1 [Download](#download)
 >
 > 2 [Build](#build)
 >
 > 3 [Example](#example)
 >
 >> 3.1 [Example input](#example-input)
 >>
 >> 3.2 [Example output](#example-output)
 >
 > 4 [Limitations](#limitations)
 >
 > 5 [Definitions](#definitions)
 >
 >> 5.1 [Type definitions](#type-definitions)
 >>
 >> 5.2 [Function definitions](#function-definitions)
 >>
 >> 5.3 [Macro definitions](#macro-definitinos)

 ## Download
 To download JSON, execute the following command
 ```bash
 $ git clone https://github.com/Jacob-C-Smith/JSON --recurse-submodules
 ```
 ## Build
 To build on UNIX like machines, execute the following commands in the same directory
 ```bash
 $ cd JSON
 $ cmake .
 $ make
 ```
  This will build the example program, and dynamic / shared libraries

  To build JSON for Windows machines, open the base directory in Visual Studio, and build your desired target(s)
 ## Example
 To run the example program, execute this command
 ```
 ./json_example file1.json [file2.json ... fileN.json]
 ```
 ### Example input 
 ```json
{
    "name"      : "Jacob Smith",
    "age"       : 20,
    "height"    : 1.775,
    "dog"       : {
        "name"  : "Eddie",
        "sex"   : "Male",
        "breed" : "Terrier"
    },
    "interests" : [
        "Computer science",
        "3D modeling",
        "Organic chemistry",
        "Mathematics",
        "Computer games",
        "Epistemology"
    ]
}
 ```
[example.json](example.json)

 ### Example output
 ```
--- example.json ---
name: Jacob Smith
age: 19
height: 1.775
dog: {
        "name"  : "Eddie",
        "sex"   : "Male",
        "breed" : "Terrier"
    }
interests: 
        [0] : Computer science
        [1] : 3D modeling
        [2] : Organic chemistry
        [3] : Mathematics
        [4] : Computer games
        [5] : Epistemology

 ```
 [Source](main.c)
 
 ## Limitations
 This parser can not parse arrays of arrays just yet. I'm still working on that one. 
 
 ## Definitions
 
 ### Type definitions
 ```c
 typedef enum   JSONValueType_e JSONValueType_t;
 typedef struct JSONToken_s     JSONToken_t;
 ```
 ### Function definitions

 ```c 
 // Parser 
 int parse_json ( char* object_text, size_t len, dict **pp_dict )
 ```

 ### Macro definitions
 ```c

// Evaluates to the value of the JSON property, if the property is not a null pointer and if the type matches the parameter 't'
 #define JSON_VALUE     ( token, t )

 // Use this macro to free dictionaries. Using dict_destroy will cause a memory leak. 
 #define FREE_JSON_DICT ( dict ) 
 ```
