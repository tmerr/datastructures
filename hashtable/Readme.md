Hash table
==========

A hash table that uses chaining, dynamic resizing, and a prime number of buckets.

##Overview

There are two common ways to make generic hash tables in C. One is to use the `void *` type. This has the advantage of being easy to implement but it has major issues with performance and type safety. The other way is to use macros which is better but it makes the implementation's source code difficult to understand.

[klib](https://github.com/attractivechaos/klib/) is an example of a popular library that follows the latter approach. The usage of klib's hash table starts by invoking a macro like `KHASH_MAP_INIT_INT(somename, unsigned char)`. This declares and defines the specialized hash table on the spot. This is barbaric because of what happens if you accidentally use the same name twice in your project. If they end up in the same translation unit it will trigger a compilation error (multiple definitions). The other issue with combining declaration and definition is that you can't reuse the compiled binary code for a definition of a hash map across translation units. Oh well.

This implementation takes a different approach. It's terrible for usability but it avoids klib's issues. To create a specialization of a hash map you need to create a `.h` and a `.c` file. The header looks like

    #ifndef XHASHTABLE_H
    #define XHASHTABLE_H

    #include "stdint.h"

    #define PREFIX x       // the prefix of all hash table functions
    #define KTYPE int32_t  // the key type
    #define VTYPE int32_t  // the value type
    #include "hashtable.h"
    #undef PREFIX
    #undef KTYPE
    #undef VTYPE

    #endif

And the `.c` file looks like 

    #include "stdint.h"

    #define PREFIX x
    #define KTYPE int32_t
    #define VTYPE int32_t
    #include "hashtable.c"
    #undef PREFIX
    #undef KTYPE
    #undef VTYPE

They act like normal normal `.h` and `.c` files with the following functions. Note that the names depend on the chosen prefix which is x in this case.

| Function     | Description        |
| ------------- |-------------|
| xhashtable\* xhashtable_create(size_t(\*hashfunc)(KTYPE), bool(\*keyeq)(KTYPE, KTYPE))  | Create a hash table. The hash function should ideally make each output (size_t) equally likely. | 
| int xhashtable_insert(xhashtable \*table, KTYPE key, VTYPE value)   | Insert the given key: value pair into the table. Returns 0 on success.        | 
| int xhashtable_remove(xhashtable \*table, KTYPE key) | Remove the element with the given key. Returns 0 on success. |
| VTYPE \*xhashtable_get(xhashtable \*table, KTYPE key) | Lookup the element with the given key. The result is a pointer to that element or NULL if it doesn't exist. |
| size_t xhashtable_size(xhashtable \*table) | Return the number of elements in the table. | 
| void xhashtable_destroy(xhashtable \*table) | Free the hash table. | 
| xhashtable_it xhashtable_it_create(xhashtable \*table) | Create an iterator for the given hash table. | 
| bool xhashtable_it_next(xhashtable_it \*it, KTYPE \*outKey, VTYPE \*outVal) | Gets the next element from the iterator. Returns false (and leaves outKey and outVal unmodified) when the end of the iterator is reached. | 

This was as much about generics as it was about hash tables so I haven't got around to benchmarking and am not too worried about performance overall. Also, disclaimer, the prime.c found in this repository is [from the internet](http://stackoverflow.com/a/5694432/1546343). Also, see test.c for example usage.
