#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#include <json/json.h>

int    print_value ( JSONValue_t *p_value );
int    print_json_file ( const char *path );
size_t load_file       ( const char *path, void *buffer, bool binary_mode );

int main ( int argc, const char* argv[] )
{    
    setlocale(LC_ALL, "");
    print_json_file("example.json");

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
    JSONValue_t  *p_json         = 0;
    dict         *p_object       = 0;
    size_t        file_len       = 0,
                  property_count = 0;
    char         *file_buf       = 0,
                **keys           = 0;
    void        **values         = 0;

    // Load the file
    {
        file_len = load_file(path, 0, false);
        file_buf = calloc(file_len+1, sizeof(char));
        load_file(path, file_buf, false);
    }

    // Parse the JSON into a value
    if ( parse_json_value(file_buf, 0, &p_json) == 0 )
        goto failed_to_parse_json;

    print_value(p_json);

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

int print_value ( JSONValue_t *p_value )
{
    if (p_value == 0)
    {
        printf("null");
    }
    else
    {
        switch (p_value->type)
        {
        case JSONboolean:
            printf("%s",p_value->boolean ? "true" : "false");
            break;
        case JSONinteger:
            printf("%lls", p_value->integer);
            break;
        case JSONfloat:
            printf("%f", p_value->floating);
            break;
        case JSONstring:
            printf("%s", p_value->string);
            break;
        case JSONobject:
            {
                // Initialized data
                size_t        property_count = dict_values(p_value->object, 0);
                char        **keys           = 0;
                JSONValue_t **values         = 0;

                keys   = calloc(property_count, sizeof(char*));
                values = calloc(property_count, sizeof(JSONValue_t*));
                
                dict_keys(p_value->object, keys);
                dict_values(p_value->object, values);

                for (size_t i = 0; i < property_count; i++)
                {
                    printf("\"%s\":",keys[i]);
                    print_value(&values[i]);
                }
            }
            break;
        case JSONarray:
            {
                // Initialized data
                size_t        element_count = 0;
                char         *keys          = 0;
                JSONValue_t **elements      = 0;

                array_get(p_value->list, 0,element_count);

                elements = calloc(element_count, sizeof(JSONValue_t*));
                
                array_get(p_value->object, elements, 0);
                
                for (size_t i = 0; i < element_count; i++)
                {
                    printf("[%i] : ");
                    print_value(elements[i]);
                    printf("\n");
                }
            }
            break;
        
        default:
            break;
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
