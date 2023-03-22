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

int parse_json_whitespace ( char *pointer, char **return_pointer )
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

int parse_json_string     ( char *pointer, char **return_pointer )
{
    // TODO: FIX ESCAPE SEQUENCES
    // Initialized data
    size_t i = 0,
           j = 1,
           k = 0;

    // Check for correct start
    if ( pointer[i] != '\"' )

        // Fail
        return 0;
    if(pointer[0]==0)
        goto exit;
    while (pointer[j])
    {
        if (pointer[j]=='\"')
        {   
            goto exit;
        }
        else if (pointer[j]=='\\')
        {
            
            switch (pointer[j+1])
            {
                case '\"':
                    pointer[i]='\"';
                    break;
                case '\\':
                    pointer[i]='\\';
                    break;
                case '/':
                    pointer[i]='/';
                    break;
                case 'b':
                    pointer[i]='\b';
                    break;
                case 'f':
                    pointer[i]='\f';
                    break;
                case 'n':
                    pointer[i]='\n';
                    break;
                case 'r':
                    pointer[i]='\r';
                    break;
                case 't':
                    pointer[i]='\t';
                    break;
                case 'u':
                    break;
                default:
                    return 0;
            }
            j++;
        }
        else
        {
            pointer[i]=pointer[j];
        }
        
        i++;
        j++;

    }
    
    exit:

    pointer[i]='\0';

    // Return the new pointer
    *return_pointer = &pointer[j];

    // Success
    return 1;
}

int parse_json_object     ( char *pointer, char **return_pointer, dict **pp_dict )
{
    
    // Initialized data
    size_t  i              = 0,
            property_count = 0;
    char   *last_pointer   = pointer;
    dict   *p_dict         = 0;

    dict_construct(&p_dict, DICT_SIZE+1);

    // Check for correct start
    if ( pointer[i] != '{' )

        // Fail
        return 0;

    i++;

    parse_property:
    // Parse whitespaces
    parse_json_whitespace(pointer, &pointer);

    if ( pointer[i] == '\"' )
    {
        char *key = &pointer[i];
        JSONValue_t *value = 0;

        parse_json_string(&pointer[i], &pointer);
        parse_json_whitespace(pointer, &pointer);

        if ( pointer[i] == ':' )
        {
            i++;
        }

        parse_json_value(&pointer[i],&pointer,&value);
        dict_add(p_dict, key, value);

        if ( pointer[1] == ',' )
        {
            property_count++;
            goto parse_property;
        }
        else if (pointer[1]=='}')
        {
            property_count++;
        }

        i++;
    }
    
    // Check for correct end
    if ( pointer[1] != '}' )

        // Fail
        return 0;

    i++;
    
    *pp_dict = p_dict;

    if(return_pointer)
        *return_pointer = &pointer[1];

    // Success
    return 1;
}

int parse_json_array      ( char *pointer, char **return_pointer, array **pp_array )
{

    // Initialized data
    size_t  i              = 0,
            property_count = 0;
    char   *last_pointer   = pointer;
    array  *p_array        = 0;

    array_construct(&p_array,32);

    // Check for correct start
    if ( pointer[i] != '[' )

        // Fail
        return 0;

    i++;

    parse_property:
    // Parse whitespaces
    parse_json_whitespace(pointer, &pointer);

    if ( pointer[i] != ']' )
    {
        JSONValue_t *value = 0;

        parse_json_value(&pointer[i],&pointer,&value);
        array_add(p_array, value);
        

        if ( pointer[1] == ',' )
        {
            property_count++;
            pointer++;
            goto parse_property;
        }
        else if (pointer[1]==']')
        {
            property_count++;
        }
        else
        {
            return 0;
        }
    }

    *pp_array = p_array;

    if(return_pointer)
        *return_pointer = &pointer[1];

    // Success
    return 1;
}

int parse_json_value      ( char *text, char **return_pointer, JSONValue_t **pp_value )
{

    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    int ret = 0;

    parse_json_whitespace(text, &text);
    switch (text[0])
    {
        case '\"':
            {
                char *last_text = text;
                parse_json_string(text, &text);
                size_t string_len = strlen(last_text);
                p_value->string = calloc(string_len+1, sizeof(char));
                strncpy(p_value->string, last_text, string_len);
                ret = 1;
                p_value->type = JSONstring;
            }
            break;
        case '{':
            {
                char *last_text = text;
                parse_json_object(text, &text, &p_value->object);
                ret = 1;
                p_value->type = JSONobject;
            }
            break;
        case '[':
            {
                char *last_text = text;
                parse_json_array(text, &text, &p_value->list);
                ret = 1;
                p_value->type = JSONarray;
            }
            break;
        case 't':
            if ( strncmp(text, "true" , 4) == 0 )
            {
                p_value->type    = JSONboolean;
                p_value->boolean = true;
                text+=3;
                ret = 1;
            }
            else
            {
                free(p_value);
                p_value = 0;
            }
            break;
        case 'f':
            if ( strncmp(text, "false", 5) == 0 )
            {
                p_value->type = JSONboolean;
                p_value->boolean = false;
                text+=4;
                ret = 1;
            }
            else
            {
                free(p_value);
                p_value = 0;
            }
            break;
        case 'n':
            if ( strncmp(text, "null" , 4) == 0 )
            {
                free(p_value);
                ret = 1;
                text+=3;
                p_value = 0;
            }
            else
            {
                free(p_value);
                p_value = 0;
            }
            break;
        default:
            break;
    }

    // Parse a number
    if ( text[0] == '-' || (text[0] >= '0' && text[0] <= '9') ) 
    {
        // Initialized data
        size_t i = 0,
               d = 0;
        bool   f = false;
        bool   n = false;
        double dret = 0.0;
        signed iret = 0;

        if (text[0] == '-')
        {
            i++;
        }

        // Skip 
        while ( ( text[i] >= '0' && text[i] <= '9' ) || text[i] == '.' ) 
        { 
            if ( text[i] == '.' )
            {
                f = true;
            }
            
            i++, d++;
        };

        errno = 0;

        if (f)
        {
            p_value->type = JSONfloat;
            p_value->floating = strtod(text, NULL);
            if ( (p_value->floating >= DBL_MAX || p_value->floating <= -DBL_MAX) && errno == ERANGE )
            {
                free(p_value);
                return 0;
            }
        }
        else
        {
            p_value->type = JSONinteger;
            p_value->integer = strtoll(text, NULL, 10);
            if ( (p_value->integer == INT64_MIN || p_value->integer == INT64_MAX) && errno == ERANGE )
            {
                free(p_value);
                return 0;
            }
        }

        d--;
        text+=d;
        ret = 1;
    }

    parse_json_whitespace(text, &text);

    if(pp_value)
        *pp_value = p_value;
    if(return_pointer)
        *return_pointer = text;

    return ret;
}

int print_value           ( JSONValue_t  *p_value, FILE *f )
{
    
    if (p_value == 0)
    {
        fprintf(f,"null");
    }
    else
    {
        switch (p_value->type)
        {
        case JSONboolean:
            fprintf(f,"%s",p_value->boolean ? "true" : "false");
            break;
        case JSONinteger:
            fprintf(f,"%lld", p_value->integer);
            break;
        case JSONfloat:
            fprintf(f,"%f", p_value->floating);
            break;
        case JSONstring:
            fprintf(f,"\"%s\"", p_value->string);
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
                fprintf(f,"{");
                for (size_t i = 0; i < property_count-1; i++)
                {
                    fprintf(f,"\"%s\":",keys[i]);
                    print_value(values[i],f);
                    fprintf(f,",");
                }
                fprintf(f,"\"%s\":",keys[property_count-1]);
                print_value(values[property_count-1],f);
                fprintf(f,"}");
            }
            break;
        case JSONarray:
            {
                // Initialized data
                size_t        element_count = 0;
                char         *keys          = 0;
                JSONValue_t **elements      = 0;

                array_get(p_value->list, 0,&element_count);

                elements = calloc(element_count, sizeof(JSONValue_t*));
                
                if ( elements == (void *) 0 )
                    goto no_mem;

                array_get(p_value->list, elements, 0);
                fprintf(f,"[");
                for (size_t i = 0; i < element_count-1; i++)
                {
                    print_value(elements[i],f);
                    fprintf(f,",");
                }

                free(elements);

                print_value(elements[element_count-1],f);
                fprintf(f,"]");
            }
            break;
        
        default:

            // Error
            return 0;
        }
    }

    // Success
    return 1;
    
    // Error handling
    {
        
        // Argument check
        {
            no_mem:
                #ifndef NDEBUG
                    printf("[Standard Library] Failed to allocate memory in call to function \"%s\"\n",__FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}

void free_value           ( JSONValue_t **pp_value )
{
    if ( pp_value == (void *)0 )
        return;

    JSONValue_t *p_value = *pp_value;

    if (p_value->type == JSONobject)
    {
        dict_free_clear(p_value->object, free_value);
    }

    if (p_value->type == JSONarray)
    {
        array_free_clear(p_value->list, free_value);
    }

    free(p_value);

    *pp_value = 0;
}