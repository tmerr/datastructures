#include "stdio.h"
#include "stdlib.h"

#define KTYPE int32_t
#define VTYPE int32_t
#include "hashtable.h"
#undef KTYPE
#undef VTYPE

size_t hashfunc(int32_t key) {
    return key;
}

void assert(bool a, char* failmsg) {
    if (!a) {
        printf("assert failed: %s", failmsg);
        getchar();
        exit(1);
    }
}

bool keyeq(int32_t a, int32_t b) {
    return a == b;
}

// test of basic functionality
void test1() {
    hashtable *table = hashtable_create(hashfunc, keyeq);

    assert(table != NULL, "test1: table is null");
    assert(hashtable_insert(table, 1, 3) == 0, "test1: failed to insert 1: 3");
    assert(hashtable_insert(table, 2, 4) == 0, "test1: failed to insert 2: 4");

    int *val = hashtable_get(table, 1);
    assert(val != NULL, "test1: no val at key 1");
    assert(*val == 3, "test1: oops expected 3");

    int *val2 = hashtable_get(table, 2);
    assert(val2 != NULL, "test1: no val at key 2");
    assert(*val2 == 4, "test1: oops expected 3");

    assert(hashtable_insert(table, 3, 5) == 0, "test1: failed to insert 3: 5");
    assert(hashtable_insert(table, 3, 6) == 0, "test1: failed to insert 3: 6");
    int *val3 = hashtable_get(table, 3);
    assert(val3 != NULL, "test1: no val at key 3");
    assert(*val3 == 6, "test1: wrong val at key 3");
    assert(hashtable_remove(table, 3) == 0, "test1: problem removing key that exists");
    assert(hashtable_remove(table, 3) == 1, "test1: problem removing key that doesn't exist");

    assert(hashtable_size(table) == 2, "test1: expected size 2");

    hashtable_destroy(table);
}

// test 100K element insertion
void test2() {
    hashtable *table = hashtable_create(hashfunc, keyeq);

    int32_t testsize = 1e5;

    for (int32_t i = 0; i < testsize; ++i) {
        int32_t j = 2 * i;
        hashtable_insert(table, i, j);
    }
    assert(hashtable_size(table) == testsize, "test2: expected size 100K");

    hashtable_destroy(table);
}

// test 200K element insertion with each key used twice
void test3() {
    hashtable *table = hashtable_create(hashfunc, keyeq);

    for (int32_t i = 0; i < 1e5; ++i) {
        int32_t j = 2 * i;
        hashtable_insert(table, i, j);
    }
    assert(hashtable_size(table) == 1e5, "test3: expected size 100K");

    for (int32_t i = 0; i < 1e5; ++i) {
        int32_t j = 3 * i;
        hashtable_insert(table, i, j);
    }
    assert(hashtable_size(table) == 1e5, "test3: expected size 100K");

    for (int32_t i = 0; i < 1e5; ++i) {
        int *j = hashtable_get(table, i);
        assert(j != NULL, "test3: hashtable_get failed when element should exist");
        assert(*j == 3 * i, "test3: hashtable_get gave the wrong value");
    }

    hashtable_destroy(table);
}

// 100K insertion then iteration
void test4() {
    hashtable *table = hashtable_create(hashfunc, keyeq);

    int32_t testsize = 5;

    for (int32_t i = 0; i < testsize; ++i) {
        int32_t j = 2 * i;
        hashtable_insert(table, i, j);
    }
    assert(hashtable_size(table) == testsize, "test4: expected size 1K");

    hashtable_it it = hashtable_it_create(table);
    int32_t counter = 0;
    int32_t i;
    int32_t j;
    while (hashtable_it_next(&it, &i, &j)) {
        assert(j == 2 * i, "test4: unexpected value");
        ++counter;
    }

    assert(counter == testsize, "test4: expected counter at 1K");

    hashtable_destroy(table);
}

int main()
{
    test1();
    puts("finished test 1");
    test2();
    puts("finished test 2");
    test3();
    puts("finished test 3");
    test4();
    puts("finished test 4");

    puts("Tests Completed.");
    getchar();
	return 0;
}
