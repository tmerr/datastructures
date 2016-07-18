#include <functional>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <utility>

#ifndef HEAP_HPP
#define HEAP_HPP

template <class T>
class Heap {
public:
    // Construct an empty heap. The comp function returns true if the first arg
    // is greater than the second arg.
    Heap(std::function<bool(T, T)> comp) {
        this->heapSize = 0;
        this->comp = comp;
    }

    // Construct a heap from a vector and consume it in the process, aka heapify.
    // The comp function returns true if the first arg is greater than the second
    // arg.
    // time: n
    Heap(std::vector<T> array, std::function<bool(T, T)> comp) {
        this->heapSize = array.size();
        array.insert(array.begin(), {});
        this->comp = comp;
        this->array = std::move(array);

        // get rightmost node of second deepest layer. 
        size_t j = 1;
        while (j * 2 < heapSize) {
            j *= 2;
        }

        // bubble down each node, skipping the ones that are already leaves.
        for (size_t i = j - 1; i >= 1; --i) {
            Heap::bubble_down(i);
        }
    }

    // Move a node down until it is ordered with respect to its children.
    // time: lg n
    void bubble_down(size_t i) {
        size_t left = i * 2;
        size_t right = i * 2 + 1;

        if (left > heapSize || right > heapSize) {
            // this is a leaf.
            return;
        }

        if (comp(array[left], array[right])) {
            if (comp(array[left], array[i])) {
                // swap
                T tmp = array[i];
                array[i] = array[left];
                array[left] = tmp;

                bubble_down(left);
            }
        }
        if (comp(array[right], array[i])) {
            // swap
            T tmp = array[i];
            array[i] = array[right];
            array[right] = tmp;

            bubble_down(right);
        }
    }

    // Move a node up until it is ordered with respect to its parent.
    // time: lg n
    void bubble_up(size_t i);

    // Insert an element, preserving the heap property.
    // time: lg n
    void insert(T elem);

    // Pop off the top element, preserving the heap property.
    // time: constant
    T pop();

    // Heap sort the internal array.
    // time: n * lg n
    void heapSort();

    // Return a reference to the internal array.
    std::vector<T> & arrayRef();

    // Return the number of nodes in the heap.
    std::size_t getHeapSize() {
        return heapSize;
    }

    // Verify the heap property.
    bool verifyHeapProperty() {
        for (size_t i = 1; i <= heapSize; ++i) {
            size_t left = i * 2;
            size_t right = i * 2 + 1;
            
            if ((left <= heapSize) && comp(array[left], array[i])) {
                return false;
            }
            if ((right <= heapSize) && comp(array[right], array[i])) {
                return false;
            }
        }
        return true;
    }

private:
    std::size_t heapSize;
    std::vector<T> array;
    std::function<bool(T, T)> comp;
};

#endif
