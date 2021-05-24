#include "include/JSON.h"

int parseJSON ( char* text, size_t len, size_t count, JSONValue_t* where )
{
    // Initialized data.
    size_t currentWhere = 0,
           ret          = 0,
           i            = 1;

    // If we aren't passed a count, we figure out how many top level items there are in the JSON file. 
    if (count == 0)
    {
        // Walk through the file.
        for(i; i < len; i++)
        {
            if (text[i] == ':')                                                                    // Any time we hit a ':', we have come across a key/value pair; We increment ret.
                ret++;
            else if (text[i] == '{')                                                               // Any time we hit a '{', we have come across an object, and need to skip over it. 
            {
                size_t bDepth = 1;                                                                 // We are inside brackets, so bDepth = 1.
                while (bDepth)
                {	
                    i++;                                                                           // Get the next character.

                    if (text[i] == '{')                                                            // If its a '{', we have entered another pair of brackets, so we are deeper.
                        bDepth++;
                    if (text[i] == '}')                                                            // If its a '}', we have exited a pair of brackets, so we are less deep.
                        bDepth--;
                    else if (text[i] == '\"')                                                      // There could always be a string in the file with a '{' or a '}' in it, so 
                        while (text[i++] != '\"');                                                 // we'll just skip over the entire string if we come across it.
                    
                }
            }
            else if (text[i] == '\"')                                                              // Any time we hit a '\"', we have come across a string, and we should just skip it.
                while (text[++i] != '\"');
        }

        return ret;                                                                                // Return the total number of key/value pairs we have found.
    }
    
    while (*++text)                                                                                // This is the main loop. It searches for key / value pairs and extracts data.
    {
        if (*text == '\"')                                                                         // We've found a key.
        {
            where[currentWhere].name = text + 1;                                                   // Set a pointer to the start of the key.
            while (*++text != '\"');                                                               // Go to the end of the string.

            *text = 0;                                                                             // Set a null terminator at the end of the string, overwriting the '\"' with a '\0'.
            text++;                                                                                // Iterate past the null terminator
            while (*text++ != ':');                                                                // Till the ':'.
            while (*++text == ' ');                                                                // and past any whitespaces.

            if (*text == '\"')                                                                     // Parse out a string
            {
                where[currentWhere].content.nWhere = ++text;                                       // Set the value pointer to one past the current text location, as to skip past the \"
                where[currentWhere].type           = JSONstring;                                   // Set the type as a string
                while (*++text != '\"');                                                           // Skip past all the contents of the string to the '\"'
                *text = 0;                                                                         // Replace the closing quote with a null terminator
            }
            else if (*text == '{')                                                                 // Parse out an object
            {
                // Initialized data
                size_t bDepth = 1;                                                                 // bDepth again keeps track of the bracket depth

                where[currentWhere].content.nWhere = text;                                         // Set the value pointer to point to the current character, the start of the object
                where[currentWhere].type           = JSONobject;                                   // Set the type as an object

                while (bDepth)                                                                     // Similar to the preparsing subroutine, we skip past anything in the object that could trip up the parser,
                {                                                                                  // and keep track of the bracket depth so we know when the value is complete.
                    text++;
                    if (*text == '{')
                        bDepth++;
                    else if (*text == '}')
                        bDepth--;
                    else if (*text == '\"')
                        while (*++text != '\"');
                }

                *++text=0;
            }
            else if (*text == '[')                                                                 // Parse out an array.
            {
                // Initialized data
                size_t c        = 0;                                                               // This is the iterator.
                size_t bDepth   = 1;                                                               // This keeps track of square bracket depth.
                size_t cDepth   = 0;                                                               // This keeps track of regular bracked depth.
                size_t aEntries = 1;                                                               // And this keeps track of the size of the array.

                where[currentWhere].type = JSONarray;                                              // Set the type.

                while (bDepth)
                {
                    c++;
                    if (text[c] == '[')                                                            // These two cases keep track of square bracket depth.
                        bDepth++;
                    else if (text[c] == ']')
                        bDepth--;
                    else if (text[c] == '{')                                                       // and these two keep track of regular bracket depth.
                        cDepth++;
                    else if (text[c] == '}')
                        cDepth--;
                    else if (text[c] == ',')                                                       // If we hit a ',', we only want to keep it if its not inside an object and its on the 
                        ((bDepth == 1) && (cDepth == 0)) ? aEntries++ : aEntries;                  // top level of the array.
                    else if (text[c] == '\"')                                                      // Again, ignore strings.
                        while (text[++c] != '\"');
                }

                if (aEntries > 1)                                                                  // If we've parsed anything, we increment the counter so that we can have a null terminator
                    aEntries++;                                                                    // in the list of pointers to array members.
                
                where[currentWhere].content.aWhere = calloc((aEntries + 1),sizeof(void*));         // Allocate some space to start placing the pointers.
                text++;                                                                            // Skip past the '['.

                bDepth = 1,                                                                        // Reset square bracket depth, regular bracket depth, and the iterator.
                cDepth = 0,
                c      = 0; 

                while (bDepth)                                                                     // Loop until we're out of the square brackets.
                {
                    if (*text == '\"')                                                             // Parse a string out of the array, like above
                    {
                        where[currentWhere].content.aWhere[c] = ++text;
                        while (*++text != '\"');
                        *text = '\0';
                        text++;
                        c++;
                        cDepth = 1;
                    }
                    else if (*text == '{')                                                         // Parse an object out, just like key/value parsing 
                    {
                        where[currentWhere].content.aWhere[c] = text;
                        cDepth++;
                        while (cDepth)
                        {
                            text++;
                            if (*text == '{')
                                cDepth++;
                            else if (*text == '}')
                                cDepth--;
                            else if (*text == '\"')
                                while (*++text != '\"');
                        }
                        *++text = '\0';
                        c++;
                        cDepth = 0;
                    }
                    else if (*text == '[')														   // Parse an array out.
                    {
                        // TODO: Finish
                        where[currentWhere].content.aWhere[c] = text; 
                    }
                    else if ((*text >= '0' && *text <= '9') || *text == '-')                       // Parse a primative, just like below.
                    {
                        where[currentWhere].content.aWhere[c] = text;
                        while ((*++text >= '0' && *text <= '9') || *text == '.' || *text == '-'); 
                        c++;
                        *text = '\0';
                    }
                    if (*text == ']')                                                              // Decrement square bracket depth if we hit a square bracket.
                        bDepth--;
                    text++;
                }
                where[currentWhere].content.aWhere[c] = (void*)0;
            }

            // Parse out a primative.
            else if ((*text >= '0' && *text <= '9') || *text == '.' || *text == '-')              
            {
                where[currentWhere].content.nWhere = text;                                         // Set a pointer to the text
                where[currentWhere].type           = JSONprimative;                                // Set the type as a primative    
                while ((*++text >= '0' && *text <= '9') || *text == '.');                          // Skip all the way past the number

            }
            currentWhere++;                                                                        // Increment the token index

            // If we've gotten everything we need, we clean up and leave
            if (currentWhere == (size_t)count)                                                     // This is the exit condition we want to meet
                goto exitAllParsed;

            // Else, just keep going until we find a new key / value pair
            while (*text++ != ',');                                                                // Skip to the next ','
            *--text = 0;
        }
    }

    // If we have exited the loop this way, we haven't exited the right way. 
    exitNotAllParsed:
        #ifndef NDEBUG
            printf("Not all JSON tokens were parsed. Only %lld out of %lld were parsed", currentWhere, count);
        #endif
        return 0;
    exitAllParsed:
        return 0;
}