#include "heap.hpp"

#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

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

void testHeapSort() {
    std::vector<int32_t> initial {9, 1, 6, 3, 10, 5, 2, 7, 8, 0, 4};
    Heap<int32_t> heap(initial, comp);
    heap.heapSort();

    auto ref = heap.arrayRef();

    std::vector<int32_t> expected {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    if (ref.size() - 1 == expected.size() &&
        std::equal(ref.begin() + 1, ref.end(), expected.begin())) {
        std::cout << "  heapsort PASS" << std::endl;
    } else {
        std::cout << "  heapsort FAIL" << std::endl;
    }
}

void testHeapifySomething() {
    std::vector<int32_t> something {3, 2, 5, 7, 6, 2, 6, 4, 2, 9};
    Heap<int32_t> heap(something, comp);
    if (heap.verifyHeapProperty()) {
        std::cout << "  heap property PASS" << std::endl;
    } else {
        std::cout << "  heap property FAIL" << std::endl;
    }
    if (heap.getHeapSize() != 10) {
        std::cout << "  wrong heap size" << std::endl;
    }
}

int main() {
    std::cout << "testing!" << std::endl;
    testConstruct();
    std::cout << "first done" << std::endl;
    testHeapifyNothing();
    std::cout << "second done" << std::endl;
    testHeapifySomething();
    std::cout << "third done" << std::endl;
    testHeapSort();
    std::cout << "fourth done" << std::endl;

    return 0;
}
