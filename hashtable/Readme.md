Hash table
==========

This is a generic hash table I wrote for practice that is:
   - Dynamically sized
   - Uses chaining to resolve collisions
   - Maintains a prime number of buckets (as a defense against bad hash functions)

It is supposed to be easy for the user to make a C header
and source file for their own specialized version of a hash table.

An example of this is given in ihashtable.c and ihashtable.h. You can see
preprocessor macros are used to define the key type, value type, and a unique
prefix for the type and function names. Some test cases are in test.c.
