// This is a hash table.
// It uses a generic key and value type that you define using macros before including this header.
// For example:
//   #define KTYPE int
//   #define VTYPE char*
//   #include "hashtable.h"
//   #undef KTYPE
//   #undef VTYPE
// The user must define the hash function and key equality function. These are
// passed into hashtable_create.
//
// Implementation notes:
//  - Uses chaining to resolve collisions.
//  - Maintains a prime number of buckets. This is a defense against bad hash functions.
//
// Author: Trevor Merrifield

#include "primes.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 11

// resize when size = THRESHOLD * capacity
#define THRESHOLD 1

// scale the table capacity by RESIZEFACTOR * capacity when the threshold is reached.
#define RESIZEFACTOR 2

typedef struct linkedlist {
    KTYPE key;
    VTYPE value;
    struct linkedlist *next;
} linkedlist;

typedef struct hashtable {
    linkedlist **buckets; // array of buckets 
    size_t size;  // number of key: value pairs in the table
    size_t capacity; // number of buckets
    size_t(*hashfunc)(KTYPE key);
    bool(*keyeq)(KTYPE key1, KTYPE key2);
} hashtable;

linkedlist **alloc_buckets(size_t number) {
    linkedlist **buckets = malloc(number*(sizeof(linkedlist*)));
    if (buckets == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < number; ++i) {
        buckets[i] = NULL;
    }
    return buckets;
}

// Create a hash table.
// hashfunc: The hash function. It should ideally make each output (size_t) equally likely.
// keyeq: The key equality function.
hashtable *hashtable_create(size_t(*hashfunc)(KTYPE), bool(*keyeq)(KTYPE, KTYPE)) {
    hashtable *table = malloc(sizeof(hashtable));
    if (table == NULL) {
        return NULL;
    } else {
        table->buckets = alloc_buckets(INITIAL_CAPACITY);
        if (table->buckets == NULL) {
            free(table);
            return NULL;
        }
        table->size = 0;
        table->capacity = INITIAL_CAPACITY;
        table->hashfunc = hashfunc;
        table->keyeq = keyeq;
        return table;
    }
}

// Allocate a node and return it. If the key is in use then return a pointer to that node instead.
// Returns NULL on failure.
linkedlist *alloc_node(hashtable *table, size_t index, KTYPE key) {
    if (table->buckets[index] == NULL) {
        table->buckets[index] = malloc(sizeof(linkedlist));
        if (table->buckets[index] != NULL) {
            ++table->size;
        }
        return table->buckets[index];
    }
    else {
        linkedlist* current = table->buckets[index];
        while (true) {
            if (table->keyeq(current->key, key)) {
                return current;
            }
            if (current->next == NULL) {
                current->next = malloc(sizeof(linkedlist));
                if (current->next != NULL) {
                    ++table->size;
                }
                return current->next;
            }
            current = current->next;
        }
    }
}

// Move a node into the new table.
void relocate(hashtable *table, linkedlist *node) {
    node->next = NULL;
    size_t hash = table->hashfunc(node->key);
    size_t index = hash % table->capacity;

    if (table->buckets[index] == NULL) {
        table->buckets[index] = node;
    } else {
        linkedlist *current = table->buckets[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
}

// Grow the hash table to the next prime number at least twice the current size
// Returns 0 on success.
int grow(hashtable *table) {
    linkedlist **oldbuckets = table->buckets;
    size_t oldcapacity = table->capacity;

    table->capacity = next_prime(RESIZEFACTOR * table->capacity);
    table->buckets = alloc_buckets(table->capacity);
    if (table->buckets == NULL) {
        return 1;
    }

    // move nodes to the new table
    for (size_t b = 0; b < oldcapacity; ++b) {
        linkedlist *current = oldbuckets[b];
        while (current != NULL) {
            linkedlist *next = current->next;
            relocate(table, current);
            current = next;
        }
    }
    free(oldbuckets);
    return 0;
}

// Insert the given element (key: value pair) into the table.
// Returns 0 on success.
int hashtable_insert(hashtable *table, KTYPE key, VTYPE value) {
    if (table->size > THRESHOLD * table->capacity) {
        if (grow(table) != 0) {
            return 1;
        }
    }

    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    linkedlist *node = alloc_node(table, index, key);
    if (node == NULL) {
        return 1;
    }
    node->key = key;
    node->value = value;
    node->next = NULL;

    return 0;
}

// Remove the element with the given key.
// Returns 0 on success.
int hashtable_remove(hashtable *table, KTYPE key) {
    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    if (table->buckets[index] == NULL) {
        return 1;
    }
    linkedlist **toupdate = &table->buckets[index];
    linkedlist *current = table->buckets[index];
    while (current != NULL) {
        if (table->keyeq(current->key, key)) {
            linkedlist* next = current->next;
            *toupdate = next;
            free(current);
            --table->size;
            return 0;
        } else {
            toupdate = &current->next;
            current = current->next;
        }
    }
    return 1;
}

// Lookup the element with the given key.
// The result is a pointer to that element or NULL if it doesn't exist.
VTYPE *hashtable_get(hashtable *table, KTYPE key) {
    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    linkedlist *current = table->buckets[index];
    while (current != NULL) {
        if (table->keyeq(current->key, key)) {
            return &current->value;
        } else {
            current = current->next;
        }
    }
    return NULL;
}

// Return the number of elements in the table
size_t hashtable_size(hashtable *table) {
    return table->size;
}

// Free the bucket
void bucket_destroy(linkedlist *current) {
    while (current != NULL) {
        linkedlist *next = current->next;
        free(current);
        current = next;
    }
}

// Free the hash table
void hashtable_destroy(hashtable *table) {
    // free each bucket
    for (size_t i = 0; i < table->capacity; ++i) {
        bucket_destroy(table->buckets[i]);
    }

    // free the table itself
    free(table);
}


//------ iterator functions below ------//

typedef struct hashtable_it {
    hashtable *table;
    size_t index;
    linkedlist *node;
} hashtable_it;

hashtable_it hashtable_it_create(hashtable *table) {
    static linkedlist dummy;
    dummy.next = NULL;
    // note the index -1 is unsigned but will overflow to 0 on the first it_next
    hashtable_it it = { table, -1, &dummy };
    return it;
}

bool hashtable_it_next(hashtable_it *it, KTYPE *outKey, VTYPE *outVal) {
    hashtable *table = it->table;

    // at the end of the bucket?
    if (it->node->next == NULL) {
        // seek to a bucket with something in it
        for (++it->index; it->index < table->capacity; ++it->index) {
            linkedlist *current = table->buckets[it->index];
            if (current != NULL) {
                it->node = current;
                *outKey = current->key;
                *outVal = current->value;
                return true;
            }
            it->index = it->index + 1;
        }
        return false;
    } else {
        linkedlist *next = it->node->next;
        *outKey = next->key;
        *outVal = next->value;
        it->node = next;
        return true;
    }
}
