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
    // should be higher in the heap than the second arg.
    Heap(std::function<bool(T, T)> comp) {
        this->heapSize = 0;
        this->comp = comp;
        // use a placeholder for position 0.
        array.emplace(array.begin());
    }

    // Construct a heap from a vector and consume it in the process, aka heapify.
    // The comp function returns true if the first arg should be higher in the heap
    // than the second arg.
    // time: n
    Heap(std::vector<T> array, std::function<bool(T, T)> comp) {
        this->heapSize = array.size();
        // use a placeholder for position 0.
        array.emplace(array.begin());

        this->comp = comp;
        this->array = std::move(array);

        // get rightmost node of second deepest layer. 
        std::size_t j = 1;
        while (j * 2 < heapSize) {
            j *= 2;
        }

        // bubble down each node, skipping the ones that are already leaves.
        for (std::size_t i = j - 1; i >= 1; --i) {
            Heap::bubble_down(i);
        }
    }

    // Insert an element, preserving the heap property.
    // time: lg n
    void insert(T elem) {
        array.push_back(elem);
        ++heapSize;
        bubble_up(heapSize);
    }

    // Pop off the top element, preserving the heap property.
    // time: lg n
    T pop() {
        T result = array[1];
        array[1] = array[heapSize];
        array.pop_back();
        --heapSize;
        bubble_down(1);

        return result;
    }

    // Peek at the top element.
    // time: constant
    T peek() {
        return array[1];
    }

    // Heap sort the internal array
    // time: n * lg n
    void heapSort() {
        std::size_t tmp = heapSize;

        for (std::size_t j = heapSize; j >= 2; --j) {
            // swap
            T tmp = array[1];
            array[1] = array[j];
            array[j] = tmp;

            --heapSize;
            bubble_down(1);
        }

        // we're left with the elements reversed so put it back :/
        std::reverse(array.begin() + 1, array.end());

        heapSize = tmp;
    }

    // Return a reference to the internal array.
    // Element 0 in the array is a dummy.
    std::vector<T> & arrayRef() {
        return this->array;
    }

    // Return the number of nodes in the heap.
    std::size_t getHeapSize() {
        return heapSize;
    }

    // Verify the heap property.
    bool verifyHeapProperty() {
        for (std::size_t i = 1; i <= heapSize; ++i) {
            std::size_t left = i * 2;
            std::size_t right = i * 2 + 1;
            
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
    // Move a node down until it is ordered correctly.
    // time: lg n
    void bubble_down(std::size_t i) {
        std::size_t left = i * 2;
        std::size_t right = i * 2 + 1;

        std::size_t bigger;
        if (left <= heapSize && right <= heapSize) {
            bigger = comp(array[left], array[right]) ? left : right;
        } else if (left <= heapSize) {
            bigger = left;
        } else if (right <= heapSize) {
            bigger = right;
        } else {
            return;
        }

        if (comp(array[bigger], array[i])) {
            // swap
            T tmp = array[i];
            array[i] = array[bigger];
            array[bigger] = tmp;

            bubble_down(bigger);
        }
    }

    // Move a node up until it is ordered correctly.
    // time: lg n
    void bubble_up(std::size_t i) {
        if (i == 1) {
            return;
        }

        std::size_t parent = i / 2;
        if (comp(array[i], array[parent])) {
            // swap
            T tmp = array[i];
            array[i] = array[parent];
            array[parent] = tmp;

            bubble_up(parent);
        }
    }

    std::size_t heapSize;
    std::vector<T> array;
    std::function<bool(T, T)> comp;
};

#endif
