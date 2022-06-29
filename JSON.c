#include <JSON/JSON.h>

// TODO: Rewrite the JSON parser in assembly with SIMD string instructions.
//       That ought to be a fun challenge.

// TODO: Rewrite the JSON parser in assembly with SIMD string instructions.
//       That ought to be a fun challenge.

DLLEXPORT int parse_json ( char* token_text, size_t len, dict** dictionary )
{

    // Argument check
    {
        #ifndef NDEBUG
            if(token_text == (void*)0)
                goto noTokenText;
            if(len == 0)
                goto noLen;
            
        #endif
    }

    // Initialized data.
    size_t token_iterator = 0,
           count          = 0,
           i              = 1;

    dict  *i_dictionary   = 0;

    // This is the preparse branch. The code parses the JSON and counts how many key/value pairs there are in the token 
    {
        // Walk through the file one character at a time
        for(; i < len; i++)
        {
            // Hitting a colon means that there is a key value pair around here
            if ( token_text[i] == ':' )      
                count++;

            // Hitting a bracket means there is an object, and need it needs to be skipped over
            else if ( token_text[i] == '{' ) 
            {
                // The iterater just passed a bracket, so b_depth = 1.
                size_t b_depth = 1; 
                while ( b_depth )
                {	
                    // Keep iterating through the object until b_depth == 0 
                    i++;

                    if ( token_text[i] == '{' )            // If the char is a '{', the iterator just passed into an object, so the counter is  incremented
                        b_depth++;
                    if ( token_text[i] == '}' )            // If the char is a '}', the iterator just passet out of an object, so the counter is decremented
                        b_depth--;
                    else if ( token_text[i] == '\"' )      // If the char is a '\"', the iterator skips to the next '\"'
                        while ( token_text[i++] != '\"' ); 
                        
                }
            }

            // Hitting a '\"' means there is a string, and it needs to be skipped over
            else if ( token_text[i] == '\"' )              
                while ( token_text[++i] != '\"' );
        }
            
    }
    size_t tmpc = count;
    dict_construct(dictionary, count*2);
    i_dictionary = *dictionary;

    // This is the body of the parser. The code parses 'count' number of JSON tokens from 'token_text' into 'tokens'
    while ( *++token_text )
    {

        // Found a key.
        if ( *token_text == '\"' ) 
        {
            // Allocate for a token
            JSONToken_t* token = calloc(1, sizeof(JSONToken_t));

            // Parse the key name and update the token
            {
                token->key = token_text + 1;  // Set the current tokens key pointer to the start of the key.
                while (*++token_text != '\"');                // Go to the end of the string.

                *token_text = 0;                              // Set a null terminator at the end of the string, overwriting the ending '\"' with a '\0'.
                token_text++;                                 // Iterate past the null terminator
                while (*token_text++ != ':');                 // Till the ':'.
                while (*++token_text == ' ');                 // and past any whitespaces.
            }

            // Parse the value into a token and update the token
            {
                // Parse out a string
                if (*token_text == '\"')
                {
                    // Format the value 
                    token->value.n_where = token_text + 1; // Set the value pointer to the correct value
                    while (*++token_text != '\"');                        // Skip past all the contents of the string to the '\"'
                    *token_text = 0;                                      // Replace the closing quote with a null terminator

                    token->type = JSONstring;             // Set the type as a string
                }

                // Parse out a primative.
                else if ( ( *token_text >= '0' && *token_text <= '9' ) || 
                            *token_text == '.'                         ||
                            *token_text == '-' )
                {
                    token->value.n_where = token_text;       // Set the value pointer to the correct value
                    token->type          = JSONprimative;    // Set the type as a primative    
                    while ( (*++token_text >= '0' && *token_text <= '9') || // Skip past the number
                            *token_text == '.' );                          

                }

                // Parse out an object
                else if (*token_text == '{')
                {
                    // Initialized data
                    size_t b_depth = 1;                               // b_depth again keeps track of the bracket depth

                    token->value.n_where = token_text; // Set the value pointer to point to the current character, the start of the object
                    token->type          = JSONobject; // Set the type as an object

                    while (b_depth)                                   // Similar to the preparsing subroutine, skip past anything in the object that could trip up the parser,
                    {                                                 // and keep track of the bracket depth.
                        token_text++;
                        if (*token_text == '{')
                            b_depth++;
                        else if (*token_text == '}')
                            b_depth--;
                        else if (*token_text == '\"')
                            while (*++token_text != '\"');
                    }
                    
                }

                // Parse out an array
                else if (*token_text == '[')
                {

                    // Initialized data
                    size_t i       = 0, // This is an iterator.
                           j       = 1, // This iterates over the size of the array.
                           b_depth = 1, // This keeps track of square bracket depth.
                           c_depth = 0; // This keeps track of regular bracked depth.

                    token->type = JSONarray; // Set the type.

                    while (b_depth)
                    {
                        i++;
                        if (token_text[i] == '[')       // These two cases keep track of square bracket depth.
                            b_depth++;
                        else if (token_text[i] == ']') 
                            b_depth--;
                        else if (token_text[i] == '{')  // and these two keep track of regular bracket depth.
                            c_depth++;
                        else if (token_text[i] == '}')
                            c_depth--;
                        else if (token_text[i] == ',')  // Hitting a ',' increments the number of array elements if the both the bracket depths are 0
                            ((b_depth == 1) && (c_depth == 0)) ? j++ : j; 
                        else if (token_text[i] == '\"') // Again, ignore strings.
                            while (token_text[++i] != '\"');
                    }

                    if (j > 1) // If there are any array elements at all, the counter is incremented to allocate for a null terminator
                        j++;

                    token->value.a_where = calloc((j + 2), sizeof(void*)); // Allocate some space to start placing the pointers.
                    token_text++;                                                         // Skip past the '['.

                    b_depth = 1,                                                          // Reset square bracket depth, regular bracket depth, and the iterator.
                    c_depth = 0,
                    i       = 0;

                    // Loop until the iterator is out of the square brackets.
                    while (b_depth) 
                    {
                        // Parse a string out of the array, like above
                        if (*token_text == '\"')                                          
                        {
                            token->value.a_where[i] = ++token_text;
                            while (*++token_text != '\"');
                            *token_text = '\0';
                            token_text++;
                            i++;
                        }

                        // Parse an object out, just like key/value parsing 
                        else if (*token_text == '{')                                      
                        {
                            token->value.a_where[i] = token_text;
                            c_depth++;
                            while (c_depth)
                            {
                                token_text++;
                                if (*token_text == '{')
                                    c_depth++;
                                else if (*token_text == '}')
                                    c_depth--;
                                else if (*token_text == '\"')
                                    while (*++token_text != '\"');
                            }
                            *++token_text = '\0';
                            i++;
                            c_depth = 0;
                        }

                        // Parse an array out.
                        else if (*token_text == '[')										
                        {
                            // TODO: Finish
                            token->value.a_where[i] = token_text;
                        }

                        // Parse a primative, just like below.
                        else if ((*token_text >= '0' && *token_text <= '9') || *token_text == '-')                       
                        {
                            token->value.a_where[i] = token_text;
                            while ((*++token_text >= '0' && *token_text <= '9') || *token_text == '.' || *token_text == '-');
                            i++;
                            *token_text = '\0';
                        }

                        if (*token_text == ']')                                             // Decrement square bracket depth if there is a square bracket on the iterator
                            b_depth--;
                        token_text++;
                    }
                    token->value.a_where[i] = (void*)0;
                }

                // Parse out true, false or null
                else if (*token_text == 't' || *token_text == 'f' || *token_text == 'n')
                {
                    if (strncmp(token_text,"true",4) == 0)
                    {
                        token->value.n_where = (void *) 1;
                    }

                    else if (strncmp(token_text, "false", 5) == 0 || strncmp(token_text, "null", 4) == 0)
                    {
                        token->value.n_where = 0;
                    }
                    token->type = JSONprimative;
                }

                // Smoething has gone very wrong
                else
                    goto exitNotAllParsed;
            }
            token_iterator++;                                                                        // Increment the token index

            dict_add(i_dictionary, token->key, token);

            // If all the tokens are accounted for, the code exits on the good branch
            if (token_iterator == tmpc)
                goto exitAllParsed;

            // Else, keep parsing tokens
            while (*token_text++ != ',');
            *--token_text = 0;

            

        }
    }
exitAllParsed:
    {
        while(*token_text++!='}');
        *(char*)token_text = '\0';
        *dictionary = i_dictionary;
        return tmpc;
    }

    // Error handling
    {
        // Bad parameters
        {
            noTokenText:
                #ifndef NDEBUG
                    printf("[JSON] Null pointer provided for token_text\n");
                #endif
                return 0;

            noLen:
                #ifndef NDEBUG
                    printf("[JSON] No length provided for text\n");
                #endif
                return 0;
        }
        
        // If the code exits on this branch, something has gone very wrong.
        exitNotAllParsed:
            #ifndef NDEBUG
                printf("[JSON] Not all JSON tokens were parsed. Only %ld out of %ld were parsed", token_iterator, tmpc);
            #endif
            return ( -1 * token_iterator);
    }
    return 1;

}

int encode_json(FILE* buffer, size_t count, JSONToken_t* tokens)
{
    // Argument check
    {

    }

    // Format the beginning of the object
    fputc('{', buffer);
    fputc('\n', buffer);

    // Iterate over tokens
    for (size_t i = 0; i < count; i++)
    {

        // Write the key
        {
            fputc('\t', buffer);

            fputc('\"', buffer);
            fputs(tokens[i].key, buffer);
            fputc('\"', buffer);
        }

        // Write the seperator
        {
            fputc(' ', buffer);
            fputc(':', buffer);
            fputc(' ', buffer);
        }

        // Write the object
        {
            switch (tokens[i].type)
            {
                case JSONstring:
                    fprintf(buffer, "\"%s\"", tokens[i].value.n_where);
                    break;
                case JSONprimative:
                    fprintf(buffer, "\"%s\"", tokens[i].value.n_where);
                    break;

                case JSONarray:
                    break;

                case JSONobject:

                    break;
            }
        }

        // Write a comma if not on the last element
        {
            if (i < count - 1)
                fputc(',', buffer);
        }
        fputc('\n', buffer);

    }

    fputc('}', buffer);

    fflush(buffer);
    return 0;
}
