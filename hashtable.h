// This is a hash table that is:
//   - Dynamically sized
//   - Uses chaining to resolve collisions
//   - Maintains a prime number of buckets (as a defense against bad hash functions)
//
// It is also generic. It is supposed to be easy for the user to make a C header
// and source file for their own specialized version of this hash table.
//
// An example of this is given in ihashtable.c and ihashtable.h. You can see
// preprocessor macros are used to define the key type, value type, and a unique
// prefix for the type and function names. The prefix is there to avoid naming
// conflicts between different kinds of hash tables.

#include <stddef.h>
#include <stdbool.h>
#include "defmacros"

typedef struct HASHTABLE HASHTABLE;

// Create a hash table.
// hashfunc: The hash function. It should ideally make each output (size_t) equally likely.
// keyeq: The key equality function.
HASHTABLE *HASHTABLE_CREATE(size_t(*hashfunc)(KTYPE), bool(*keyeq)(KTYPE, KTYPE));

// Insert the given element (key: value pair) into the table.
// Returns 0 on success.
int HASHTABLE_INSERT(HASHTABLE *table, KTYPE key, VTYPE value);

// Remove the element with the given key.
// Returns 0 on success.
int HASHTABLE_REMOVE(HASHTABLE *table, KTYPE key);

// Lookup the element with the given key.
// The result is a pointer to that element or NULL if it doesn't exist.
VTYPE *HASHTABLE_GET(HASHTABLE *table, KTYPE key);

// Return the number of elements in the table.
size_t HASHTABLE_SIZE(HASHTABLE *table);

// Free the hash table.
void HASHTABLE_DESTROY(HASHTABLE *table);


//------- iterator -------//
typedef struct LINKEDLIST LINKEDLIST;

// An iterator.
typedef struct HASHTABLE_IT {
    HASHTABLE *table;
    size_t index;
    LINKEDLIST *node;
} HASHTABLE_IT;

// Create an iterator for the given hash table.
HASHTABLE_IT HASHTABLE_IT_CREATE(HASHTABLE *table);

// Gets the next element from the iterator.
// Returns false (and leaves outKey and outVal unmodified) when the end of the
// iterator is reached.
bool HASHTABLE_IT_NEXT(HASHTABLE_IT *it, KTYPE *outKey, VTYPE *outVal);

#include "undefmacros"
