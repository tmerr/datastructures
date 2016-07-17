#include "heap.hpp"

#include <vector>
#include <cstdint>
#include <iostream>

using std::int32_t;

bool comp(int32_t a, int32_t b) {
    return a < b;
}

void testConstruct() {
    Heap<int32_t> heap(comp);
}

void testHeapifyNothing() {
    std::vector<int32_t> nothing;
    Heap<int32_t> heap(nothing, comp);
}

void testHeapifySomething() {
    std::vector<int32_t> something {3, 2, 5, 7, 6, 2, 6, 4, 2, 9};
    Heap<int32_t> heap(something, comp);
    if (!heap.verifyHeapProperty()) {
        std::cout << "test failed: heap property violated" << std::endl;
    }
}

int main() {
    testConstruct();
    testHeapifyNothing();
    testHeapifySomething();

    return 0;
}
