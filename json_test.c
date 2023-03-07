/*
 * @file queue tester
 * @author Jacob C Smith
*/

// TODO: Improve documentation

// Include
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

#include <json/json.h>
#include <queue/queue.h>

// Test results
enum result_e {
    zero=0,    
    one=1,
};

typedef enum result_e result_t;

int total_tests      = 0,
    total_passes     = 0,
    total_fails      = 0,
    ephemeral_tests  = 0,
    ephemeral_passes = 0,
    ephemeral_fails  = 0;

// Forward declarations
int  run_tests                 ( void );
int  print_final_summary       ( void );
int  print_test                ( const char  *scenario_name, const char *test_name, bool passed );

int  test_null  ( char *name );
int  test_bool  ( char *name );
int  test_int   ( char *name );
int  test_float ( char *name );

bool test_parse_json ( char *test_file, int(*expected_value_constructor) (JSONValue_t **), result_t expected );
result_t load_json ( JSONValue_t **pp_value, char *test_file );

result_t value_equals (JSONValue_t *a, JSONValue_t *b);

int  construct_null            ( JSONValue_t **pp_value );

int  construct_bool_false      ( JSONValue_t **pp_value );
int  construct_bool_true       ( JSONValue_t **pp_value );

int  construct_int_minus_one   ( JSONValue_t **pp_value ); 
int  construct_int_zero        ( JSONValue_t **pp_value ); 
int  construct_int_one         ( JSONValue_t **pp_value ); 
int  construct_int_max         ( JSONValue_t **pp_value ); 
int  construct_int_min         ( JSONValue_t **pp_value ); 

int  construct_float_minus_one ( JSONValue_t **pp_value ); 
int  construct_float_zero      ( JSONValue_t **pp_value ); 
int  construct_float_one       ( JSONValue_t **pp_value ); 
int  construct_float_max       ( JSONValue_t **pp_value ); 
int  construct_float_min       ( JSONValue_t **pp_value ); 

size_t load_file ( const char *path, void *buffer, bool binary_mode );

// Entry point
int main(int argc, const char* argv[])
{

    // Run tests
    run_tests();

    // Success
    return EXIT_SUCCESS;
}

int run_tests                 ( void )
{

    // Test the parser
    test_null("parse_null");
    test_bool("parse_bool");
    test_int("parse_int");
    test_float("parse_float");
    
    // Success
    return 1;
}

result_t load_json ( JSONValue_t **pp_value, char *test_file )
{

    // Initialized data
    size_t    file_len = 0;
    char     *file_buf = 0;
    result_t  r        = 0; 
    
    // Load the file
    {
        file_len = load_file(test_file, 0, false);
        file_buf = calloc(file_len+1, sizeof(char));
        load_file(test_file, file_buf, false);
    }

    // Parse JSON
    r = parse_json(file_buf, file_len, pp_value);

    // Success
    return r;
}

result_t value_equals (JSONValue_t *a, JSONValue_t *b)
{
    result_t result = 1;

    if (a == 0)
    {
        if (b == 0)
            return result;
        else
            return 0;
    }
    
    if (b == 0)
    {
        if (a == 0)
            return result;
        else
            return 0;
    }      

    if(a->type != b->type)
        result = 0;

    if ( a->type == JSONboolean )
        if ( a->boolean != b->boolean )
            result = 0;
    
    if ( a->type == JSONfloat )
        if ( a->floating != b->floating )
            result = 0;
    
    if ( a->type == JSONinteger )
        if ( a->integer != b->integer )
            result = 0;

    if ( a->type == JSONstring )
        if ( strcmp(a->string, b->string) )
            result = 0;

    return result;
}

int construct_null ( JSONValue_t **pp_value )
{

    *pp_value = (void *) 0;

    return 1;
}

int construct_bool_false ( JSONValue_t **pp_value )
{

    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONboolean;

    // Value
    p_value->boolean = false;

    // Success
    return 1;
}

int construct_bool_true ( JSONValue_t **pp_value )
{

    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONboolean;

    // Value
    p_value->boolean = true;

    // Success
    return 1;
}

int construct_int_minus_one ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONinteger;

    // Value
    p_value->integer = -1;

    // Success
    return 1;
}

int construct_int_zero ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONinteger;

    // Value
    p_value->integer = 0;

    // Success
    return 1;
}

int construct_int_one ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONinteger;

    // Value
    p_value->integer = 1;

    // Success
    return 1;
}

int construct_int_max ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONinteger;

    // Value
    p_value->integer = INT64_MAX;

    // Success
    return 1;
}

int construct_int_min ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONinteger;

    // Value
    p_value->integer = INT64_MIN;

    // Success
    return 1;
}

int construct_float_minus_one ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONfloat;

    // Value
    p_value->floating = -1.0;

    // Success
    return 1;
}

int construct_float_zero ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONfloat;

    // Value
    p_value->floating = 0.0;

    // Success
    return 1;
}

int construct_float_one ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONfloat;

    // Value
    p_value->floating = 1.0;

    // Success
    return 1;
}

int construct_float_max ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONfloat;

    // Value
    p_value->floating = DBL_MAX;

    // Success
    return 1;
}

int construct_float_min ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONfloat;

    // Value
    p_value->floating = DBL_MIN;

    // Success
    return 1;
}

int construct_string_empty ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "";

    // Success
    return 1;
}

int construct_string_a ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "a";

    // Success
    return 1;
}

int construct_string_abc ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "abc";

    // Success
    return 1;
}

int construct_string_quote_abc_quote ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\"abc\"";

    // Success
    return 1;
}

int construct_object_empty ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONobject;

    // Value
    dict_construct(&p_value->object, 1);

    // Success
    return 1;
}

int construct_object_string ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONstring;
    p_abc_value->string = "def";
    
    // Value
    dict_construct(&p_value->object, 1);

    // Success
    return 1;
}

int construct_object_int ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONinteger;
    p_abc_value->integer = 123;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Success
    return 1;
}

int construct_object_object ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_def_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONobject;

    p_def_value->type    = JSONinteger;
    p_def_value->integer = 123;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_construct(&p_abc_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);
    dict_add(p_abc_value->object, "def", p_abc_value);

    // Success
    return 1;
}

bool test_parse_json ( char *test_file, int(*expected_value_constructor) (JSONValue_t **), result_t expected )
{
    
    // Initialized data
    JSONValue_t *p_return_value   = 0,
                *p_expected_value = 0;
    bool         ret      = true;
    size_t       file_len = 0;
    result_t    *result   = 0;
    char        *file_buf = 0;

    if (expected_value_constructor)
        expected_value_constructor(&p_expected_value);

    result = load_json ( &p_return_value, test_file );

    // Success
    return (expected == value_equals(p_return_value, p_expected_value));

}

int test_null ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "null", test_parse_json("test cases/pass/null.json", (void *) 0, one));

    print_final_summary();

    return 1;

}

int test_bool ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "false", test_parse_json("test cases/pass/bool/bool_false.json", construct_bool_false, one));
    print_test(name, "true" , test_parse_json("test cases/pass/bool/bool_true.json", construct_bool_true, one));

    print_final_summary();

    return 1;

}

int test_int ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "int_-1" , test_parse_json("test cases/pass/int/int_-1.json" , construct_int_minus_one, one));
    print_test(name, "int_0"  , test_parse_json("test cases/pass/int/int_0.json"  , construct_int_zero     , one));
    print_test(name, "int_1"  , test_parse_json("test cases/pass/int/int_1.json"  , construct_int_one      , one));
    print_test(name, "int_max", test_parse_json("test cases/pass/int/int_max.json", construct_int_minus_one, one));
    print_test(name, "int_min", test_parse_json("test cases/pass/int/int_min.json", construct_int_zero     , one));

    print_final_summary();

    return 1;
}

int test_float ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "float_-1" , test_parse_json("test cases/pass/float/float_-1.json" , construct_float_minus_one, one));
    print_test(name, "float_0"  , test_parse_json("test cases/pass/float/float_0.json"  , construct_float_zero     , one));
    print_test(name, "float_1"  , test_parse_json("test cases/pass/float/float_1.json"  , construct_float_one      , one));
    print_test(name, "float_max", test_parse_json("test cases/pass/float/float_max.json", construct_float_minus_one, one));
    print_test(name, "float_min", test_parse_json("test cases/pass/float/float_min.json", construct_float_zero     , one));

    print_final_summary();

    return 1;
}

int test_string ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "string \"a\""      , test_parse_json("test cases/pass/string/string_a.json"              , /* TODO */ 0, one));
    print_test(name, "string \"abc\""    , test_parse_json("test cases/pass/string/string_abc.json"            , /* TODO */ 0, one));
    print_test(name, "string \"\""       , test_parse_json("test cases/pass/string/string_empty.json"          , /* TODO */ 0, one));
    print_test(name, "string \"\"abc\"\"", test_parse_json("test cases/pass/string/string_quote_abc_quote.json", /* TODO */ 0, one));
    print_test(name, "string \"    abc\"", test_parse_json("test cases/pass/string/string_whitespaces_abc.json", /* TODO */ 0, one));


    print_final_summary();

    return 1;
}

int print_test ( const char *scenario_name, const char *test_name, bool passed )
{

    // Initialized data
    printf("%s_test_%-17s %s\n",scenario_name, test_name, (passed) ? "PASS" : "FAIL");

    // Increment the counters
    {
        if (passed)
        {
            ephemeral_passes++;
        }
        else
        {
            ephemeral_fails++;
        }

        ephemeral_tests++;
    }

    // Success
    return 1;
}

int print_final_summary ()
{

    // Accumulate
    total_tests  += ephemeral_tests,
    total_passes += ephemeral_passes,
    total_fails  += ephemeral_fails;

    // Print
    printf("\nTests: %d, Passed: %d, Failed: %d (%%%.3f)\n",  ephemeral_tests, ephemeral_passes, ephemeral_fails, ((float)ephemeral_passes/(float)ephemeral_tests*100.f));
    printf("Total: %d, Passed: %d, Failed: %d (%%%.3f)\n\n",  total_tests, total_passes, total_fails, ((float)total_passes/(float)total_tests*100.f));
    
    ephemeral_tests  = 0;
    ephemeral_passes = 0;
    ephemeral_fails  = 0;

    // Success
    return 1;
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
