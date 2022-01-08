#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <JSON/JSON.h>

int main ( int argc, const char* argv[])
{
    // Uninitialized data
    JSONToken_t *tokens;                              // JSON tokens
    FILE        *f;

    // Initialized data
    size_t       len   = 0,                           // File length
                 count = 0;                           // Number of tokens
    char*        data  = 0;                           // File contents


    // Load the file 
	{
        if ( argc > 1 )
            f = fopen(argv[1], "r");  // File

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
    count  = parse_json(data, len, 0, (void*)0);
    tokens = calloc(count, sizeof(JSONToken_t));

    // Parse the JSON data
    parse_json(data,len,count,tokens);

    // Dump the contents of the file to stdout
    for (size_t i = 0; i < count; i++)
    {
        // Check for an array
        if(tokens[i].type == JSONarray)
        {
            // Uninitialized data
            size_t j = 0;

            // Print out the key and value formatting
            printf("Key:    \"%s\"\nValues:\n", tokens[i].key);

            // Print out all members of the double pointer
            while (tokens[i].value.a_where[j])
            {
                printf("        \"%s\"\n", (char *)tokens[i].value.a_where[j]);
                j++;
            }
            
            putchar('\n');

            continue;
        }

        // Print the token
        printf("Key:    \"%s\" \nValue:  \"%s\"\n\n",tokens[i].key, (char *)tokens[i].value.n_where);
    }

    return 0;
}