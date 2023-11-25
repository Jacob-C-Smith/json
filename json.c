#include <json/json.h>

// Constants
const size_t DICT_SIZE = 16;

int parse_json_whitespace ( const char *pointer, const char **const return_pointer )
{

    // Argument check
    if ( pointer  == (void *) 0 ) goto no_pointer;
    if ( *pointer ==       '\0' ) goto done;

    // Skip past spaces, line feed, carriage return, horizontal tab
    while (
        *pointer == ' '  ||
        *pointer == '\n' ||
        *pointer == '\r' ||
        *pointer == '\t' ||
        *pointer == '\0' 
    )
    { pointer++; };

    // Error checking
    if ( *pointer == '\0' ) return 0;
    
    done:
    // Return the new pointer
    *return_pointer = pointer;

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_pointer:
                #ifndef NDEBUG
                    printf("[JSON] Null pointer provided for parameter \"pointer\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;

        }
    }
}

int parse_json_string ( char *const pointer, const char **const return_pointer )
{

    // Argument check
    if ( pointer == (void *) 0 ) goto no_pointer;

    // Initialized data
    size_t i = 0,
           j = 1,
           k = 0;

    // Error check
    if ( pointer[i] != '\"' ) return 0;
    
    // Check for an empty string
    if ( pointer[0] == 0 ) goto exit;

    // Walk the string
    while ( pointer[j] )
    {

        // Exit branch
        if ( pointer[j] == '\"' ) goto exit;
        
        // Escape sequence
        else if ( pointer[j] == '\\' )
        {

            // Process the escape sequence            
            switch ( pointer[j+1] )
            {

                // Single quote
                case '\"':

                    // " 
                    pointer[i] = '\"';

                    // Done
                    break;

                // Backslash
                case '\\':

                    // (backslash)
                    pointer[i] = '\\';

                    // Done
                    break;

                // Forward slash
                case '/':

                    // / 
                    pointer[i] = '/';

                    // Done
                    break;

                // Backspace
                case 'b':

                    // \b
                    pointer[i] = '\b';

                    // Done
                    break;

                // Form feed
                case 'f':

                    // \f 
                    pointer[i] = '\f';

                    // Done 
                    break;

                // Line feed
                case 'n':

                    // \n
                    pointer[i] = '\n';

                    // Done
                    break;
                
                // Carriage return
                case 'r':

                    // \r
                    pointer[i] = '\r';

                    break;

                // Horizontal tab
                case 't':

                    // \t
                    pointer[i]='\t';

                    // Done
                    break;
                
                // TODO: Unicode
                case 'u':

                    // TODO:

                    // Done
                    break;

                // Bad string
                default:

                    // Error
                    return 0;
            }
            
            // Increment the cursor
            j++;
        }

        // Copy the text
        else
            pointer[i] = pointer[j];
        
        // Increment the cursors
        i++, j++;
    }
    
    exit:

    // Insert a null terminator
    pointer[i] = '\0';

    // Return the updated pointer
    *return_pointer = &pointer[j];

    // Success
    return 1;
    
    // Error handling
    {

        // Argument errors
        {
            no_pointer:
                #ifndef NDEBUG
                    printf("[JSON] Null pointer provided for parameter \"pointer\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;

        }
    }
}

int parse_json_object ( char *pointer, const char **const return_pointer, dict **const pp_dict )
{
    
    // Initialized data
    size_t  property_count = 0;
    char   *last_pointer   = pointer;
    dict   *p_dict         = 0;

    // Construct a dict
    if ( dict_construct(&p_dict, DICT_SIZE, 0) == 0 ) goto failed_to_construct_dict;

    // Error checking
    if ( *pointer != '{' ) return 0;

    // Increment the cursor
    pointer++;

    parse_property:

    // Parse whitespaces
    parse_json_whitespace(pointer, &pointer);

    // Parse the property key
    if ( *pointer == '\"' )
    {

        // Initialized data
        char       *key   = pointer;
        json_value *value = 0;

        // Parse the property key
        parse_json_string(pointer, &pointer);

        // Increment the cursor
        pointer++;

        // Parse any whitespace
        parse_json_whitespace(pointer, &pointer);

        // Check for a valid property
        if ( *pointer == ':' )
            pointer++;

        // Invalid property
        else
            goto expected_value;

        // Parse a JSON value
        if ( parse_json_value(pointer, &pointer, &value) == 0 ) goto failed_to_parse_json_value;

        // Add the value to the dictionary
        dict_add(p_dict, key, value);

        // Search for the next property
        if ( *pointer == ',' )
        {

            // Increment the property counter and the pointer 
            property_count++, pointer++;

            // Parse another property
            goto parse_property;
        }
        // ... or the end of the object
        else if ( *pointer == '}' )

            // Increment the property counter for the last time
            property_count++;
    }
    
    // Error checking
    if ( *pointer != '}' ) return 0;

    // Increment the cursor
    pointer++;

    // Return the cursor to the caller
    if ( return_pointer ) *return_pointer = pointer;

    // Return a pointer to the caller
    *pp_dict = p_dict;

    // Success
    return 1;

    expected_value:
    failed_to_parse_json_value:

        return 0;

    // Error check
    {
        
        // dict errors
        {
            failed_to_construct_dict:
                #ifndef NDEBUG
                    printf("[dict] Failed to construct dict in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}

int parse_json_array ( char *pointer, const char **const return_pointer, const array **const pp_array )
{

    // Initialized data
    size_t  i              = 0,
            property_count = 0;
    char   *last_pointer   = pointer;
    array  *p_array        = 0;

    // Construct an array
    if ( array_construct(&p_array, DICT_SIZE) == 0 ) goto failed_to_construct_array;

    // Error checking
    if ( pointer[0] != '[' ) return 0;

    // Increment the cursor
    pointer++;

    parse_property:
    
    // Parse whitespaces
    parse_json_whitespace(pointer, &pointer);

    // Walk the text
    if ( pointer[0] != ']' )
    {

        // Initialized data
        json_value *value = 0;

        // Parse the text into a value
        parse_json_value(&pointer[i], &pointer, &value);

        // Add the value to the array
        array_add(p_array, value);

        // Check for another property
        if ( *pointer == ',' )
        {

            // Increment the property counter and the cursor
            property_count++, pointer++;

            // Parse another property
            goto parse_property;
        }
        // Default
        else if ( *pointer == ']' )

            // Increment the property counter one last time
            property_count++;
        else

            // Error
            return 0;
    }
    
    // Error checking
    if ( *pointer != ']' ) return 0;
        
    // Increment the cursor
    pointer++;

    // Return the cursor to the caller
    if ( return_pointer ) *return_pointer = pointer;

    // Return a pointer to the caller
    *pp_array = p_array;

    // Success
    return 1;

    failed_to_construct_array:

        // Error
        return 0;
}

int parse_json_value ( char *text, const char **const return_pointer, const json_value **const pp_value )
{

    // Argument check
    if ( text     == (void *) 0 ) goto no_text;
    if ( pp_value == (void *) 0 ) goto no_value;

    // Initialized data
    json_value *p_value = JSON_REALLOC(0, sizeof(json_value));
    int         ret     = 0;

    // Parse whitespace
    (void) parse_json_whitespace(text, &text);

    // Parse a value
    switch ( *text )
    {

        // This branch parses a string
        case '\"':
            {

                // Initialized data
                char *last_text = text;
                size_t string_len = 0;
                
                // Parse the string
                if ( parse_json_string(text, &text) == 0 ) goto failed_to_parse_json_string;
                
                // Increment the cursor
                text++;

                // Copy the string into the JSON value
                {

                    // Compute the length of the string
                    string_len = strlen(last_text);           

                    // Allocate memory for the string
                    p_value->string = JSON_REALLOC(0, (string_len+1) * sizeof(char));
                
                    // Error check
                    if ( p_value->string == (void *) 0 ) goto no_mem;

                    // Copy the string 
                    (void) strncpy(p_value->string, last_text, string_len+1);
                }

                // Return okay
                ret = 1;
                
                // Set the return type
                p_value->type = JSON_VALUE_STRING;
            }
            break;
        
        // This branch parses an object
        case '{':
            {
                
                // Initialized data
                char *last_text = text;

                // Parse the JSON text as an object
                if ( parse_json_object(text, &text, &p_value->object) == 0 ) goto failed_to_parse_json_as_object;

                // Return okay
                ret = 1;

                // Set the value type
                p_value->type = JSON_VALUE_OBJECT;
            }
            break;
        
        // This branch parses an array
        case '[':
            {

                // Initialized data
                char *last_text = text;

                // Parse the JSON text as an array
                if ( parse_json_array(text, &text, &p_value->list) == 0 ) goto failed_to_parse_json_as_array;

                // Return okay 
                ret = 1;

                // Set the value type
                p_value->type = JSON_VALUE_ARRAY;
            }
            break;

        // This branch parses the 'true' keyword
        case 't':

            // Check for the correct keyworkd
            if ( strncmp(text, "true" , 4) == 0 )
            {

                // Store a true value
                *p_value = (json_value)
                {
                    .type = JSON_VALUE_BOOLEAN,
                    .boolean = true
                };

                // Skip the cursor
                text += 4;

                // Return okay
                ret = 1;
            }
            // Default
            else
            {

                // Free the JSON value
                JSON_REALLOC(p_value, 0);

                // p_value = nullptr
                p_value = 0;

                // Return error
                ret = 0;
            }
            break;
        
        // This branch parses the 'false' keyword
        case 'f':
            
            // Check for the 'false' keyword
            if ( strncmp(text, "false", 5) == 0 )
            {

                // Store a true value
                *p_value = (json_value)
                {
                    .type = JSON_VALUE_BOOLEAN,
                    .boolean = false
                };

                // Skip the cursor
                text+=5;

                // Return okay
                ret = 1;
            }
            // Default
            else
            {

                // Free the JSON value
                JSON_REALLOC(p_value, 0);

                // p_value = nullptr
                p_value = 0;

                // Return error
                ret = 0;
            }
            break;
        
        // This branch parses the 'null' keyword
        case 'n':
            // Check for the 'null' keyword
            if ( strncmp(text, "null" , 4) == 0 )
            {

                // Free the JSON value
                JSON_REALLOC(p_value, 0);

                // Return okay
                ret = 1;
                
                // Skip the cursor
                text += 4;

                // p_value = nullptr
                p_value = 0;
            }

            // Default
            else
            {

                // Free p_value
                JSON_REALLOC(p_value, 0);

                // Return error
                p_value = 0;
            }
            break;
        default:
            break;
    }

    // Parse a number
    if ( *text == '-' || (*text >= '0' && *text <= '9') ) 
    {
        // Initialized data
        size_t i = 0,
               d = 0;
        bool   f = false;
        bool   n = false;
        double dret = 0.0;
        signed iret = 0;
        
        // Check the number's sign
        if ( text[0] == '-' )

            // Set the negative bit, and skip past the sign
            i++, n = true;

        // Skip digits and decimal places
        while ( ( text[i] >= '0' && text[i] <= '9' ) || text[i] == '.' ) 
        { 

            // Check for a float
            f |= ( text[i] == '.' );
            
            // Increment
            i++, d++;
        };

        // Clear errors
        errno = 0;

        // Parse a floating point
        if ( f )
        {

            // Store a number value
            *p_value = (json_value)
            {
                .type = JSON_VALUE_NUMBER,
                .number = strtod(text, NULL)
            };

            // Bounds check
            if ( (p_value->number >= DBL_MAX || p_value->number <= -DBL_MAX) && errno == ERANGE )
            {
                
                // Free the JSON value
                JSON_REALLOC(p_value, 0);

                // Error
                return 0;
            }
        }

        // Parse an integer
        else
        {

            // Set the type of the value
            *p_value = (json_value)
            {
                .type = JSON_VALUE_INTEGER,
                .integer = strtoll(text, NULL, 10)
            };

            // Bounds check
            if ( (p_value->integer <= -9223372036854775807LL || p_value->integer >= 9223372036854775807LL) && errno == ERANGE )
            {

                // Free the value
                JSON_REALLOC(p_value, 0);

                // Error
                return 0;
            }
        }

        // Decrement the decimal places
        d--;

        // Skip N decimal places
        text+=d; 

        // Increment the cursor
        text++;

        // Increment the cursor again if the number has a - sign
        text += n ? 1 : 0;

        // Return okay
        ret = 1;
    }

    // Parse whitespace
    (void) parse_json_whitespace(text, &text);

    // Write the return value
    if ( pp_value ) *pp_value = p_value;

    // Update the cursor
    if ( return_pointer ) *return_pointer = text;

    // Success
    return ret;

    failed_to_parse_json_string:
    failed_to_parse_json_as_array:
    failed_to_parse_json_as_object:
        return 0;

    // Error handling
    {

        // Argument errors
        {
            no_text:
                #ifndef NDEBUG
                    printf("[JSON] Null pointer provided for parameter \"text\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;

            no_value:
                #ifndef NDEBUG
                    printf("[JSON] Null pointer provided for parameter \"pp_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // Standard library errors
        {
            no_mem:
                #ifndef NDEBUG
					printf("[Standard Library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
				#endif

				// Error
				return 0;

        }
    }
}

int serialize_json_value ( const json_value *const p_value, char *_buffer )
{

    size_t written_characters = 0;

    // Null case
    if ( p_value == 0 ) written_characters += sprintf(&_buffer[written_characters], "null");

    // Everything else
    else
    {

        // Print the value
        switch (p_value->type)
        {
            
            // Print a boolean value
            case JSON_VALUE_BOOLEAN:
                written_characters += sprintf(&_buffer[written_characters],"%s",p_value->boolean ? "true" : "false");
                break;
            
            // Print an integer value
            case JSON_VALUE_INTEGER:
                written_characters += sprintf(&_buffer[written_characters],"%lld", p_value->integer);
                break;

            // Print a floating point value
            case JSON_VALUE_NUMBER:
                written_characters += sprintf(&_buffer[written_characters],"%g", p_value->number);
                break;


            // Print a string
            case JSON_VALUE_STRING:
            {

                // Initialized data
                size_t len = strlen(p_value->string);

                // Formatting
                written_characters += sprintf(&_buffer[written_characters], "\"");
                
                // Iterate over each character
                for (size_t i = 0; i < len; i++)
                {

                    // Check for an escape sequence
                    switch(p_value->string[i])
                    {

                        // Double quote
                        case '\"':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = '\"';

                            break;

                        // Backslash
                        case '\\':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = '\\';

                            break;

                        // Forward slash
                        case '/':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = '/';

                            break;
                        
                        // Backspace
                        case '\b':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = 'b';

                            break;
                        
                        // Form feed
                        case '\f':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = 'f';

                            break;
                        
                        // Line feed
                        case '\n':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = 'n';

                            break;
                        
                        // Carriage return
                        case '\r':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = 'r';

                            break;
                        
                        // Horizontal tab
                        case '\t':
                            _buffer[written_characters++] = '\\';
                            _buffer[written_characters++] = 't';

                            break;

                        // TODO: Unicode
                        // TODO:case '\u':
                            // TODO:
                            // TODO:break;
                        
                        // Default
                        default:
                            _buffer[written_characters++] = p_value->string[i];
                    }
                }
            
                // Formatting
                written_characters += sprintf(&_buffer[written_characters], "\"");
                break;
            }
            
            // Print an object
            case JSON_VALUE_OBJECT:
            {
                if ( p_value->object )
                {
                    // Initialized data
                    size_t        property_count = dict_values(p_value->object, 0);
                    char        **keys           = 0;
                    json_value **values          = 0;

                    written_characters += sprintf(&_buffer[written_characters],"{");

                    if ( property_count == 0 )
                        goto done;

                    keys   = JSON_REALLOC(0, property_count * sizeof(char*));
                    values = JSON_REALLOC(0, property_count * sizeof(json_value*));

                    dict_keys(p_value->object, keys);
                    dict_values(p_value->object, (void **)values);
                    for (size_t i = 0; i < property_count-1; i++)
                    {
                        written_characters += sprintf(&_buffer[written_characters],"\"%s\":",keys[i]);
                        serialize_json_value(values[i],_buffer);
                        written_characters += sprintf(&_buffer[written_characters],",");
                    }
                    written_characters += sprintf(&_buffer[written_characters],"\"%s\":",keys[property_count-1]);
                    serialize_json_value(values[property_count-1], _buffer);

                    JSON_REALLOC(keys, 0);
                    JSON_REALLOC(values, 0);
                    done:
                    written_characters += sprintf(&_buffer[written_characters],"}");
                }
                break;
            }
            
            // Print an array
            case JSON_VALUE_ARRAY:
            {
                
                // Initialized data
                size_t       element_count = 0;
                char        *keys          = 0;
                json_value **elements      = 0;

                // Error check
                if ( p_value->list == (void *) 0 )
                    goto no_list;

                // Get the contents of the array
                {

                    // Get the quantity of elements                    
                    array_get(p_value->list, 0,&element_count);

                    // Allocate memory for the elements
                    elements = JSON_REALLOC(0, element_count * sizeof(json_value*));

                    // Error check
                    if ( elements == (void *) 0 )
                        goto no_mem;

                    // Get the contents of the array
                    array_get(p_value->list, (void **)elements, 0);
                }
                
                // Formatting
                written_characters += sprintf(&_buffer[written_characters],"[");

                // Print the first element
                if ( element_count )
                    serialize_json_value(elements[0], _buffer);
                
                // Iterate over each element
                for (size_t i = 1; i < element_count; i++)
                {

                    // Formatting
                    written_characters += sprintf(&_buffer[written_characters], ",");

                    // Print the value
                    serialize_json_value(elements[i], _buffer);
                }

                // Free the element
                JSON_REALLOC(elements, 0);


                // Formatting
                written_characters += sprintf(&_buffer[written_characters], "]");
            }
                break;
            
            default:

                // Error
                return 0;
            }
    }

    // Success
    return 1;

    no_list:
    no_mem:
        return 0;
}

int print_json_value ( const json_value *const p_value, FILE *f )
{
    
    // Null case
    if ( p_value == 0 ) fprintf(f,"null");

    // Everything else
    else
    {

        // Print the value
        switch (p_value->type)
        {
            // Print a boolean value
            case JSON_VALUE_BOOLEAN:
                fprintf(f,"%s",p_value->boolean ? "true" : "false");
                break;
            
            // Print an integer value
            case JSON_VALUE_INTEGER:
                fprintf(f,"%lld", p_value->integer);
                break;

            // Print a floating point value
            case JSON_VALUE_NUMBER:
                fprintf(f,"%g", p_value->number);
                break;


            // Print a string
            case JSON_VALUE_STRING:
            {

                // Initialized data
                size_t len = strlen(p_value->string);

                // Formatting
                fprintf(f, "\"");
                
                // Iterate over each character
                for (size_t i = 0; i < len; i++)
                {

                    // Check for an escape sequence
                    switch(p_value->string[i])
                    {

                        // Double quote
                        case '\"':
                            putc('\\', f);
                            putc('\"', f);

                            break;

                        // Backslash
                        case '\\':
                            putc('\\', f);
                            putc('\\', f);

                            break;

                        // Forward slash
                        case '/':
                            putc('\\', f);
                            putc('/', f);

                            break;
                        
                        // Backspace
                        case '\b':
                            putc('\\', f);
                            putc('b', f);

                            break;
                        
                        // Form feed
                        case '\f':
                            putc('\\', f);
                            putc('f', f);

                            break;
                        
                        // Line feed
                        case '\n':
                            putc('\\', f);
                            putc('n', f);

                            break;
                        
                        // Carriage return
                        case '\r':
                            putc('\\', f);
                            putc('r', f);

                            break;
                        
                        // Horizontal tab
                        case '\t':
                            putc('\\', f);
                            putc('t', f);

                            break;

                        // TODO: Unicode
                        // TODO:case '\u':
                            // TODO:
                            // TODO:break;
                        
                        // Default
                        default:
                            putc(p_value->string[i], f);
                    }
                }
            
                // Formatting
                fprintf(f, "\"");
                break;
            }
            
            // Print an object
            case JSON_VALUE_OBJECT:
            {
                if ( p_value->object )
                {
                    // Initialized data
                    size_t        property_count = dict_values(p_value->object, 0);
                    char        **keys           = 0;
                    json_value **values          = 0;

                    fprintf(f,"{");

                    if ( property_count == 0 )
                        goto done;

                    keys   = JSON_REALLOC(0, property_count * sizeof(char*));
                    values = JSON_REALLOC(0, property_count * sizeof(json_value*));

                    dict_keys(p_value->object, keys);
                    dict_values(p_value->object, (void **)values);
                    for (size_t i = 0; i < property_count-1; i++)
                    {
                        fprintf(f,"\"%s\":",keys[i]);
                        print_json_value(values[i],f);
                        fprintf(f,",");
                    }
                    fprintf(f,"\"%s\":",keys[property_count-1]);
                    print_json_value(values[property_count-1],f);

                    JSON_REALLOC(keys, 0);
                    JSON_REALLOC(values, 0);
                    done:
                    fprintf(f,"}");
                }
                break;
            }
            
            // Print an array
            case JSON_VALUE_ARRAY:
            {
                
                // Initialized data
                size_t       element_count = 0;
                char        *keys          = 0;
                json_value **elements      = 0;

                // Error check
                if ( p_value->list == (void *) 0 )
                    goto no_list;

                // Get the contents of the array
                {

                    // Get the quantity of elements                    
                    array_get(p_value->list, 0,&element_count);

                    // Allocate memory for the elements
                    elements = JSON_REALLOC(0, element_count * sizeof(json_value*));

                    // Error check
                    if ( elements == (void *) 0 )
                        goto no_mem;

                    // Get the contents of the array
                    array_get(p_value->list, (void **)elements, 0);
                }
                
                // Formatting
                fprintf(f,"[");

                // Print the first element
                if ( element_count )
                    print_json_value(elements[0],f);
                
                // Iterate over each element
                for (size_t i = 1; i < element_count; i++)
                {

                    // Formatting
                    fprintf(f, ",");

                    // Print the value
                    print_json_value(elements[i],f);
                }

                // Free the element
                JSON_REALLOC(elements, 0);


                // Formatting
                fprintf(f, "]");
            }
                break;
            
            default:

                // Error
                return 0;
            }
    }

    // Success
    return 1;
    
    no_list:
        return 0;

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

void free_json_value ( const json_value *const p_value )
{
    
    // Argument errors
    if ( p_value == (void *)0 ) return;
    
    // Free a string
    if (p_value->type == JSON_VALUE_STRING)
    {

        if ( p_value->string ) JSON_REALLOC(p_value->string, 0);
    }

    // Free an object
    if (p_value->type == JSON_VALUE_OBJECT)
    {

        // Free each dict property
        if ( p_value->object )
        {
            dict_free_clear(p_value->object, (void(*)(const void *const))free_json_value);
            dict_destroy(&p_value->object);
        }
    }

    // Free an array
    if (p_value->type == JSON_VALUE_ARRAY)
    {
        array_free_clear(p_value->list, (void(*)(void *))free_json_value);
        array_destroy(&p_value->list);
    }

    // Free the value
    JSON_REALLOC(p_value, 0);
    
    // Success
    return;
    
}
