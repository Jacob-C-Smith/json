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
#include <errno.h>
#include <float.h>

#include <json/json.h>
#include <dict/dict.h>
#include <array/array.h>

//////////////////
// Test results //
//////////////////
typedef enum result_e {
    zero=0,    
    one=1,
} result_t;

///////////////////
// Test counters //
///////////////////
int total_tests      = 0,
    total_passes     = 0,
    total_fails      = 0,
    ephemeral_tests  = 0,
    ephemeral_passes = 0,
    ephemeral_fails  = 0;

//////////////////////////
// Forward declarations //
//////////////////////////

// Utility functions
int      run_tests           ( void );
int      print_final_summary ( void );
int      print_test          ( const char   *scenario_name, const char   *test_name,                                    bool     passed );
bool     test_parse_json     ( char         *test_file    , int         (*expected_value_constructor) (JSONValue_t **), result_t expected );
result_t load_json           ( JSONValue_t **pp_value     , char         *test_file );
result_t value_equals        ( JSONValue_t  *a            , JSONValue_t  *b );
size_t   load_file           ( const char   *path         , void         *buffer                                      , bool     binary_mode );

// Testing domains
int  test_null   ( char *name );
int  test_bool   ( char *name );
int  test_int    ( char *name );
int  test_float  ( char *name );
int  test_string ( char *name );
int  test_object ( char *name );
int  test_array  ( char *name );

// Scenario constructors
int  construct_null                   ( JSONValue_t **pp_value );

int  construct_bool_false             ( JSONValue_t **pp_value );
int  construct_bool_true              ( JSONValue_t **pp_value );

int  construct_int_minus_one          ( JSONValue_t **pp_value ); 
int  construct_int_zero               ( JSONValue_t **pp_value ); 
int  construct_int_one                ( JSONValue_t **pp_value ); 
int  construct_int_max                ( JSONValue_t **pp_value ); 
int  construct_int_min                ( JSONValue_t **pp_value ); 

int  construct_float_minus_one        ( JSONValue_t **pp_value ); 
int  construct_float_zero             ( JSONValue_t **pp_value ); 
int  construct_float_one              ( JSONValue_t **pp_value ); 
int  construct_float_max              ( JSONValue_t **pp_value ); 
int  construct_float_min              ( JSONValue_t **pp_value ); 

int construct_string_empty            ( JSONValue_t **pp_value );
int construct_string_a                ( JSONValue_t **pp_value );
int construct_string_abc              ( JSONValue_t **pp_value );
int construct_string_quote_abc_quote  ( JSONValue_t **pp_value );
int construct_string_quote            ( JSONValue_t **pp_value );
int construct_string_quote_quote      ( JSONValue_t **pp_value );
int construct_string_whitespaces_abc  ( JSONValue_t **pp_value );
int construct_string_reverse_solidus  ( JSONValue_t **pp_value );
int construct_string_solidus          ( JSONValue_t **pp_value );
int construct_string_backspace        ( JSONValue_t **pp_value );
int construct_string_formfeed         ( JSONValue_t **pp_value );
int construct_string_linefeed         ( JSONValue_t **pp_value );
int construct_string_carriage_return  ( JSONValue_t **pp_value );
int construct_string_horizontal_tab   ( JSONValue_t **pp_value );

int construct_object_empty            ( JSONValue_t **pp_value );
int construct_object_string           ( JSONValue_t **pp_value );
int construct_object_int              ( JSONValue_t **pp_value );
int construct_object_float            ( JSONValue_t **pp_value );
int construct_object_false            ( JSONValue_t **pp_value );
int construct_object_true             ( JSONValue_t **pp_value );
int construct_object_strings          ( JSONValue_t **pp_value );
int construct_object_mixed_values     ( JSONValue_t **pp_value );
int construct_object_object           ( JSONValue_t **pp_value );
int construct_object_object_object    ( JSONValue_t **pp_value );
int construct_object_recursive        ( JSONValue_t **pp_value );
int construct_object_array            ( JSONValue_t **pp_value );
int construct_object_array_objects    ( JSONValue_t **pp_value );
int construct_object_array_object     ( JSONValue_t **pp_value );

int construct_array_empty             ( JSONValue_t **pp_value );
int construct_array_null              ( JSONValue_t **pp_value );
int construct_array_nulls             ( JSONValue_t **pp_value );
int construct_array_bool              ( JSONValue_t **pp_value );
int construct_array_bools             ( JSONValue_t **pp_value );
int construct_array_int               ( JSONValue_t **pp_value );
int construct_array_ints              ( JSONValue_t **pp_value );
int construct_array_float             ( JSONValue_t **pp_value );
int construct_array_floats            ( JSONValue_t **pp_value );
int construct_array_string_empty      ( JSONValue_t **pp_value );
int construct_array_string            ( JSONValue_t **pp_value );
int construct_array_strings           ( JSONValue_t **pp_value );
int construct_array_object_empty      ( JSONValue_t **pp_value );
int construct_array_object            ( JSONValue_t **pp_value );
int construct_array_objects           ( JSONValue_t **pp_value );
int construct_array_array_empty       ( JSONValue_t **pp_value );
int construct_array_array_array_empty ( JSONValue_t **pp_value );
int construct_array_matrix            ( JSONValue_t **pp_value );
int construct_array_tensor            ( JSONValue_t **pp_value );

// Entry point
int main      ( int argc, const char* argv[] )
{

    // Run tests
    run_tests();

    // Success
    return EXIT_SUCCESS;
}

int run_tests ( void )
{

    // Test the parser

    // Test parsing valid and invalid null
    test_null("parse_null");

    // Test parsing valid and invalid booleans
    test_bool("parse_bool");

    // Test parsing valid, invalid, and over/underflowed integers
    test_int("parse_int");

    // Test parsing valid, invalid, and over/underflowed floating point numbers
    test_float("parse_float");
    
    // Test parsing valid, invalid, and unicode strings
    test_string("parse_string");

    // Test parsing a variety of objects
    test_object("parse_object");

    // Test parsing a variety of arrays
    test_array("parse_array");

    // Success
    return 1;
}

int test_null ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "null", test_parse_json("test cases/pass/null.json", (void *) 0, one));
    print_test(name, "nul" , test_parse_json("test cases/fail/null.json", (void *) 0, zero));

    print_final_summary();

    return 1;

}

int test_bool ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "false", test_parse_json("test cases/pass/bool/bool_false.json", construct_bool_false, one));
    print_test(name, "true" , test_parse_json("test cases/pass/bool/bool_true.json" , construct_bool_true , one));
    print_test(name, "fals" , test_parse_json("test cases/fail/bool/bool_false.json", (void *) 0          , zero));
    print_test(name, "tru"  , test_parse_json("test cases/fail/bool/bool_true.json" , (void *) 0          , zero));

    print_final_summary();

    return 1;

}

int test_int ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "int_-1"    , test_parse_json("test cases/pass/int/int_-1.json" , construct_int_minus_one, one));
    print_test(name, "int_0"     , test_parse_json("test cases/pass/int/int_0.json"  , construct_int_zero     , one));
    print_test(name, "int_1"     , test_parse_json("test cases/pass/int/int_1.json"  , construct_int_one      , one));
    print_test(name, "int_max"   , test_parse_json("test cases/pass/int/int_max.json", construct_int_max      , one));
    print_test(name, "int_min"   , test_parse_json("test cases/pass/int/int_min.json", construct_int_min      , one));
    print_test(name, "int_max_+1", test_parse_json("test cases/fail/int/int_max.json", (void *)0              , zero));
    print_test(name, "int_min_-1", test_parse_json("test cases/fail/int/int_min.json", (void *)0              , zero));

    print_final_summary();

    return 1;
}

int test_float ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "float_-1"    , test_parse_json("test cases/pass/float/float_-1.json" , construct_float_minus_one, one));
    print_test(name, "float_0"     , test_parse_json("test cases/pass/float/float_0.json"  , construct_float_zero     , one));
    print_test(name, "float_1"     , test_parse_json("test cases/pass/float/float_1.json"  , construct_float_one      , one));
    print_test(name, "float_max"   , test_parse_json("test cases/pass/float/float_max.json", construct_float_max      , one));
    print_test(name, "float_min"   , test_parse_json("test cases/pass/float/float_min.json", construct_float_min      , one));
    print_test(name, "float_max_+1", test_parse_json("test cases/fail/float/float_max.json", (void *)0                , zero));
    print_test(name, "float_min_-1", test_parse_json("test cases/fail/float/float_min.json", (void *)0                , zero));

    print_final_summary();

    return 1;
}

int test_string ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "string \"\""       , test_parse_json("test cases/pass/string/string_empty.json"          , construct_string_empty          , one));
    print_test(name, "string \"a\""      , test_parse_json("test cases/pass/string/string_a.json"              , construct_string_a              , one));
    print_test(name, "string \"abc\""    , test_parse_json("test cases/pass/string/string_abc.json"            , construct_string_abc            , one));
    print_test(name, "string \"\"abc\"\"", test_parse_json("test cases/pass/string/string_quote_abc_quote.json", construct_string_quote_abc_quote, one));
    print_test(name, "string \"\"\"\""   , test_parse_json("test cases/pass/string/string_quote_quote.json"    , construct_string_quote_quote    , one));
    print_test(name, "string     \"abc\"", test_parse_json("test cases/pass/string/string_whitespaces_abc.json", construct_string_whitespaces_abc, one));
    print_test(name, "string \"\"\""     , test_parse_json("test cases/pass/string/string_quote.json"          , construct_string_quote          , one));
    print_test(name, "string \"\\\\\""   , test_parse_json("test cases/pass/string/string_reverse_solidus.json", construct_string_reverse_solidus, one));
    print_test(name, "string \"\\/\""    , test_parse_json("test cases/pass/string/string_solidus.json"        , construct_string_solidus        , one));
    print_test(name, "string \"\\b\""    , test_parse_json("test cases/pass/string/string_backspace.json"      , construct_string_backspace      , one));
    print_test(name, "string \"\\f\""    , test_parse_json("test cases/pass/string/string_formfeed.json"       , construct_string_formfeed       , one));
    print_test(name, "string \"\\n\""    , test_parse_json("test cases/pass/string/string_linefeed.json"       , construct_string_linefeed       , one));
    print_test(name, "string \"\\r\""    , test_parse_json("test cases/pass/string/string_carriage_return.json", construct_string_carriage_return, one));
    print_test(name, "string \"\\t\""    , test_parse_json("test cases/pass/string/string_horizontal_tab.json" , construct_string_horizontal_tab , one));
    //print_test(name, "string \"\u1234\"" , test_parse_json("test cases/pass/string/string_escape.json"         , construct_string_escape         , one));

    print_final_summary();

    return 1;
}

int test_object ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "object {}"                                               , test_parse_json("test cases/pass/object/object_empty.json"        , construct_object_empty        , one));
    print_test(name, "object {\"abc\":\"def\"}"                                , test_parse_json("test cases/pass/object/object_string.json"       , construct_object_string       , one));
    print_test(name, "object {\"abc\":123}"                                    , test_parse_json("test cases/pass/object/object_int.json"          , construct_object_int          , one));
    print_test(name, "object {\"pi\":3.14}"                                    , test_parse_json("test cases/pass/object/object_float.json"        , construct_object_float        , one));
    print_test(name, "object {\"abc\":false}"                                  , test_parse_json("test cases/pass/object/object_false.json"        , construct_object_false        , one));
    print_test(name, "object {\"abc\":true}"                                   , test_parse_json("test cases/pass/object/object_true.json"         , construct_object_true         , one));
    print_test(name, "object {\"abc\":\"def\",\"ghi\":\"jkl\",\"mno\":\"pqr\"}", test_parse_json("test cases/pass/object/object_strings.json"      , construct_object_strings      , one));
    print_test(name, "object {\"name\":\"jake\",\"age\":20,\"height\":1.779}"  , test_parse_json("test cases/pass/object/object_mixed_values.json" , construct_object_mixed_values , one));
    print_test(name, "object {\"abc\":{\"def\":123}}"                          , test_parse_json("test cases/pass/object/object_object.json"       , construct_object_object       , one));
    print_test(name, "object {\"abc\":{\"def\":{\"ghi\":123}}}"                , test_parse_json("test cases/pass/object/object_object_object.json", construct_object_object_object, one));
    // TODO
    print_test(name, "object {\"abc\":[1,2,3]}"                                , test_parse_json("test cases/pass/object/object_array.json"        , construct_object_array        , one));
    print_test(name, "object {\"a\":[{\"a\":1},{\"b\":2},{\"c\":3}]}"          , test_parse_json("test cases/pass/object/object_array.json"        , construct_object_array_objects, one));
    print_test(name, "object {\"a\":[{\"a\":1}]}"                              , test_parse_json("test cases/pass/object/object_array_object.json" , construct_object_array_object , one));

    //print_test(name, "object {\"a\":[{\"b\":[ ... {\"y\":[{\"z\":[]}]}]}]}"    , test_parse_json("test cases/pass/object/object_array.json"        , //TODO        , one));
    //print_test(name, "object {\"a\":{\"b\":{\"c\":{\"d\":{...{\"z\":{ }...}"   , test_parse_json("test cases/pass/object/object_recursive.json"    , construct_object_recursive    , one));

    print_final_summary();

    return 1;
}

int test_array ( char *name )
{

    // Initialized data
    JSONValue_t *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "array []"                                  , test_parse_json("test cases/pass/array/array_empty.json"            , construct_array_empty            , one));
    print_test(name, "array [null]"                              , test_parse_json("test cases/pass/array/array_null.json"             , construct_array_null             , one));
    print_test(name, "array [null, null, null]"                  , test_parse_json("test cases/pass/array/array_nulls.json"            , construct_array_nulls            , one));
    print_test(name, "array [true]"                              , test_parse_json("test cases/pass/array/array_bool.json"             , construct_array_bool             , one));
    print_test(name, "array [true, false, true]"                 , test_parse_json("test cases/pass/array/array_bools.json"            , construct_array_bools            , one));
    print_test(name, "array [1]"                                 , test_parse_json("test cases/pass/array/array_int.json"              , construct_array_int              , one));
    print_test(name, "array [1, 2, 3]"                           , test_parse_json("test cases/pass/array/array_ints.json"             , construct_array_ints             , one));
    print_test(name, "array [3.141]"                             , test_parse_json("test cases/pass/array/array_float.json"            , construct_array_float            , one));
    print_test(name, "array [1.2, 3.4, 5.6]"                     , test_parse_json("test cases/pass/array/array_floats.json"           , construct_array_floats           , one));
    print_test(name, "array [\"\"]"                              , test_parse_json("test cases/pass/array/array_string_empty.json"     , construct_array_string_empty     , one));
    print_test(name, "array [\"abc\"]"                           , test_parse_json("test cases/pass/array/array_string.json"           , construct_array_string           , one));
    print_test(name, "array [\"abc\", \"def\", \"ghi\"]"         , test_parse_json("test cases/pass/array/array_strings.json"          , construct_array_strings          , one));
    print_test(name, "array [{}]"                                , test_parse_json("test cases/pass/array/array_object_empty.json"     , construct_array_object_empty     , one));
    print_test(name, "array [{\"a\":1}]"                         , test_parse_json("test cases/pass/array/array_object.json"           , construct_array_object           , one));
    print_test(name, "array [[{\"a\":1}, {\"b\":2}, {\"c\":3}]]" , test_parse_json("test cases/pass/array/array_objects.json"          , construct_array_objects          , one));
    print_test(name, "array [[]]"                                , test_parse_json("test cases/pass/array/array_array_empty.json"      , construct_array_array_empty      , one));
    print_test(name, "array [[[]]]"                              , test_parse_json("test cases/pass/array/array_array_array_empty.json", construct_array_array_array_empty, one));
    
    // TODO
    print_test(name, "array [[1, 2, 3],[4, 5, 6],[7, 8, 9]]"     , test_parse_json("test cases/pass/array/array_matrix.json"           , construct_array_matrix           , one));
    print_test(name, "array [[[1, 2], [3, 4]], [[5, 6], [7, 8]]]", test_parse_json("test cases/pass/array/array_tensor.json"           , construct_array_tensor           , one));

    print_final_summary();

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
    r = parse_json_value(file_buf, 0, pp_value);

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
    {
        result = 0;
        goto exit;
    }

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

    if ( a->type == JSONobject )
    {
        dict   *a_dict       = a->object,
               *b_dict       = b->object;
        size_t  a_properties = dict_values(a_dict, 0),
                b_properties = dict_values(b_dict, 0);
        char  **a_keys       = 0,
              **b_keys       = 0;
        void  **a_values     = 0,
              **b_values     = 0;
               
        if (a_properties != b_properties)
        {
            result = 0;
        }

        a_keys   = calloc(a_properties+1, sizeof(char *)),
        b_keys   = calloc(a_properties+1, sizeof(char *)),
        a_values = calloc(b_properties+1, sizeof(JSONValue_t *)),
        b_values = calloc(b_properties+1, sizeof(JSONValue_t *));
        
        dict_keys(a_dict, a_keys);
        dict_keys(b_dict, b_keys);
        dict_values(a_dict, a_values);
        dict_values(b_dict, b_values);
        
        for (size_t i = 0; i < a_properties; i++)
        {
            bool has_key   = false,
                 has_value = false;
                 
            for (size_t j = 0; j < b_properties; j++)
            {
                if (strcmp(a_keys[i], b_keys[j])==0)
                {
                    has_key = true;
                }
                if ( value_equals(a_values[i], b_values[j]) )
                {
                    has_value = true;
                }
            }

            if( ( has_key && has_value ) == 0 )
            {
                result = 0;
            }
        }
    }

    if ( a->type == JSONarray )
    {
        array        *a_array      = a->list,
                     *b_array      = b->list;
        size_t        a_properties = 0,
                      b_properties = 0;
        JSONValue_t **a_values     = 0,
                    **b_values     = 0;
        
        array_get(a_array,0,&a_properties);
        array_get(b_array,0,&b_properties);

        if (a_properties != b_properties)
        {
            result = 0;
        }

        a_values = calloc(b_properties+1, sizeof(JSONValue_t *)),
        b_values = calloc(b_properties+1, sizeof(JSONValue_t *));
        
        array_get(a_array, a_values, 0);
        array_get(b_array, b_values, 0);
        
        for (size_t i = 0; i < a_properties; i++)
        {
            bool has_value = false;
                 
            for (size_t j = 0; j < b_properties; j++)
            {
                if ( value_equals(a_values[i], b_values[j]) )
                {
                    has_value = true;
                }
            }
        }

        // Clean up
        free(a_values);
        free(b_values);
    }

    exit:
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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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
    p_value->floating = -DBL_MAX;

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_quote ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\"";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_quote_quote ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\"\"";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_whitespaces_abc ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "abc";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_reverse_solidus ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\\";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_solidus ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "/";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_backspace ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\b";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_formfeed ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\f";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_linefeed ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\n";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_carriage_return ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\r";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_horizontal_tab ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONstring;

    // Value
    p_value->string = "\t";

    // Return
    *pp_value = p_value;

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

    // Return
    *pp_value = p_value;

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
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_int  ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONinteger;
    p_abc_value->integer = 123;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_float  ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONfloat;
    p_abc_value->floating = 3.14;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "pi", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_false  ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONboolean;
    p_abc_value->boolean = false;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_true  ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONboolean;
    p_abc_value->boolean = true;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_strings ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_ghi_value = calloc(1, sizeof(JSONValue_t)),
                *p_mno_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type = JSONobject;
    p_abc_value->type = JSONstring;
    p_abc_value->string = "def";
    p_ghi_value->type = JSONstring;
    p_ghi_value->string = "jkl";
    p_mno_value->type = JSONstring;
    p_mno_value->string = "pqr";
    
    // Value
    dict_construct(&p_value->object, 3);
    dict_add(p_value->object, "abc", p_abc_value);
    dict_add(p_value->object, "ghi", p_ghi_value);
    dict_add(p_value->object, "mno", p_mno_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_mixed_values ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value        = calloc(1, sizeof(JSONValue_t)),
                *p_name_value   = calloc(1, sizeof(JSONValue_t)),
                *p_age_value    = calloc(1, sizeof(JSONValue_t)),
                *p_height_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type            = JSONobject;
    p_name_value->type       = JSONstring;
    p_name_value->string     = "jake";
    p_age_value->type        = JSONinteger;
    p_age_value->integer     = 20;
    p_height_value->type     = JSONfloat;
    p_height_value->floating = 1.779;
    
    // Value
    dict_construct(&p_value->object, 3);
    dict_add(p_value->object, "name"  , p_name_value);
    dict_add(p_value->object, "age"   , p_age_value);
    dict_add(p_value->object, "height", p_height_value);

    // Return
    *pp_value = p_value;

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
    dict_add(p_abc_value->object, "def", p_def_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_object_object ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_def_value = calloc(1, sizeof(JSONValue_t)),
                *p_ghi_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONobject;
    p_def_value->type    = JSONobject;
    p_ghi_value->type    = JSONinteger;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_construct(&p_abc_value->object, 1);
    dict_construct(&p_def_value->object, 1);

    p_ghi_value->integer = 123;
    
    dict_add(p_def_value->object, "ghi", p_ghi_value);
    dict_add(p_abc_value->object, "def", p_def_value);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_recursive       ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value      = calloc(1, sizeof(JSONValue_t)),
                *p_iter_value = calloc(1, sizeof(JSONValue_t)),
                *p_last_value = calloc(1, sizeof(JSONValue_t));
    

    p_iter_value->type = JSONobject;

    // Construct the object
    dict_construct(&p_value->object, 1);
    
    p_iter_value->type = JSONobject;
    dict_construct(&p_iter_value->object, 1);

    p_last_value = p_iter_value;
    
    // Iterate through the alphabet
    for (size_t i = 'z'; i > 'a'-1; i--)
    {

        // Initialized data
        char iter_key[2] = { (char) i, 0x0 };

        p_iter_value = calloc(1, sizeof(JSONValue_t)),

        p_iter_value->type = JSONobject;
        dict_construct(&p_iter_value->object, 1);
        dict_add(p_iter_value->object,iter_key, p_last_value);   
        p_last_value = p_iter_value;
    }
    

    // Return
    *pp_value = p_iter_value;

    // Success
    return 1;
}

int construct_object_array            ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_def_value = calloc(1, sizeof(JSONValue_t)),
                *p_ghi_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONobject;
    p_def_value->type    = JSONobject;
    p_ghi_value->type    = JSONinteger;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_construct(&p_abc_value->object, 1);
    dict_construct(&p_def_value->object, 1);

    p_ghi_value->integer = 123;
    
    dict_add(p_def_value->object, "ghi", p_ghi_value);
    dict_add(p_abc_value->object, "def", p_def_value);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_array_objects    ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_def_value = calloc(1, sizeof(JSONValue_t)),
                *p_ghi_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONobject;
    p_def_value->type    = JSONobject;
    p_ghi_value->type    = JSONinteger;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_construct(&p_abc_value->object, 1);
    dict_construct(&p_def_value->object, 1);

    p_ghi_value->integer = 123;
    
    dict_add(p_def_value->object, "ghi", p_ghi_value);
    dict_add(p_abc_value->object, "def", p_def_value);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_array_object     ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value     = calloc(1, sizeof(JSONValue_t)),
                *p_abc_value = calloc(1, sizeof(JSONValue_t)),
                *p_def_value = calloc(1, sizeof(JSONValue_t)),
                *p_ghi_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type        = JSONobject;
    p_abc_value->type    = JSONobject;
    p_def_value->type    = JSONobject;
    p_ghi_value->type    = JSONinteger;

    // Construct the object
    dict_construct(&p_value->object, 1);
    dict_construct(&p_abc_value->object, 1);
    dict_construct(&p_def_value->object, 1);

    p_ghi_value->integer = 123;
    
    dict_add(p_def_value->object, "ghi", p_ghi_value);
    dict_add(p_abc_value->object, "def", p_def_value);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_empty ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONarray;

    // Value
    array_construct(&p_value->list, 1);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_null ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONarray;

    // Value
    array_construct(&p_value->list, 1);
    array_add(p_value->list, 0);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_nulls ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t));
    
    // Type
    p_value->type    = JSONarray;

    // Value
    array_construct(&p_value->list, 3);
    array_add(p_value->list, 0);
    array_add(p_value->list, 0);
    array_add(p_value->list, 0);

    size_t i = 0;
    array_get(p_value->list, 0, &i);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_bool ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_bool  = 0;

    // Type
    p_value->type = JSONarray;

    // Value
    p_bool = calloc(1, sizeof(JSONValue_t));
    p_bool->type = JSONboolean;
    p_bool->boolean = true;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_bools ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_bool  = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_bool = calloc(1, sizeof(JSONValue_t));
    p_bool->type = JSONboolean;
    p_bool->boolean = true;
    array_add(p_value->list, p_bool);
    
    // Value 2
    p_bool = calloc(1, sizeof(JSONValue_t));
    p_bool->type = JSONboolean;
    p_bool->boolean = false;
    array_add(p_value->list, p_bool);

    // Value 3
    p_bool = calloc(1, sizeof(JSONValue_t));
    p_bool->type = JSONboolean;
    p_bool->boolean = true;
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_int ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_bool  = 0;

    // Type
    p_value->type = JSONarray;

    // Value
    p_bool = calloc(1, sizeof(JSONValue_t));
    p_bool->type = JSONinteger;
    p_bool->integer = 1;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_ints ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_int  = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_int = calloc(1, sizeof(JSONValue_t));
    p_int->type = JSONinteger;
    p_int->integer = 1;
    array_add(p_value->list, p_int);
    
    // Value 2
    p_int = calloc(1, sizeof(JSONValue_t));
    p_int->type = JSONinteger;
    p_int->integer = 2;
    array_add(p_value->list, p_int);

    // Value 3
    p_int = calloc(1, sizeof(JSONValue_t));
    p_int->type = JSONinteger;
    p_int->integer = 3;
    array_add(p_value->list, p_int);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_float ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_float  = 0;

    // Type
    p_value->type = JSONarray;

    // Value
    p_float = calloc(1, sizeof(JSONValue_t));
    p_float->type = JSONfloat;
    p_float->floating = 3.141;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_float);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_floats ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_float  = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_float = calloc(1, sizeof(JSONValue_t));
    p_float->type = JSONfloat;
    p_float->floating = 1.2;
    array_add(p_value->list, p_float);
    
    // Value 2
    p_float = calloc(1, sizeof(JSONValue_t));
    p_float->type = JSONfloat;
    p_float->floating = 3.4;
    array_add(p_value->list, p_float);

    // Value 3
    p_float = calloc(1, sizeof(JSONValue_t));
    p_float->type = JSONfloat;
    p_float->floating = 5.6;
    array_add(p_value->list, p_float);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_string_empty ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_string  = 0;

    // Type
    p_value->type = JSONarray;

    // Value
    p_string = calloc(1, sizeof(JSONValue_t));
    p_string->type = JSONstring;
    p_string->string = "";
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_string ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value = calloc(1, sizeof(JSONValue_t)),
                *p_string  = 0;

    // Type
    p_value->type = JSONarray;

    // Value
    p_string = calloc(1, sizeof(JSONValue_t));
    p_string->type = JSONstring;
    p_string->string = "abc";
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_strings ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value  = calloc(1, sizeof(JSONValue_t)),
                *p_string = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_string = calloc(1, sizeof(JSONValue_t));
    p_string->type = JSONstring;
    p_string->string = "abc";
    array_add(p_value->list, p_string);
    
    // Value 2
    p_string = calloc(1, sizeof(JSONValue_t));
    p_string->type = JSONstring;
    p_string->string = "def";
    array_add(p_value->list, p_string);

    // Value 3
    p_string = calloc(1, sizeof(JSONValue_t));
    p_string->type = JSONstring;
    p_string->string = "ghi";
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_object_empty ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value  = calloc(1, sizeof(JSONValue_t)),
                *p_object = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object = calloc(1, sizeof(JSONValue_t));
    p_object->type = JSONobject;
    dict_construct(&p_object->object, 2);
    array_add(p_value->list, p_object);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_object ( JSONValue_t **pp_value )
{
    
    // Initialized data
    JSONValue_t *p_value           = calloc(1, sizeof(JSONValue_t)),
                *p_object          = 0,
                *p_object_property = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object_property = calloc(1, sizeof(JSONValue_t));
    p_object_property->type = JSONinteger;
    p_object_property->integer = 1;

    // Value 2
    p_object = calloc(1, sizeof(JSONValue_t));
    p_object->type = JSONobject;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "a", p_object_property);
    array_add(p_value->list, p_object);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_objects ( JSONValue_t **pp_value )
{
    
    // Initialized data
    JSONValue_t *p_value           = calloc(1, sizeof(JSONValue_t)),
                *p_object          = 0,
                *p_object_property = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object_property = calloc(1, sizeof(JSONValue_t));
    p_object_property->type = JSONinteger;
    p_object_property->integer = 1;

    // Value 2
    p_object = calloc(1, sizeof(JSONValue_t));
    p_object->type = JSONobject;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "a", p_object_property);
    array_add(p_value->list, p_object);
    
    // Value 1
    p_object_property = calloc(1, sizeof(JSONValue_t));
    p_object_property->type = JSONinteger;
    p_object_property->integer = 2;

    // Value 2
    p_object = calloc(1, sizeof(JSONValue_t));
    p_object->type = JSONobject;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "b", p_object_property);
    array_add(p_value->list, p_object);

    // Value 1
    p_object_property = calloc(1, sizeof(JSONValue_t));
    p_object_property->type = JSONinteger;
    p_object_property->integer = 3;

    // Value 2
    p_object = calloc(1, sizeof(JSONValue_t));
    p_object->type = JSONobject;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "c", p_object_property);
    array_add(p_value->list, p_object);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_array_empty ( JSONValue_t **pp_value )
{
    
    // Initialized data
    JSONValue_t *p_value           = calloc(1, sizeof(JSONValue_t)),
                *p_array           = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_array = calloc(1, sizeof(JSONValue_t));
    p_array->type = JSONarray;
    array_add(p_array->list, 1);

    // Value 2
    array_add(p_value->list, p_array);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_array_array_empty ( JSONValue_t **pp_value )
{
    
    // Initialized data
    JSONValue_t *p_value           = calloc(1, sizeof(JSONValue_t)),
                *p_array           = 0,
                *p_array_array     = 0;

    // Type
    p_value->type = JSONarray;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_array_array = calloc(1, sizeof(JSONValue_t));
    p_array_array->type = JSONarray;
    array_construct(&p_array_array->list, 3);

    // Value 2
    p_array = calloc(1, sizeof(JSONValue_t));
    p_array->type = JSONarray;
    array_construct(&p_array->list, 3);
    array_add(p_array->list, p_array_array);

    // Value 2
    array_add(p_value->list, p_array);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_matrix ( JSONValue_t **pp_value )
{
    // Initialized data
    JSONValue_t *p_value           = calloc(1, sizeof(JSONValue_t)),
                *p_array           = 0,
                *p_object_property = 0;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_tensor ( JSONValue_t **pp_value )
{
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
    result_t     result   = 0;
    char        *file_buf = 0;

    if (expected_value_constructor)
        expected_value_constructor(&p_expected_value);

    result = load_json ( &p_return_value, test_file );

    // Success
    return (result == expected && value_equals(p_return_value, p_expected_value));
}

int print_test ( const char *scenario_name, const char *test_name, bool passed )
{

    // Initialized data
    printf("%s_test_%-75s %s\n",scenario_name, test_name, (passed) ? "PASS" : "FAIL");

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
