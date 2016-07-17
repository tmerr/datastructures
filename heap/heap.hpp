#include <functional>
#include <vector>
#include <cstddef>

#ifndef HEAP_HPP
#define HEAP_HPP

template <class T>
class Heap {
public:
    // Construct an empty heap.
    Heap(std::function<bool(T, T)> comp);

    // Construct a heap from a vector and consume it in the process, aka heapify.
    // time: n
    Heap(std::vector<T> array, std::function<bool(T, T)> comp);

    // Move a node down until it is ordered with respect to its children.
    // time: lg n
    void bubble_down(int i);

    // Move a node up until it is ordered with respect to its parent.
    // time: lg n
    void bubble_up(int i);

    // Insert an element, preserving the heap property.
    // time: lg n
    void insert(T elem);

    // Pop off the top element, preserving the heap property.
    // time: constant
    void pop();

    // Heap sort the internal array.
    // time: n * lg n
    void heapSort();

    // Return a reference to the internal array.
    std::vector<T> & arrayRef();

    // Return the number of nodes in the heap.
    std::size_t getHeapSize();

    // Verify the heap property.
    bool verifyHeapProperty();

private:
    std::size_t heapSize;
    std::vector<T> array;
    std::function<bool(T, T)> comp;
};

#endif
