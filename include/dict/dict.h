#pragma once

#include <stdio.h>
#include <stdlib.h>

#define DLLEXPORT extern __declspec(dllexport)

struct dict_item_s;
struct dict_s;

struct dict_item_s {

	char* key;
	void* value;

	struct dict_item_s* next;
};

struct dict_s {
	struct dict_item_s  **entries;
	size_t                entry_count,
		                  n_entries;
};

typedef struct dict_item_s  dict_item;
typedef struct dict_s       dict;

// Allocaters
DLLEXPORT int        dict_create    ( dict **dictionary );

// Constructors
DLLEXPORT int        dict_construct ( dict **dictionary, size_t count );
DLLEXPORT int        dict_from_keys ( dict **dictionary, char      **keys,   size_t count );

// Accessors
DLLEXPORT dict_item *dict_get       ( dict  *dictionary, char       *name );
DLLEXPORT int        dict_items     ( dict  *dictionary, dict_item **items );
DLLEXPORT int        dict_values    ( dict  *dictionary, char      **values );
DLLEXPORT int        dict_keys      ( dict  *dictionary, char      **keys );

// Mutators
DLLEXPORT int        dict_add       ( dict  *dictionary, char       *name,   void  *val );
DLLEXPORT void      *dict_pop       ( dict  *dictionary, char       *name );

// TODO: 
//DLLEXPORT dict_item *dict_pop_item  ( dict  *dictionary, char       *name );

// Shallow copy
//DLLEXPORT dict      *dict_copy      ( dict  *dictionary, dict      **target );

// Clear all items
//DLLEXPORT dict      *dict_clear     ( dict  *dictionary );

// Destructors
DLLEXPORT int        dict_destroy   ( dict  *dictionary );