#include <json/json.h>

// Type definitions
typedef union  JSONContent_u   JSONContent_t;
typedef enum   JSONValueType_e JSONValueType_t;

static const char *token_types[] = {
    "object", 
    "array",
    "string",
    "primative"
};

int parse_whitespace ( char *pointer, char **return_pointer )
{

    // Initialized data
    size_t i = 0;

    // Skip past spaces, line feed, carriage return, horizontal tab
    while(pointer[i] == ' ' || pointer[i] == '\n' || pointer[i] == '\r' || pointer[i] == '\t' || pointer[i] == '\0' ) { i++; };

    // Check for null terminator
    if ( pointer[i] == '\0' )

        // Fail
        return 0;

    // Return the new pointer
    *return_pointer = &pointer[i];

    // Success
    return 1;
}

int parse_object     ( char *pointer, char **return_pointer)
{
    
    // Initialized data
    size_t i = 0;

    // Check for correct start
    if ( pointer[i] != '{' )

        // Fail
        return 0;

    i++;

    // Parse whitespaces
    parse_whitespace(pointer, &pointer);

    if ( pointer[i] == '\"' )
    {
        char *key = 0;

        parse_string(pointer, &key);
        parse_whitespace(pointer, &pointer);

        if ( pointer[i] == ':' )
            i++;
        //parse_value()
        if ( pointer[i] == ',' )
            i++;
    }
    
    
    exit:
    
    // Check for correct end
    if ( pointer[i] != '}' )

        // Fail
        return 0;

    i++;

    // Success
    return 1;
}

int parse_string     ( char *pointer, char **return_pointer )
{

    // Initialized data
    size_t i = 0;

    // Check for correct start
    if ( pointer[++i] != '\"' )

        // Fail
        return 0;

    while (pointer[i])
    {

        if (pointer[i]=='\"')
            goto exit;
        else if (pointer[i]=='\\')
        {
            switch (pointer[i+1])
            {
                case '\"':
                    pointer[i]=='\"';
                    break;
                case '\\':
                    pointer[i]=='\\';
                    break;
                case '/':
                    pointer[i]=='/';
                    break;
                case 'b':
                    pointer[i]=='\b';
                    break;
                case 'f':
                    pointer[i]=='\f';
                    break;
                case 'n':
                    pointer[i]=='\n';
                    break;
                case 'r':
                    pointer[i]=='\r';
                    break;
                case 't':
                    pointer[i]=='\t';
                    break;
                case 'u':
                    /* code */
                    break;
                default:
                    return 0;
            }

            i++;
        }

        i++;
    }
    
    exit:
    pointer[i]='\0';

    // Return the new pointer
    *return_pointer = &pointer[1];

    // Success
    return 1;
}

// TODO: Rewrite the JSON parser in assembly with SIMD string instructions.
//       That ought to be a fun challenge.
int parse_json ( char* token_text, size_t len, JSONValue_t **pp_token )
{

    return 1;
}

void free_token (void *ptr)
{
    //if ( ((JSONValue_t *)ptr)->type == JSONarray )
    //    free(((JSONValue_t *)ptr)->value.a_where);
    
    //free(ptr);
}