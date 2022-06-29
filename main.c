#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dict/dict.h>
#include <JSON/JSON.h>

int main ( int argc, const char* argv[])
{
    // Uninitialized data
    FILE        *f;

    // Initialized data
    size_t       len   = 0,                           // File length
                 count = 0;                           // Number of tokens
    char*        data  = 0;                           // File contents
    dict        *dictionary = 0;

    // Load the file 
    {
        if ( argc > 0 )
            f = fopen(argv[1], "rb");
        else
            goto noFile;
	    
	    // Find file size
    	fseek(f, 0, SEEK_END);
	    len = ftell(f);
	    fseek(f, 0, SEEK_SET);

	    // Allocate data
	    data = calloc(len+1, sizeof(char));

	    // Read the file into data
	    fread(data, 1, len, f);

	    // Close the file handle
	    fclose(f);
    }

    // Parse the JSON data
    parse_json(data,len,&dictionary);

    JSONToken_t  *t      = dict_get(dictionary, "dog");
  
    char        **keys   = calloc(dictionary->n_entries + 1, sizeof(char*));
    JSONToken_t **values = calloc(dictionary->n_entries + 1, sizeof(char*));

    dict_keys(dictionary, keys);
    dict_values(dictionary, values);

    for (size_t i = 0; i < dictionary->n_entries; i++)
    {
        printf("%s : ", keys[i]);
        if (values[i]->type == JSONarray)
        {
            for (size_t j = 0; values[i]->value.a_where[j]; j++)
                printf("\t%s\n", values[i]->value.a_where[j]);
        }
        else
            printf("\t%s\n", values[i]->value.n_where);
    }

    return 0;

    // Error handling
    {
        noFile:
	    printf("No file supplied!\nSpecify a file as an argument\n");
	    return 0;
    }
}