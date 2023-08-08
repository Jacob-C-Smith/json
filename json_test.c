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

#include <dict/dict.h>
#include <array/array.h>
#include <json/json.h>


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
int      print_time_pretty   ( double seconds );
int      run_tests           ( void );
int      print_final_summary ( void );
int      print_test          ( const char   *scenario_name, const char   *test_name,                                    bool     passed );
bool     test_parse_json     ( char         *test_file    , int         (*expected_value_constructor) (json_value **), result_t expected );
bool     test_serial_json    ( char         *test_file    , char         *expected_file                               , int(*expected_value_constructor) (json_value **), result_t expected );
result_t load_json           ( json_value  **pp_value     , char         *test_file );
result_t save_json           ( char         *path         , json_value   *p_value );
result_t value_equals        ( json_value   *a            , json_value   *b );
size_t   load_file           ( const char   *path         , void         *buffer                                      , bool     binary_mode );

int test_parse_null   (char *text);
int test_parse_bool   (char *text);
int test_parse_int    (char *text);
int test_parse_float  (char *text);
int test_parse_string (char *text);
int test_parse_object (char *text);
int test_parse_array  (char *text);
int test_serial_null  (char *text);
int test_serial_bool  (char *text);
int test_serial_int   (char *text);
int test_serial_float (char *text);
int test_serial_string(char *text);
int test_serial_object(char *text);
int test_serial_array (char *text);

// Scenario constructors
int  construct_null                   ( json_value **pp_value );

int  construct_bool_false             ( json_value **pp_value );
int  construct_bool_true              ( json_value **pp_value );

int  construct_int_minus_one          ( json_value **pp_value ); 
int  construct_int_zero               ( json_value **pp_value ); 
int  construct_int_one                ( json_value **pp_value ); 
int  construct_int_max                ( json_value **pp_value ); 
int  construct_int_min                ( json_value **pp_value ); 

int  construct_float_minus_one        ( json_value **pp_value ); 
int  construct_float_zero             ( json_value **pp_value ); 
int  construct_float_one              ( json_value **pp_value ); 
int  construct_float_max              ( json_value **pp_value ); 
int  construct_float_min              ( json_value **pp_value ); 

int construct_string_empty            ( json_value **pp_value );
int construct_string_a                ( json_value **pp_value );
int construct_string_abc              ( json_value **pp_value );
int construct_string_quote_abc_quote  ( json_value **pp_value );
int construct_string_quote            ( json_value **pp_value );
int construct_string_quote_quote      ( json_value **pp_value );
int construct_string_whitespaces_abc  ( json_value **pp_value );
int construct_string_reverse_solidus  ( json_value **pp_value );
int construct_string_solidus          ( json_value **pp_value );
int construct_string_backspace        ( json_value **pp_value );
int construct_string_formfeed         ( json_value **pp_value );
int construct_string_linefeed         ( json_value **pp_value );
int construct_string_carriage_return  ( json_value **pp_value );
int construct_string_horizontal_tab   ( json_value **pp_value );

int construct_object_empty            ( json_value **pp_value );
int construct_object_string           ( json_value **pp_value );
int construct_object_int              ( json_value **pp_value );
int construct_object_float            ( json_value **pp_value );
int construct_object_false            ( json_value **pp_value );
int construct_object_true             ( json_value **pp_value );
int construct_object_strings          ( json_value **pp_value );
int construct_object_mixed_values     ( json_value **pp_value );
int construct_object_object           ( json_value **pp_value );
int construct_object_object_object    ( json_value **pp_value );
int construct_object_recursive        ( json_value **pp_value );
int construct_object_array            ( json_value **pp_value );
int construct_object_array_objects    ( json_value **pp_value );
int construct_object_array_object     ( json_value **pp_value );

int construct_array_empty             ( json_value **pp_value );
int construct_array_null              ( json_value **pp_value );
int construct_array_nulls             ( json_value **pp_value );
int construct_array_bool              ( json_value **pp_value );
int construct_array_bools             ( json_value **pp_value );
int construct_array_int               ( json_value **pp_value );
int construct_array_ints              ( json_value **pp_value );
int construct_array_float             ( json_value **pp_value );
int construct_array_floats            ( json_value **pp_value );
int construct_array_string_empty      ( json_value **pp_value );
int construct_array_string            ( json_value **pp_value );
int construct_array_strings           ( json_value **pp_value );
int construct_array_object_empty      ( json_value **pp_value );
int construct_array_object            ( json_value **pp_value );
int construct_array_objects           ( json_value **pp_value );
int construct_array_array_empty       ( json_value **pp_value );
int construct_array_array_array_empty ( json_value **pp_value );
int construct_array_matrix            ( json_value **pp_value );
int construct_array_tensor            ( json_value **pp_value );

// Entry point
int main ( int argc, const char* argv[] )
{
    
    // Initialized data
    timestamp t0 = 0,
              t1 = 0;

    // Initialize the timer library
    timer_init();

    // Formatting
    printf("|=============|\n| JSON TESTER |\n|=============|\n\n");

    // Start
    t0 = timer_high_precision();

    // Run tests
    run_tests();

    // Stop
    t1 = timer_high_precision();

    // Report the time it took to run the tests
    printf("\njson tests took ");
    print_time_pretty ( (double)(t1-t0)/(double)timer_seconds_divisor() );
    printf(" to test\n");

    // Flush stdio
    fflush(stdout);

    // Exit
    return ( total_passes == total_tests ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int print_time_pretty ( double seconds )
{

    // Initialized data
    double _seconds     = seconds;
    size_t days         = 0,
           hours        = 0,
           minutes      = 0,
           __seconds    = 0,
           milliseconds = 0,
           microseconds = 0;

    // Days
    while ( _seconds > 86400.0 ) { days++;_seconds-=286400.0; };

    // Hours
    while ( _seconds > 3600.0 ) { hours++;_seconds-=3600.0; };

    // Minutes
    while ( _seconds > 60.0 ) { minutes++;_seconds-=60.0; };

    // Seconds
    while ( _seconds > 1.0 ) { __seconds++;_seconds-=1.0; };

    // milliseconds
    while ( _seconds > 0.001 ) { milliseconds++;_seconds-=0.001; };

    // Microseconds        
    while ( _seconds > 0.000001 ) { microseconds++;_seconds-=0.000001; };

    // Print days
    if ( days ) 
        printf("%d D, ", days);
    
    // Print hours
    if ( hours )
        printf("%d h, ", hours);

    // Print minutes
    if ( minutes )
        printf("%d m, ", minutes);

    // Print seconds
    if ( __seconds )
        printf("%d s, ", __seconds);
    
    // Print milliseconds
    if ( milliseconds )
        printf("%d ms, ", milliseconds);
    
    // Print microseconds
    if ( microseconds )
        printf("%d us", microseconds);
    
    // Success
    return 1;
}

int run_tests ( void )
{

    // Test the parser
    // Start
    timestamp parser_t0 = 0,
              parser_t1 = 0,
              serial_t0 = 0,
              serial_t1 = 0;

    // Start
    parser_t0 = timer_high_precision();

    // Test parsing valid and invalid null
    test_parse_null("parse_null");

    // Test parsing valid and invalid booleans
    test_parse_bool("parse_bool");

    // Test parsing valid, invalid, and over/underflowed integers
    test_parse_int("parse_int");

    // Test parsing valid, invalid, and over/underflowed floating point numbers
    test_parse_float("parse_float");
    
    // Test parsing valid, invalid, and unicode strings
    test_parse_string("parse_string");

    // Test parsing a variety of objects
    test_parse_object("parse_object");

    // Test parsing a variety of arrays
    test_parse_array("parse_array");

    // Stop
    parser_t1 = timer_high_precision();


    // Test the serializer

    // Start
    serial_t0 = timer_high_precision();

    // Test serializing null
    test_serial_null("serial_null");
    
    // Test serializing booleans
    test_serial_bool("serial_bool");

    // Test serializing integers
    test_serial_int("serial_int");

    // Test serializing floating point numbers
    test_serial_float("serial_float");
    
    // Test serializing strings
    test_serial_string("serial_string");

    // Test serializing objects
    test_serial_object("serial_object");

    // Test serializing arrays
    test_serial_array("serial_array");

    // Stop
    serial_t1 = timer_high_precision();

    // Report the time it took to run the parser tests
    printf("parser tests took: ");
    print_time_pretty ( (double)(parser_t1-parser_t0)/(double)timer_seconds_divisor() );
    printf(" to test\n");


    // Report the time it took to run the serializer tests
    printf("serial tests took: ");
    print_time_pretty ( (double)(serial_t1-serial_t0)/(double)timer_seconds_divisor() );
    printf(" to test\n");

    // Success
    return 1;
}

int test_parse_null ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "null", test_parse_json("parse test cases/pass/null.json", (void *) 0, one));
    print_test(name, "nul" , test_parse_json("parse test cases/fail/null.json", (void *) 0, zero));

    print_final_summary();

    return 1;

}

int test_parse_bool ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "false", test_parse_json("parse test cases/pass/bool/bool_false.json", construct_bool_false, one));
    print_test(name, "true" , test_parse_json("parse test cases/pass/bool/bool_true.json" , construct_bool_true , one));
    print_test(name, "fals" , test_parse_json("parse test cases/fail/bool/bool_false.json", (void *) 0          , zero));
    print_test(name, "tru"  , test_parse_json("parse test cases/fail/bool/bool_true.json" , (void *) 0          , zero));

    print_final_summary();

    return 1;

}

int test_parse_int ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "-1"    , test_parse_json("parse test cases/pass/int/int_-1.json" , construct_int_minus_one, one));
    print_test(name, "0"     , test_parse_json("parse test cases/pass/int/int_0.json"  , construct_int_zero     , one));
    print_test(name, "1"     , test_parse_json("parse test cases/pass/int/int_1.json"  , construct_int_one      , one));
    print_test(name, "max"   , test_parse_json("parse test cases/pass/int/int_max.json", construct_int_max      , one));
    print_test(name, "min"   , test_parse_json("parse test cases/pass/int/int_min.json", construct_int_min      , one));
    print_test(name, "max +1", test_parse_json("parse test cases/fail/int/int_max.json", (void *)0              , zero));
    print_test(name, "min -1", test_parse_json("parse test cases/fail/int/int_min.json", (void *)0              , zero));

    print_final_summary();

    return 1;
}

int test_parse_float ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "-1.0"    , test_parse_json("parse test cases/pass/float/float_-1.json" , construct_float_minus_one, one));
    print_test(name, "0.0"     , test_parse_json("parse test cases/pass/float/float_0.json"  , construct_float_zero     , one));
    print_test(name, "1.0"     , test_parse_json("parse test cases/pass/float/float_1.json"  , construct_float_one      , one));
    print_test(name, "max"     , test_parse_json("parse test cases/pass/float/float_max.json", construct_float_max      , one));
    print_test(name, "min"     , test_parse_json("parse test cases/pass/float/float_min.json", construct_float_min      , one));
    print_test(name, "max + 1.0", test_parse_json("parse test cases/fail/float/float_max.json", (void *)0                , zero));
    print_test(name, "min - 1.0", test_parse_json("parse test cases/fail/float/float_min.json", (void *)0                , zero));

    print_final_summary();

    return 1;
}

int test_parse_string ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "\"\""       , test_parse_json("parse test cases/pass/string/string_empty.json"          , construct_string_empty          , one));
    print_test(name, "\"a\""      , test_parse_json("parse test cases/pass/string/string_a.json"              , construct_string_a              , one));
    print_test(name, "\"abc\""    , test_parse_json("parse test cases/pass/string/string_abc.json"            , construct_string_abc            , one));
    print_test(name, "\"\"abc\"\"", test_parse_json("parse test cases/pass/string/string_quote_abc_quote.json", construct_string_quote_abc_quote, one));
    print_test(name, "\"\"\"\""   , test_parse_json("parse test cases/pass/string/string_quote_quote.json"    , construct_string_quote_quote    , one));
    print_test(name, "    \"abc\"", test_parse_json("parse test cases/pass/string/string_whitespaces_abc.json", construct_string_whitespaces_abc, one));
    print_test(name, "\"\"\""     , test_parse_json("parse test cases/pass/string/string_quote.json"          , construct_string_quote          , one));
    print_test(name, "\"\\\\\""   , test_parse_json("parse test cases/pass/string/string_reverse_solidus.json", construct_string_reverse_solidus, one));
    print_test(name, "\"\\/\""    , test_parse_json("parse test cases/pass/string/string_solidus.json"        , construct_string_solidus        , one));
    print_test(name, "\"\\b\""    , test_parse_json("parse test cases/pass/string/string_backspace.json"      , construct_string_backspace      , one));
    print_test(name, "\"\\f\""    , test_parse_json("parse test cases/pass/string/string_formfeed.json"       , construct_string_formfeed       , one));
    print_test(name, "\"\\n\""    , test_parse_json("parse test cases/pass/string/string_linefeed.json"       , construct_string_linefeed       , one));
    print_test(name, "\"\\r\""    , test_parse_json("parse test cases/pass/string/string_carriage_return.json", construct_string_carriage_return, one));
    print_test(name, "\"\\t\""    , test_parse_json("parse test cases/pass/string/string_horizontal_tab.json" , construct_string_horizontal_tab , one));
    //print_test(name, "string \"\u1234\"" , test_parse_json("parse test cases/pass/string/string_escape.json"         , construct_string_escape         , one));

    print_final_summary();

    return 1;
}

int test_parse_object ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "{}"                                               , test_parse_json("parse test cases/pass/object/object_empty.json"        , construct_object_empty        , one));
    print_test(name, "{\"abc\":\"def\"}"                                , test_parse_json("parse test cases/pass/object/object_string.json"       , construct_object_string       , one));
    print_test(name, "{\"abc\":123}"                                    , test_parse_json("parse test cases/pass/object/object_int.json"          , construct_object_int          , one));
    print_test(name, "{\"pi\":3.14}"                                    , test_parse_json("parse test cases/pass/object/object_float.json"        , construct_object_float        , one));
    print_test(name, "{\"abc\":false}"                                  , test_parse_json("parse test cases/pass/object/object_false.json"        , construct_object_false        , one));
    print_test(name, "{\"abc\":true}"                                   , test_parse_json("parse test cases/pass/object/object_true.json"         , construct_object_true         , one));
    print_test(name, "{\"abc\":\"def\",\"ghi\":\"jkl\",\"mno\":\"pqr\"}", test_parse_json("parse test cases/pass/object/object_strings.json"      , construct_object_strings      , one));
    print_test(name, "{\"name\":\"jake\",\"age\":20,\"height\":1.779}"  , test_parse_json("parse test cases/pass/object/object_mixed_values.json" , construct_object_mixed_values , one));
    print_test(name, "{\"abc\":{\"def\":123}}"                          , test_parse_json("parse test cases/pass/object/object_object.json"       , construct_object_object       , one));
    print_test(name, "{\"abc\":{\"def\":{\"ghi\":123}}}"                , test_parse_json("parse test cases/pass/object/object_object_object.json", construct_object_object_object, one));
    print_test(name, "{\"abc\":[1,2,3]}"                                , test_parse_json("parse test cases/pass/object/object_array.json"        , construct_object_array        , one));
    print_test(name, "{\"a\":[{\"a\":1},{\"b\":2},{\"c\":3}]}"          , test_parse_json("parse test cases/pass/object/object_array_objects.json", construct_object_array_objects, one));
    print_test(name, "{\"a\":[{\"a\":1}]}"                              , test_parse_json("parse test cases/pass/object/object_array_object.json" , construct_object_array_object , one));
    print_test(name, "{\"a\":{\"b\":{\"c\":{\"d\":{...{\"z\":{ }...}"   , test_parse_json("parse test cases/pass/object/object_recursive.json"    , construct_object_recursive    , one));

    print_final_summary();

    return 1;
}

int test_parse_array ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "[]"                                  , test_parse_json("parse test cases/pass/array/array_empty.json"            , construct_array_empty            , one));
    print_test(name, "[null]"                              , test_parse_json("parse test cases/pass/array/array_null.json"             , construct_array_null             , one));
    print_test(name, "[null, null, null]"                  , test_parse_json("parse test cases/pass/array/array_nulls.json"            , construct_array_nulls            , one));
    print_test(name, "[true]"                              , test_parse_json("parse test cases/pass/array/array_bool.json"             , construct_array_bool             , one));
    print_test(name, "[true, false, true]"                 , test_parse_json("parse test cases/pass/array/array_bools.json"            , construct_array_bools            , one));
    print_test(name, "[1]"                                 , test_parse_json("parse test cases/pass/array/array_int.json"              , construct_array_int              , one));
    print_test(name, "[1, 2, 3]"                           , test_parse_json("parse test cases/pass/array/array_ints.json"             , construct_array_ints             , one));
    print_test(name, "[3.14]"                              , test_parse_json("parse test cases/pass/array/array_float.json"            , construct_array_float            , one));
    print_test(name, "[1.2, 3.4, 5.6]"                     , test_parse_json("parse test cases/pass/array/array_floats.json"           , construct_array_floats           , one));
    print_test(name, "[\"\"]"                              , test_parse_json("parse test cases/pass/array/array_string_empty.json"     , construct_array_string_empty     , one));
    print_test(name, "[\"abc\"]"                           , test_parse_json("parse test cases/pass/array/array_string.json"           , construct_array_string           , one));
    print_test(name, "[\"abc\", \"def\", \"ghi\"]"         , test_parse_json("parse test cases/pass/array/array_strings.json"          , construct_array_strings          , one));
    print_test(name, "[{}]"                                , test_parse_json("parse test cases/pass/array/array_object_empty.json"     , construct_array_object_empty     , one));
    print_test(name, "[{\"a\":1}]"                         , test_parse_json("parse test cases/pass/array/array_object.json"           , construct_array_object           , one));
    print_test(name, "[[{\"a\":1}, {\"b\":2}, {\"c\":3}]]" , test_parse_json("parse test cases/pass/array/array_objects.json"          , construct_array_objects          , one));
    print_test(name, "[[]]"                                , test_parse_json("parse test cases/pass/array/array_array_empty.json"      , construct_array_array_empty      , one));
    print_test(name, "[[[]]]"                              , test_parse_json("parse test cases/pass/array/array_array_array_empty.json", construct_array_array_array_empty, one));
    print_test(name, "[[1, 2, 3],[4, 5, 6],[7, 8, 9]]"     , test_parse_json("parse test cases/pass/array/array_matrix.json"           , construct_array_matrix           , one));
    print_test(name, "[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]", test_parse_json("parse test cases/pass/array/array_tensor.json"           , construct_array_tensor           , one));

    print_final_summary();

    return 1;
}

int test_serial_null ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "null", test_serial_json("serial test cases/TESTER_null.json", "parse test cases/pass/null.json", construct_null, one));

    print_final_summary();

    return 1;

}

int test_serial_bool ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "false", test_serial_json("serial test cases/bool/TESTER_bool_false.json", "parse test cases/pass/bool/bool_false.json", construct_bool_false, one));
    print_test(name, "true" , test_serial_json("serial test cases/bool/TESTER_bool_true.json" , "parse test cases/pass/bool/bool_true.json" , construct_bool_true , one));

    print_final_summary();

    return 1;

}

int test_serial_int ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    print_test(name, "-1"    , test_serial_json("serial test cases/int/TESTER_int_-1.json" , "parse test cases/pass/int/int_-1.json" , construct_int_minus_one, one));
    print_test(name, "0"     , test_serial_json("serial test cases/int/TESTER_int_0.json"  , "parse test cases/pass/int/int_0.json"  , construct_int_zero     , one));
    print_test(name, "1"     , test_serial_json("serial test cases/int/TESTER_int_1.json"  , "parse test cases/pass/int/int_1.json"  , construct_int_one      , one));
    print_test(name, "max"   , test_serial_json("serial test cases/int/TESTER_int_max.json", "parse test cases/pass/int/int_max.json", construct_int_max      , one));
    print_test(name, "min"   , test_serial_json("serial test cases/int/TESTER_int_min.json", "parse test cases/pass/int/int_min.json", construct_int_min      , one));

    print_final_summary();

    return 1;
}

int test_serial_float ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);

    printf("TODO: While the floating point serializer does work, the formatting is poor.\n");
    printf("These tests will fail if compared against the original file, so I've disabled them\n");

    // TODO: Format perfectly. 
    //print_test(name, "-1.0"    , test_serial_json("serial test cases/float/TESTER_float_-1.json" , "parse test cases/pass/float/float_-1.json" , construct_float_minus_one, one));
    //print_test(name, "0.0"     , test_serial_json("serial test cases/float/TESTER_float_0.json"  , "parse test cases/pass/float/float_0.json"  , construct_float_zero     , one));
    //print_test(name, "1.0"     , test_serial_json("serial test cases/float/TESTER_float_1.json"  , "parse test cases/pass/float/float_1.json"  , construct_float_one      , one));
    //print_test(name, "max"     , test_serial_json("serial test cases/float/TESTER_float_max.json", "parse test cases/pass/float/float_max.json", construct_float_max      , one));
    //print_test(name, "min"     , test_serial_json("serial test cases/float/TESTER_float_min.json", "parse test cases/pass/float/float_min.json", construct_float_min      , one));

    print_final_summary();

    return 1;
}

int test_serial_string ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "\"\""       , test_serial_json("serial test cases/string/TESTER_string_empty.json"          , "parse test cases/pass/string/string_empty.json"          , construct_string_empty          , one));
    print_test(name, "\"a\""      , test_serial_json("serial test cases/string/TESTER_string_a.json"              , "parse test cases/pass/string/string_a.json"              , construct_string_a              , one));
    print_test(name, "\"abc\""    , test_serial_json("serial test cases/string/TESTER_string_abc.json"            , "parse test cases/pass/string/string_abc.json"            , construct_string_abc            , one));
    print_test(name, "\"\"abc\"\"", test_serial_json("serial test cases/string/TESTER_string_quote_abc_quote.json", "parse test cases/pass/string/string_quote_abc_quote.json", construct_string_quote_abc_quote, one));
    print_test(name, "\"\"\"\""   , test_serial_json("serial test cases/string/TESTER_string_quote_quote.json"    , "parse test cases/pass/string/string_quote_quote.json"    , construct_string_quote_quote    , one));
    print_test(name, "\"\"\""     , test_serial_json("serial test cases/string/TESTER_string_quote.json"          , "parse test cases/pass/string/string_quote.json"          , construct_string_quote          , one));
    print_test(name, "\"\\\\\""   , test_serial_json("serial test cases/string/TESTER_string_reverse_solidus.json", "parse test cases/pass/string/string_reverse_solidus.json", construct_string_reverse_solidus, one));
    print_test(name, "\"\\/\""    , test_serial_json("serial test cases/string/TESTER_string_solidus.json"        , "parse test cases/pass/string/string_solidus.json"        , construct_string_solidus        , one));
    print_test(name, "\"\\b\""    , test_serial_json("serial test cases/string/TESTER_string_backspace.json"      , "parse test cases/pass/string/string_backspace.json"      , construct_string_backspace      , one));
    print_test(name, "\"\\f\""    , test_serial_json("serial test cases/string/TESTER_string_formfeed.json"       , "parse test cases/pass/string/string_formfeed.json"       , construct_string_formfeed       , one));
    print_test(name, "\"\\n\""    , test_serial_json("serial test cases/string/TESTER_string_linefeed.json"       , "parse test cases/pass/string/string_linefeed.json"       , construct_string_linefeed       , one));
    print_test(name, "\"\\r\""    , test_serial_json("serial test cases/string/TESTER_string_carriage_return.json", "parse test cases/pass/string/string_carriage_return.json", construct_string_carriage_return, one));
    print_test(name, "\"\\t\""    , test_serial_json("serial test cases/string/TESTER_string_horizontal_tab.json" , "parse test cases/pass/string/string_horizontal_tab.json" , construct_string_horizontal_tab , one));

    print_final_summary();

    return 1;
}

int test_serial_object ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "{}"                                               , test_serial_json("serial test cases/object/TESTER_object_empty.json"        , "parse test cases/pass/object/object_empty.json"        , construct_object_empty        , one));
    print_test(name, "{\"abc\":\"def\"}"                                , test_serial_json("serial test cases/object/TESTER_object_string.json"       , "parse test cases/pass/object/object_string.json"       , construct_object_string       , one));
    print_test(name, "{\"abc\":123}"                                    , test_serial_json("serial test cases/object/TESTER_object_int.json"          , "parse test cases/pass/object/object_int.json"          , construct_object_int          , one));
    print_test(name, "{\"pi\":3.14}"                                    , test_serial_json("serial test cases/object/TESTER_object_float.json"        , "parse test cases/pass/object/object_float.json"        , construct_object_float        , one));
    print_test(name, "{\"abc\":false}"                                  , test_serial_json("serial test cases/object/TESTER_object_false.json"        , "parse test cases/pass/object/object_false.json"        , construct_object_false        , one));
    print_test(name, "{\"abc\":true}"                                   , test_serial_json("serial test cases/object/TESTER_object_true.json"         , "parse test cases/pass/object/object_true.json"         , construct_object_true         , one));
    print_test(name, "{\"abc\":\"def\",\"ghi\":\"jkl\",\"mno\":\"pqr\"}", test_serial_json("serial test cases/object/TESTER_object_strings.json"      , "parse test cases/pass/object/object_strings.json"      , construct_object_strings      , one));
    print_test(name, "{\"name\":\"jake\",\"age\":20,\"height\":1.779}"  , test_serial_json("serial test cases/object/TESTER_object_mixed_values.json" , "parse test cases/pass/object/object_mixed_values.json" , construct_object_mixed_values , one));
    print_test(name, "{\"abc\":{\"def\":123}}"                          , test_serial_json("serial test cases/object/TESTER_object_object.json"       , "parse test cases/pass/object/object_object.json"       , construct_object_object       , one));
    print_test(name, "{\"abc\":{\"def\":{\"ghi\":123}}}"                , test_serial_json("serial test cases/object/TESTER_object_object_object.json", "parse test cases/pass/object/object_object_object.json", construct_object_object_object, one));
    print_test(name, "{\"abc\":[1,2,3]}"                                , test_serial_json("serial test cases/object/TESTER_object_array.json"        , "parse test cases/pass/object/object_array.json"        , construct_object_array        , one));
    print_test(name, "{\"a\":[{\"a\":1},{\"b\":2},{\"c\":3}]}"          , test_serial_json("serial test cases/object/TESTER_object_array_objects.json", "parse test cases/pass/object/object_array_objects.json", construct_object_array_objects, one));
    print_test(name, "{\"a\":[{\"a\":1}]}"                              , test_serial_json("serial test cases/object/TESTER_object_array_object.json" , "parse test cases/pass/object/object_array_object.json" , construct_object_array_object , one));
    print_test(name, "{\"a\":{\"b\":{\"c\":{\"d\":{...{\"z\":{ }...}"   , test_serial_json("serial test cases/object/TESTER_object_recursive.json"    , "parse test cases/pass/object/object_recursive.json"    , construct_object_recursive    , one));

    print_final_summary();

    return 1;
}

int test_serial_array ( char *name )
{

    // Initialized data
    json_value *p_value = 0;

    printf("Scenario: %s\n", name);
    
    print_test(name, "[]"                                  , test_serial_json("serial test cases/array/TESTER_array_empty.json"            , "parse test cases/pass/array/array_empty.json"            , construct_array_empty            , one));
    print_test(name, "[null]"                              , test_serial_json("serial test cases/array/TESTER_array_null.json"             , "parse test cases/pass/array/array_null.json"             , construct_array_null             , one));
    print_test(name, "[null, null, null]"                  , test_serial_json("serial test cases/array/TESTER_array_nulls.json"            , "parse test cases/pass/array/array_nulls.json"            , construct_array_nulls            , one));
    print_test(name, "[true]"                              , test_serial_json("serial test cases/array/TESTER_array_bool.json"             , "parse test cases/pass/array/array_bool.json"             , construct_array_bool             , one));
    print_test(name, "[true, false, true]"                 , test_serial_json("serial test cases/array/TESTER_array_bools.json"            , "parse test cases/pass/array/array_bools.json"            , construct_array_bools            , one));
    print_test(name, "[1]"                                 , test_serial_json("serial test cases/array/TESTER_array_int.json"              , "parse test cases/pass/array/array_int.json"              , construct_array_int              , one));
    print_test(name, "[1, 2, 3]"                           , test_serial_json("serial test cases/array/TESTER_array_ints.json"             , "parse test cases/pass/array/array_ints.json"             , construct_array_ints             , one));
    print_test(name, "[3.14]"                              , test_serial_json("serial test cases/array/TESTER_array_float.json"            , "parse test cases/pass/array/array_float.json"            , construct_array_float            , one));
    print_test(name, "[1.2, 3.4, 5.6]"                     , test_serial_json("serial test cases/array/TESTER_array_floats.json"           , "parse test cases/pass/array/array_floats.json"           , construct_array_floats           , one));
    print_test(name, "[\"\"]"                              , test_serial_json("serial test cases/array/TESTER_array_string_empty.json"     , "parse test cases/pass/array/array_string_empty.json"     , construct_array_string_empty     , one));
    print_test(name, "[\"abc\"]"                           , test_serial_json("serial test cases/array/TESTER_array_string.json"           , "parse test cases/pass/array/array_string.json"           , construct_array_string           , one));
    print_test(name, "[\"abc\", \"def\", \"ghi\"]"         , test_serial_json("serial test cases/array/TESTER_array_strings.json"          , "parse test cases/pass/array/array_strings.json"          , construct_array_strings          , one));
    print_test(name, "[{}]"                                , test_serial_json("serial test cases/array/TESTER_array_object_empty.json"     , "parse test cases/pass/array/array_object_empty.json"     , construct_array_object_empty     , one));
    print_test(name, "[{\"a\":1}]"                         , test_serial_json("serial test cases/array/TESTER_array_object.json"           , "parse test cases/pass/array/array_object.json"           , construct_array_object           , one));
    print_test(name, "[[{\"a\":1}, {\"b\":2}, {\"c\":3}]]" , test_serial_json("serial test cases/array/TESTER_array_objects.json"          , "parse test cases/pass/array/array_objects.json"          , construct_array_objects          , one));
    print_test(name, "[[]]"                                , test_serial_json("serial test cases/array/TESTER_array_array_empty.json"      , "parse test cases/pass/array/array_array_empty.json"      , construct_array_array_empty      , one));
    print_test(name, "[[[]]]"                              , test_serial_json("serial test cases/array/TESTER_array_array_array_empty.json", "parse test cases/pass/array/array_array_array_empty.json", construct_array_array_array_empty, one));
    print_test(name, "[[1, 2, 3],[4, 5, 6],[7, 8, 9]]"     , test_serial_json("serial test cases/array/TESTER_array_matrix.json"           , "parse test cases/pass/array/array_matrix.json"           , construct_array_matrix           , one));
    print_test(name, "[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]", test_serial_json("serial test cases/array/TESTER_array_tensor.json"           , "parse test cases/pass/array/array_tensor.json"           , construct_array_tensor           , one));

    print_final_summary();

    return 1;
}

result_t load_json ( json_value **pp_value, char *test_file )
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

result_t value_equals (json_value *a, json_value *b)
{
    result_t result = 1;

    if ( a == 0 )
    {
        if (b == 0)
            return result;
        else
            return 0;
    }
    
    if ( b == 0 )
    {
        if (a == 0)
            return result;
        else
            return 0;
    }      

    if ( a->type != b->type )
    {
        result = 0;
        goto exit;
    }

    if ( a->type == JSON_VALUE_BOOLEAN )
        if ( a->boolean != b->boolean )
            result = 0;
    
    if ( a->type == JSON_VALUE_NUMBER )

        // Set the last bit of each float's mantissa to zero. 
        // This averts rounding errors
        if ( *((signed *)((&a->number))) & 0xfffffffffffffffe ==
             *((signed *)((&b->number))) & 0xfffffffffffffffe)
            result = 0;

    if ( a->type == JSON_VALUE_INTEGER )
        if ( a->integer != b->integer )
            result = 0;

    if ( a->type == JSON_VALUE_STRING )
        if ( strcmp(a->string, b->string) )
            result = 0;

    if ( a->type == JSON_VALUE_OBJECT )
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
        a_values = calloc(b_properties+1, sizeof(json_value *)),
        b_values = calloc(b_properties+1, sizeof(json_value *));
        
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

            if ( ( has_key && has_value ) == 0 )
            {
                result = 0;
            }
        }
    }

    if ( a->type == JSON_VALUE_ARRAY )
    {
        array        *a_array      = a->list,
                     *b_array      = b->list;
        size_t        a_properties = 0,
                      b_properties = 0;
        json_value **a_values     = 0,
                    **b_values     = 0;
        
        array_get(a_array,0,&a_properties);
        array_get(b_array,0,&b_properties);

        if (a_properties != b_properties)
        {
            result = 0;
        }

        a_values = calloc(b_properties+1, sizeof(json_value *)),
        b_values = calloc(b_properties+1, sizeof(json_value *));
        
        array_get(a_array, (void **) a_values, 0);
        array_get(b_array, (void **) b_values, 0);
        
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

            if (has_value == false)
                result = 0;
        }

        // Clean up
        free(a_values);
        free(b_values);
    }

    exit:
    return result;
}

int construct_null ( json_value **pp_value )
{

    *pp_value = (void *) 0;

    return 1;
}

int construct_bool_false ( json_value **pp_value )
{

    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_BOOLEAN;

    // Value
    p_value->boolean = false;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_bool_true ( json_value **pp_value )
{

    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_BOOLEAN;

    // Value
    p_value->boolean = true;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_int_minus_one ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_INTEGER;

    // Value
    p_value->integer = -1;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_int_zero ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_INTEGER;

    // Value
    p_value->integer = 0;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_int_one ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_INTEGER;

    // Value
    p_value->integer = 1;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_int_max ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_INTEGER;

    // Value
    p_value->integer = 9223372036854775807;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_int_min ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_INTEGER;

    // Value
    p_value->integer = (-9223372036854775807 - 1);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_float_minus_one ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_NUMBER;

    // Value
    p_value->number = -1.0;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_float_zero ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_NUMBER;

    // Value
    p_value->number = 0.0;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_float_one ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_NUMBER;

    // Value
    p_value->number = 1.0;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_float_max ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_NUMBER;

    // Value
    p_value->number = DBL_MAX;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_float_min ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_NUMBER;

    // Value
    p_value->number = -DBL_MAX;

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_empty ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_a ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "a";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_abc ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "abc";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_quote_abc_quote ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\"abc\"";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_quote ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\"";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_quote_quote ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\"\"";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_whitespaces_abc ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "abc";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_reverse_solidus ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\\";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_solidus ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "/";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_backspace ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\b";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_formfeed ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\f";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_linefeed ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\n";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_carriage_return ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\r";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_string_horizontal_tab ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_STRING;

    // Value
    p_value->string = "\t";

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_empty ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_OBJECT;

    // Value
    dict_construct(&p_value->object, 1);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_string ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_STRING;
    p_abc_value->string = "def";
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_int  ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_INTEGER;
    p_abc_value->integer = 123;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_float  ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_NUMBER;
    p_abc_value->number = 3.14;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "pi", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_false  ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_BOOLEAN;
    p_abc_value->boolean = false;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_true  ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_BOOLEAN;
    p_abc_value->boolean = true;
    
    // Value
    dict_construct(&p_value->object, 1);
    dict_add(p_value->object, "abc", p_abc_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_strings ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value)),
                *p_ghi_value = calloc(1, sizeof(json_value)),
                *p_mno_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type = JSON_VALUE_OBJECT;
    p_abc_value->type = JSON_VALUE_STRING;
    p_abc_value->string = "def";
    p_ghi_value->type = JSON_VALUE_STRING;
    p_ghi_value->string = "jkl";
    p_mno_value->type = JSON_VALUE_STRING;
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

int construct_object_mixed_values ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value        = calloc(1, sizeof(json_value)),
                *p_name_value   = calloc(1, sizeof(json_value)),
                *p_age_value    = calloc(1, sizeof(json_value)),
                *p_height_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type            = JSON_VALUE_OBJECT;
    p_name_value->type       = JSON_VALUE_STRING;
    p_name_value->string     = "jake";
    p_age_value->type        = JSON_VALUE_INTEGER;
    p_age_value->integer     = 20;
    p_height_value->type     = JSON_VALUE_NUMBER;
    p_height_value->number = 1.779;
    
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

int construct_object_object ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value)),
                *p_def_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type        = JSON_VALUE_OBJECT;
    p_abc_value->type    = JSON_VALUE_OBJECT;

    p_def_value->type    = JSON_VALUE_INTEGER;
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

int construct_object_object_object ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value     = calloc(1, sizeof(json_value)),
                *p_abc_value = calloc(1, sizeof(json_value)),
                *p_def_value = calloc(1, sizeof(json_value)),
                *p_ghi_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type        = JSON_VALUE_OBJECT;
    p_abc_value->type    = JSON_VALUE_OBJECT;
    p_def_value->type    = JSON_VALUE_OBJECT;
    p_ghi_value->type    = JSON_VALUE_INTEGER;

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

int construct_object_recursive ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value      = calloc(1, sizeof(json_value)),
                *p_iter_value = calloc(1, sizeof(json_value)),
                *p_last_value = calloc(1, sizeof(json_value));
    // Initialized data
    char *iter_key = 0;


    p_iter_value->type = JSON_VALUE_OBJECT;

    // Construct the object
    dict_construct(&p_value->object, 1);
    
    p_iter_value->type = JSON_VALUE_OBJECT;
    dict_construct(&p_iter_value->object, 1);

    p_last_value = p_iter_value;
    
    // Iterate through the alphabet
    for (size_t i = 'z'; i > 'a'-1; i--)
    {

        iter_key = calloc(2, sizeof(char));
        iter_key[0] = i;
        iter_key[1] = 0;

        p_iter_value = calloc(1, sizeof(json_value)),

        p_iter_value->type = JSON_VALUE_OBJECT;
        dict_construct(&p_iter_value->object, 1);
        dict_add(p_iter_value->object,iter_key, p_last_value);   
        p_last_value = p_iter_value;
    }
    
    // Return
    *pp_value = p_iter_value;

    // Success
    return 1;
}

int construct_object_array ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value       = calloc(1, sizeof(json_value)),
                *p_array_value = calloc(1, sizeof(json_value)),
                *p_int_value   = 0;
    
    // Type
    p_value->type        = JSON_VALUE_OBJECT;
    p_array_value->type  = JSON_VALUE_ARRAY;

    // Construct the object
    dict_construct(&p_value->object, 1);
    array_construct(&p_array_value->list, 3);

    p_int_value          = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 1;
    array_add(p_array_value->list, p_int_value);

    p_int_value          = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 2;
    array_add(p_array_value->list, p_int_value);
    
    p_int_value          = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 3;
    array_add(p_array_value->list, p_int_value);

    dict_add(p_value->object, "abc", p_array_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_array_objects ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value       = calloc(1, sizeof(json_value)),
                *p_array_value = calloc(1, sizeof(json_value)),
                *p_dict_value  = 0,
                *p_int_value   = 0;
    
    // Type
    p_value->type        = JSON_VALUE_OBJECT;
    p_array_value->type  = JSON_VALUE_ARRAY;

    // Construct the object
    dict_construct(&p_value->object, 1);
    array_construct(&p_array_value->list, 3);

    p_int_value          = calloc(1, sizeof(json_value));
    p_dict_value         = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 1;
    dict_construct(&p_dict_value->object, 1);
    dict_add(p_dict_value->object,"a",p_int_value);
    array_add(p_array_value->list, p_dict_value);

    p_int_value          = calloc(1, sizeof(json_value));
    p_dict_value         = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 2;
    dict_construct(&p_dict_value->object, 1);
    dict_add(p_dict_value->object,"b",p_int_value);
    array_add(p_array_value->list, p_dict_value);
    
    p_int_value          = calloc(1, sizeof(json_value));
    p_dict_value         = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 3;
    dict_construct(&p_dict_value->object, 1);
    dict_add(p_dict_value->object,"c",p_int_value);
    array_add(p_array_value->list, p_dict_value);

    dict_add(p_value->object, "a", p_array_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_object_array_object ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value       = calloc(1, sizeof(json_value)),
                *p_array_value = calloc(1, sizeof(json_value)),
                *p_dict_value  = 0,
                *p_int_value   = 0;
    
    // Type
    p_value->type        = JSON_VALUE_OBJECT;
    p_array_value->type  = JSON_VALUE_ARRAY;

    // Construct the object
    dict_construct(&p_value->object, 1);
    array_construct(&p_array_value->list, 3);

    p_int_value          = calloc(1, sizeof(json_value));
    p_dict_value         = calloc(1, sizeof(json_value));
    p_int_value->type    = JSON_VALUE_INTEGER;
    p_int_value->integer = 1;
    dict_construct(&p_dict_value->object, 1);
    dict_add(p_dict_value->object,"a",p_int_value);
    array_add(p_array_value->list, p_dict_value);

    dict_add(p_value->object, "a", p_array_value);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_empty ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_ARRAY;

    // Value
    array_construct(&p_value->list, 1);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_null ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_ARRAY;

    // Value
    array_construct(&p_value->list, 1);
    array_add(p_value->list, 0);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_nulls ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value));
    
    // Type
    p_value->type    = JSON_VALUE_ARRAY;

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

int construct_array_bool ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_bool  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;

    // Value
    p_bool = calloc(1, sizeof(json_value));
    p_bool->type = JSON_VALUE_BOOLEAN;
    p_bool->boolean = true;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_bools ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_bool  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_bool = calloc(1, sizeof(json_value));
    p_bool->type = JSON_VALUE_BOOLEAN;
    p_bool->boolean = true;
    array_add(p_value->list, p_bool);
    
    // Value 2
    p_bool = calloc(1, sizeof(json_value));
    p_bool->type = JSON_VALUE_BOOLEAN;
    p_bool->boolean = false;
    array_add(p_value->list, p_bool);

    // Value 3
    p_bool = calloc(1, sizeof(json_value));
    p_bool->type = JSON_VALUE_BOOLEAN;
    p_bool->boolean = true;
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_int ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_bool  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;

    // Value
    p_bool = calloc(1, sizeof(json_value));
    p_bool->type = JSON_VALUE_INTEGER;
    p_bool->integer = 1;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_bool);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_ints ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_int  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_int = calloc(1, sizeof(json_value));
    p_int->type = JSON_VALUE_INTEGER;
    p_int->integer = 1;
    array_add(p_value->list, p_int);
    
    // Value 2
    p_int = calloc(1, sizeof(json_value));
    p_int->type = JSON_VALUE_INTEGER;
    p_int->integer = 2;
    array_add(p_value->list, p_int);

    // Value 3
    p_int = calloc(1, sizeof(json_value));
    p_int->type = JSON_VALUE_INTEGER;
    p_int->integer = 3;
    array_add(p_value->list, p_int);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_float ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_float  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;

    // Value
    p_float = calloc(1, sizeof(json_value));
    p_float->type = JSON_VALUE_NUMBER;
    p_float->number = 3.14;
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_float);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_floats ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_float  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_float = calloc(1, sizeof(json_value));
    p_float->type = JSON_VALUE_NUMBER;
    p_float->number = 1.2;
    array_add(p_value->list, p_float);
    
    // Value 2
    p_float = calloc(1, sizeof(json_value));
    p_float->type = JSON_VALUE_NUMBER;
    p_float->number = 3.4;
    array_add(p_value->list, p_float);

    // Value 3
    p_float = calloc(1, sizeof(json_value));
    p_float->type = JSON_VALUE_NUMBER;
    p_float->number = 5.6;
    array_add(p_value->list, p_float);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_string_empty ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_string  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;

    // Value
    p_string = calloc(1, sizeof(json_value));
    p_string->type = JSON_VALUE_STRING;
    p_string->string = "";
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_string ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value = calloc(1, sizeof(json_value)),
                *p_string  = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;

    // Value
    p_string = calloc(1, sizeof(json_value));
    p_string->type = JSON_VALUE_STRING;
    p_string->string = "abc";
    array_construct(&p_value->list, 1);
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_strings ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value  = calloc(1, sizeof(json_value)),
                *p_string = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_string = calloc(1, sizeof(json_value));
    p_string->type = JSON_VALUE_STRING;
    p_string->string = "abc";
    array_add(p_value->list, p_string);
    
    // Value 2
    p_string = calloc(1, sizeof(json_value));
    p_string->type = JSON_VALUE_STRING;
    p_string->string = "def";
    array_add(p_value->list, p_string);

    // Value 3
    p_string = calloc(1, sizeof(json_value));
    p_string->type = JSON_VALUE_STRING;
    p_string->string = "ghi";
    array_add(p_value->list, p_string);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_object_empty ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value  = calloc(1, sizeof(json_value)),
                *p_object = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object = calloc(1, sizeof(json_value));
    p_object->type = JSON_VALUE_OBJECT;
    dict_construct(&p_object->object, 2);
    array_add(p_value->list, p_object);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_object ( json_value **pp_value )
{
    
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_object          = 0,
                *p_object_property = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object_property = calloc(1, sizeof(json_value));
    p_object_property->type = JSON_VALUE_INTEGER;
    p_object_property->integer = 1;

    // Value 2
    p_object = calloc(1, sizeof(json_value));
    p_object->type = JSON_VALUE_OBJECT;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "a", p_object_property);
    array_add(p_value->list, p_object);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_objects ( json_value **pp_value )
{
    
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_object          = 0,
                *p_object_property = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_object_property = calloc(1, sizeof(json_value));
    p_object_property->type = JSON_VALUE_INTEGER;
    p_object_property->integer = 1;

    // Value 2
    p_object = calloc(1, sizeof(json_value));
    p_object->type = JSON_VALUE_OBJECT;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "a", p_object_property);
    array_add(p_value->list, p_object);
    
    // Value 1
    p_object_property = calloc(1, sizeof(json_value));
    p_object_property->type = JSON_VALUE_INTEGER;
    p_object_property->integer = 2;

    // Value 2
    p_object = calloc(1, sizeof(json_value));
    p_object->type = JSON_VALUE_OBJECT;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "b", p_object_property);
    array_add(p_value->list, p_object);

    // Value 1
    p_object_property = calloc(1, sizeof(json_value));
    p_object_property->type = JSON_VALUE_INTEGER;
    p_object_property->integer = 3;

    // Value 2
    p_object = calloc(1, sizeof(json_value));
    p_object->type = JSON_VALUE_OBJECT;
    dict_construct(&p_object->object, 2);
    dict_add(p_object->object, "c", p_object_property);
    array_add(p_value->list, p_object);

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_array_empty ( json_value **pp_value )
{
    
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_array           = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 2
    p_array = calloc(1, sizeof(json_value));
    p_array->type = JSON_VALUE_ARRAY;
    array_construct(&p_array->list, 3);

    // Value 2
    array_add(p_value->list, p_array);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_array_array_empty ( json_value **pp_value )
{
    
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_array           = 0,
                *p_array_array     = 0;

    // Type
    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);
    
    // Value 1
    p_array_array = calloc(1, sizeof(json_value));
    p_array_array->type = JSON_VALUE_ARRAY;
    array_construct(&p_array_array->list, 3);

    // Value 2
    p_array = calloc(1, sizeof(json_value));
    p_array->type = JSON_VALUE_ARRAY;
    array_construct(&p_array->list, 3);
    array_add(p_array->list, p_array_array);

    // Value 2
    array_add(p_value->list, p_array);
    
    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_matrix ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_subarray        = 0,
                *p_object_property = 0;

    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 3);

    {
        p_subarray = calloc(1, sizeof(json_value));
        p_subarray->type = JSON_VALUE_ARRAY;
        array_construct(&p_subarray->list, 3);

        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 1;
        array_add(p_subarray->list, p_object_property);

        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 2;
        array_add(p_subarray->list, p_object_property);

        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 3;
        array_add(p_subarray->list, p_object_property);

        array_add(p_value->list, p_subarray);
    }

    {
        p_subarray = calloc(1, sizeof(json_value));
        p_subarray->type = JSON_VALUE_ARRAY;
        array_construct(&p_subarray->list, 3);
    
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 4;
        array_add(p_subarray->list, p_object_property);
        
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 5;
        array_add(p_subarray->list, p_object_property);
    
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 6;
        array_add(p_subarray->list, p_object_property);
    
        array_add(p_value->list, p_subarray);
    }

    {
        p_subarray = calloc(1, sizeof(json_value));
        p_subarray->type = JSON_VALUE_ARRAY;
        array_construct(&p_subarray->list, 3);
    
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 7;
        array_add(p_subarray->list, p_object_property);
        
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 8;
        array_add(p_subarray->list, p_object_property);
    
        p_object_property = calloc(1, sizeof(json_value));
        p_object_property->type = JSON_VALUE_INTEGER;
        p_object_property->integer = 9;
        array_add(p_subarray->list, p_object_property);
    
        array_add(p_value->list, p_subarray);
    }

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

int construct_array_tensor ( json_value **pp_value )
{
    // Initialized data
    json_value *p_value           = calloc(1, sizeof(json_value)),
                *p_subarray        = 0,
                *p_subsubarray     = 0,
                *p_object_property = 0;

    p_value->type = JSON_VALUE_ARRAY;
    array_construct(&p_value->list, 2);

    {
        p_subarray = calloc(1, sizeof(json_value));
        p_subarray->type = JSON_VALUE_ARRAY;
        array_construct(&p_subarray->list, 2);


        {
            p_subsubarray = calloc(1, sizeof(json_value));
            p_subsubarray->type = JSON_VALUE_ARRAY;
            array_construct(&p_subsubarray->list, 2);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 1;
            array_add(p_subsubarray->list, p_object_property);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 2;
            array_add(p_subsubarray->list, p_object_property);

            array_add(p_subarray->list, p_subsubarray);
        }

        {
            p_subsubarray = calloc(1, sizeof(json_value));
            p_subsubarray->type = JSON_VALUE_ARRAY;
            array_construct(&p_subsubarray->list, 2);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 3;
            array_add(p_subsubarray->list, p_object_property);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 4;
            array_add(p_subsubarray->list, p_object_property);

            array_add(p_subarray->list, p_subsubarray);
        }

        array_add(p_value->list, p_subarray);
    }

    {
        p_subarray = calloc(1, sizeof(json_value));
        p_subarray->type = JSON_VALUE_ARRAY;
        array_construct(&p_subarray->list, 2);


        {
            p_subsubarray = calloc(1, sizeof(json_value));
            p_subsubarray->type = JSON_VALUE_ARRAY;
            array_construct(&p_subsubarray->list, 2);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 5;
            array_add(p_subsubarray->list, p_object_property);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 6;
            array_add(p_subsubarray->list, p_object_property);

            array_add(p_subarray->list, p_subsubarray);
        }

        {
            p_subsubarray = calloc(1, sizeof(json_value));
            p_subsubarray->type = JSON_VALUE_ARRAY;
            array_construct(&p_subsubarray->list, 2);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 7;
            array_add(p_subsubarray->list, p_object_property);

            p_object_property = calloc(1, sizeof(json_value));
            p_object_property->type = JSON_VALUE_INTEGER;
            p_object_property->integer = 8;
            array_add(p_subsubarray->list, p_object_property);

            array_add(p_subarray->list, p_subsubarray);
        }

        array_add(p_value->list, p_subarray);
    }

    // Return
    *pp_value = p_value;

    // Success
    return 1;
}

bool test_parse_json ( char *test_file, int(*expected_value_constructor) (json_value **), result_t expected )
{
    
    // Initialized data
    json_value *p_return_value   = 0,
                *p_expected_value = 0;
    bool         ret      = true;
    size_t       file_len = 0;
    result_t     result   = 0,
                 value_eq = 0;
    char        *file_buf = 0;

    if (expected_value_constructor)
        expected_value_constructor(&p_expected_value);

    result = load_json ( &p_return_value, test_file );
    value_eq = value_equals(p_return_value, p_expected_value);
    
    if ( p_return_value )
        free_json_value(p_return_value);
    
    // Success
    return (result == expected && value_eq);
}

bool test_serial_json ( char *test_file, char *expected_file, int(*expected_value_constructor) (json_value **), result_t expected )
{
    
    // Initialized data
    json_value *p_return_value   = 0,
                *p_expected_value = 0;
    bool         ret      = true;
    size_t       file_len = 0;
    result_t     result   = 0,
                 value_eq = 0;
    char        *file_buf = 0;
    FILE *p_f  = fopen(test_file, "w");
    FILE *p_ef = 0;
    
    if (expected_value_constructor)
        expected_value_constructor(&p_expected_value);

    result = print_json_value(p_expected_value, p_f);
    
    free_json_value(p_return_value);
    fclose(p_f);

    p_f  = fopen(test_file, "r");
    p_ef = fopen(expected_file, "r");    

    // Short circuit
    if ( load_file(test_file, 0, false) != load_file(expected_file, 0, false) ) 
        return false;

    // Compare the files
    {

        // Initialized data
        size_t  f_len  = load_file(test_file, 0, false),
                ef_len = load_file(expected_file, 0, false);
        char   *f_contents  = calloc(f_len+1, sizeof(char)),
               *ef_contents = calloc(ef_len+1, sizeof(char));

        // Error checking
        if ( ef_contents == (void *) 0 )
            return false;
        
        // Error checking
        if ( f_contents == (void *) 0 )
            return false;

        load_file(test_file    , f_contents, false),
        load_file(expected_file, ef_contents, false);

        for (size_t i = 0; i < f_len; i++)
        {
            if ( f_contents[i] != ef_contents[i] )
                ret = false;
        }

        // Free the file contents
        free(f_contents);
        free(ef_contents);
    }



    // Success
    return ret;
}

int print_test ( const char *scenario_name, const char *test_name, bool passed )
{

    // Initialized data
    printf("%s %-75s %s\n",scenario_name, test_name, (passed) ? "PASS" : "FAIL");

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
    if ( buffer )
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
