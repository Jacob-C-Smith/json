#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#include <json/json.h>

int    print_json_file ( const char *path );
size_t load_file       ( const char *path, void *buffer, bool binary_mode );

int main ( int argc, const char* argv[] )
{    
    setlocale(LC_ALL, "");
    print_json_file("test cases/pass/object/object_empty.json");

    // Check for valid argument
    if ( argc == 1 )
        goto no_argument;

    // Iterate over command line arguments
    for (size_t i = 1; i < argc; i++)
    {

        // Output formatting
        printf("--- %s ---\n", argv[i]);

        // Write the contents of the file to stdout
        print_json_file(argv[i]);

        // Output formatting
        putchar('\n');
    }

    // Success
    return EXIT_SUCCESS;

    no_argument:
        printf("Usage: json_example file1.json [file2.json ... fileN.json]\n");
        
        // Error
        return EXIT_FAILURE;
}

int    print_json_file ( const char *path )
{
    
    // Argument checking 
    {
        #ifndef NDEBUG
            if ( path == 0 )
                goto no_path;
        #endif
    }

    // Initialized data
    dict    *p_json         = 0;
    size_t   file_len       = 0,
             property_count = 0;
    char    *file_buf       = 0,
           **keys           = 0;
    void   **values         = 0;

    // Load the file
    {
        file_len = load_file(path, 0, false);
        file_buf = calloc(file_len+1, sizeof(char));
        load_file(path, file_buf, false);
    }

    char *end=0;

    /*
    // Parse the JSON into a dict
    if ( parse_json(file_buf, file_len, &p_json) == 0 )
        goto failed_to_parse_json;

    // Get the number of properties in the dictionary
    property_count = dict_keys(p_json, 0);

    // Allocate memory for keys and values
    keys   = calloc(property_count, sizeof(char *));
    values = calloc(property_count, sizeof(void *));
    
    // Get the keys and values of the properties
    dict_keys(p_json, keys);
    dict_values(p_json, values);

    // Print the contents of the file
    for (size_t i = 0; i < property_count; i++)
    {

        // Initialized data
        JSONToken_t *t = values[i];

        // Write the property key
        printf("%s: ", keys[i]);

        // Write the property value
        if (t->type == JSONarray)
        {

            // Initialzied data
            char   **contents = JSON_VALUE(t, JSONarray);
            size_t   j        = 0;

            // Print all array contents
            for (size_t j = 0; contents[j]; j++)
            {
                printf("\n\t[%llu] : %s", j, contents[j]);
            }

            putchar('\n');
        }
        else
        {
            printf("%s\n", (char *) t->value.n_where);
        }
    }

    // Clean up
    {
        free(values);
        free(keys);
        FREE_JSON_DICT(p_json);
        free(file_buf);
    }
    */
    
    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_path:
                #ifndef NDEBUG
                    printf("[JSON] Null path provided to function \"%s\n", __FUNCTION__);
                #endif

            // Error
            return 0;
        }

        // JSON Errors
        {

            failed_to_parse_json:
                #ifndef NDEBUG
                    printf("[JSON] Failed to parse JSON in call to function \"%s\n", __FUNCTION__);
                #endif

            // Error
            return 0;
        }
    }
}

size_t load_file ( const char *path, void *buffer, bool binary_mode )
{

    // Argument checking 
    {
        #ifndef NDEBUG
            if ( path == 0 )
                goto no_path;
        #endif
    }

    // Initialized data
    size_t  ret = 0;
    FILE   *f   = fopen(path, (binary_mode) ? "rb" : "r");
    
    // Check if file is valid
    if ( f == NULL )
        goto invalid_file;

    // Find file size and prep for read
    fseek(f, 0, SEEK_END);
    ret = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Read to data
    if(buffer)
        ret = fread(buffer, 1, ret, f);

    // The file is no longer needed
    fclose(f);
    
    // Success
    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_path:
                #ifndef NDEBUG
                    printf("[JSON] Null path provided to function \"%s\n", __FUNCTION__);
                #endif

            // Error
            return 0;
        }

        // File errors
        {
            invalid_file:
                #ifndef NDEBUG
                    printf("[Standard library] Failed to load file \"%s\". %s\n",path, strerror(errno));
                #endif

            // Error
            return 0;
        }
    }
}
