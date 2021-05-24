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
    FILE*  f     = fopen("example.json", "rb"); // File
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
        else if (tokens[i].type == JSONstring || tokens[i].type == JSONprimative || tokens[i].type == JSONobject)
        {
            // Print the key/value pair
            printf("Key:    \"%s\" \nValue:  \"%s\"\n\n",tokens[i].name, tokens[i].content.nWhere);
        }
    }

    return 0;
}