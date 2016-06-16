// See documentation in header

#include "hashtable.h"
#include "primes.h"
#include <stdlib.h>

//------- configuration -------//
#define INITIAL_CAPACITY 11
// resize when size = THRESHOLD * capacity
#define THRESHOLD 1
// when resizing, capacity is next_prime(RESIZEFACTOR * capacity)
#define RESIZEFACTOR 2

#include "defmacros"

typedef struct LINKEDLIST {
    KTYPE key;
    VTYPE value;
    struct LINKEDLIST *next;
} LINKEDLIST;

typedef struct HASHTABLE {
    LINKEDLIST **buckets; // array of buckets 
    size_t size;  // number of key: value pairs in the table
    size_t capacity; // number of buckets
    size_t(*hashfunc)(KTYPE key);
    bool(*keyeq)(KTYPE key1, KTYPE key2);
} HASHTABLE;

LINKEDLIST **alloc_buckets(size_t number) {
    LINKEDLIST **buckets = malloc(number*(sizeof(LINKEDLIST*)));
    if (buckets == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < number; ++i) {
        buckets[i] = NULL;
    }
    return buckets;
}

HASHTABLE *HASHTABLE_CREATE(size_t(*hashfunc)(KTYPE), bool(*keyeq)(KTYPE, KTYPE)) {
    HASHTABLE *table = malloc(sizeof(HASHTABLE));
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
LINKEDLIST *alloc_node(HASHTABLE *table, size_t index, KTYPE key) {
    if (table->buckets[index] == NULL) {
        table->buckets[index] = malloc(sizeof(LINKEDLIST));
        if (table->buckets[index] != NULL) {
            ++table->size;
        }
        return table->buckets[index];
    }
    else {
        LINKEDLIST* current = table->buckets[index];
        while (true) {
            if (table->keyeq(current->key, key)) {
                return current;
            }
            if (current->next == NULL) {
                current->next = malloc(sizeof(LINKEDLIST));
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
void relocate(HASHTABLE *table, LINKEDLIST *node) {
    node->next = NULL;
    size_t hash = table->hashfunc(node->key);
    size_t index = hash % table->capacity;

    if (table->buckets[index] == NULL) {
        table->buckets[index] = node;
    } else {
        LINKEDLIST *current = table->buckets[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
}

// Grow the hash table to the next prime number at least twice the current size
// Returns 0 on success.
int grow(HASHTABLE *table) {
    LINKEDLIST **oldbuckets = table->buckets;
    size_t oldcapacity = table->capacity;

    table->capacity = next_prime(RESIZEFACTOR * table->capacity);
    table->buckets = alloc_buckets(table->capacity);
    if (table->buckets == NULL) {
        return 1;
    }

    // move nodes to the new table
    for (size_t b = 0; b < oldcapacity; ++b) {
        LINKEDLIST *current = oldbuckets[b];
        while (current != NULL) {
            LINKEDLIST *next = current->next;
            relocate(table, current);
            current = next;
        }
    }
    free(oldbuckets);
    return 0;
}

int HASHTABLE_INSERT(HASHTABLE *table, KTYPE key, VTYPE value) {
    if (table->size > THRESHOLD * table->capacity) {
        if (grow(table) != 0) {
            return 1;
        }
    }

    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    LINKEDLIST *node = alloc_node(table, index, key);
    if (node == NULL) {
        return 1;
    }
    node->key = key;
    node->value = value;
    node->next = NULL;

    return 0;
}

int HASHTABLE_REMOVE(HASHTABLE *table, KTYPE key) {
    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    if (table->buckets[index] == NULL) {
        return 1;
    }
    LINKEDLIST **toupdate = &table->buckets[index];
    LINKEDLIST *current = table->buckets[index];
    while (current != NULL) {
        if (table->keyeq(current->key, key)) {
            LINKEDLIST* next = current->next;
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

VTYPE *HASHTABLE_GET(HASHTABLE *table, KTYPE key) {
    size_t hash = table->hashfunc(key);
    size_t index = hash % table->capacity;

    LINKEDLIST *current = table->buckets[index];
    while (current != NULL) {
        if (table->keyeq(current->key, key)) {
            return &current->value;
        } else {
            current = current->next;
        }
    }
    return NULL;
}

size_t HASHTABLE_SIZE(HASHTABLE *table) {
    return table->size;
}

// Free the bucket
void bucket_destroy(LINKEDLIST *current) {
    while (current != NULL) {
        LINKEDLIST *next = current->next;
        free(current);
        current = next;
    }
}

void HASHTABLE_DESTROY(HASHTABLE *table) {
    // free each bucket
    for (size_t i = 0; i < table->capacity; ++i) {
        bucket_destroy(table->buckets[i]);
    }

    // free the table itself
    free(table);
}


//------- iterator functions -------//
HASHTABLE_IT HASHTABLE_IT_CREATE(HASHTABLE *table) {
    static LINKEDLIST dummy;
    dummy.next = NULL;
    // note the index -1 is unsigned but will overflow to 0 on the first it_next
    HASHTABLE_IT it = { table, -1, &dummy };
    return it;
}

bool HASHTABLE_IT_NEXT(HASHTABLE_IT *it, KTYPE *outKey, VTYPE *outVal) {
    HASHTABLE *table = it->table;

    // at the end of the bucket?
    if (it->node->next == NULL) {
        // seek to a bucket with something in it
        for (++it->index; it->index < table->capacity; ++it->index) {
            LINKEDLIST *current = table->buckets[it->index];
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
        LINKEDLIST *next = it->node->next;
        *outKey = next->key;
        *outVal = next->value;
        it->node = next;
        return true;
    }
}

#include "undefmacros"
