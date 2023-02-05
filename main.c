#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <dict/dict.h>
#include <JSON/JSON.h>

size_t load_file ( const char *path, void *buffer, bool binary_mode );

int main ( int argc, const char* argv[] )
{

    // Initialized data
    dict   *p_json   = 0;
    size_t  file_len = 0;
    char   *file_buf = 0;

    // Check for valid argument

    // Load the file
    file_len = load_file("../example.json", 0, false);
    file_buf = calloc(file_len+1, sizeof(char));
    load_file("../example.json", file_buf, false);





















    // Parse the JSON into a dict
    parse_json(file_buf, file_len, &p_json);

    // Parse the dict 
    {

        // Initialized data
        JSONToken_t *t   = 0;
        char        *dog = 0;

        t = dict_get(p_json, "dog");
        dog = JSON_VALUE(t, JSONobject);

        printf("%s\n",dog);
    }

    return 1;



















    no_argument:return 0;
}

size_t load_file ( const char *path, void *buffer, bool binary_mode )
{

    // Argument checking 
    {
        #ifndef NDEBUG
            if ( path == 0 )
                goto noPath;
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
            noPath:
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