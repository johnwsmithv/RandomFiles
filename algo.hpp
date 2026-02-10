#pragma once

#include <type_traits> // std::same_as
#include <vector> // std::vector
#include <limits> // std::numeric_limits
#include <print> // std::print, C++23 feature
#include <iostream> // std::cout

struct Node {
    int value;
    Node* node;
};

// Makes limiting the type of templated values
// easier to read from a programmer prespective
template<typename T>
concept OnlyNumeric = 
    std::same_as<T, int> ||
    std::same_as<T, float> ||
    std::same_as<T, double>;

class Algo {
public:
    // Linked List
    static Node* generateLinkedList(const int numNodes);
    static Node* reverseLinkedList(Node* linkedList);
    static void destroyLinkedList(Node* linkedList);
    static void printLinkedList(Node* linkedList);

    // Sorting algorithms
    template<typename T> requires OnlyNumeric<T>
    static void bubbleSort(std::vector<T>& toBeSorted);

    template<typename T> requires OnlyNumeric<T>
    static void selectionSort(std::vector<T>& toBeSorted);

    template<typename T> requires OnlyNumeric<T>
    static void insertionSort(std::vector<T>& toBeSorted);

    template<typename T> requires OnlyNumeric<T>
    static std::vector<T> mergeSort(std::vector<T>& toBeSorted);
};

/**
 * @brief Basic implementation of bubble sort.
 * 
 * @tparam T required to be an integral type
 * @param toBeSorted The vector which we want sorted
 */
template<typename T> requires OnlyNumeric<T>
void Algo::bubbleSort(std::vector<T>& toBeSorted) {
    if(toBeSorted.empty()) {
        return;
    }

    while(true) {
        // Might be able to use ranges here, but going to start with
        // a traditonal for-loop
        int swaps = 0;
        for(size_t i = 0; i < toBeSorted.size(); i++) {
            if(i + 1 < toBeSorted.size()) {
                // Want to ensure we aren't going out-of-bounds!
                if(toBeSorted[i] > toBeSorted[i + 1]) {
                    int temp = toBeSorted[i + 1];
                    toBeSorted[i + 1] = toBeSorted[i];
                    toBeSorted[i] = temp;

                    swaps++;
                }
            }
        }

        // If this is true, then we are done bubble sort
        if(swaps == 0) {
            break;
        }
    }
}

/**
 * @brief A basic implementation of selection sort
 * which iterates through the array and saves off the
 * minimum values index and then swaps that with the ith
 * index until the loop is sorted.
 * 
 * @tparam T required to be an integral type
 * @param toBeSorted The loop to be sorted
 */
template<typename T> requires OnlyNumeric<T>
void Algo::selectionSort(std::vector<T>& toBeSorted) {
    if(toBeSorted.empty() || toBeSorted.size() == 1UL) {
        return;
    }

    size_t idx = 0UL;
    while(idx < toBeSorted.size()) {
        size_t minIdx = idx;
        T minVal = std::numeric_limits<T>::max();

        for(size_t i = idx; i < toBeSorted.size(); i++) {
            if(toBeSorted[i] < minVal) {
                minVal = toBeSorted[i];
                minIdx = i;
            }
        }

        T temp = toBeSorted[idx];
        toBeSorted[idx++] = toBeSorted[minIdx];
        toBeSorted[minIdx] = temp;
    }
}

/**
 * @brief Basic implementation of insertion sort.
 * 
 * @todo This might be able to be optimized by terminating the'
 * for loop early.... look into later.
 * 
 * @tparam T 
 * @param toBeSorted 
 * @return requires 
 */
template<typename T> requires OnlyNumeric<T>
void Algo::insertionSort(std::vector<T>& toBeSorted) {
    size_t upperIdx = 0UL;

    while(upperIdx < toBeSorted.size()) {
        for(size_t i = upperIdx; i > 0; i--) {
            if(toBeSorted[i] < toBeSorted[i - 1]) {
                T temp = toBeSorted[i - 1];
                toBeSorted[i - 1] = toBeSorted[i];
                toBeSorted[i] = temp;
            }
        }

        upperIdx++;
    }
}

template<typename T> requires OnlyNumeric<T>
void merge(std::vector<T>& B, const int begin, const int middle, const int end, std::vector<T>& A) {
    int i = begin;
    int j = middle;

    for(int k = begin; k < end; k++) {
        if(i < middle && (j >= end || A[i] <= A[j])) {
            B[k] = A[i++];
        } else {
            B[k] = A[j++];
        }
    }
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& vec) {
    for(const auto& val : vec) {
        os << val << "\t";
    }

    return os;
}

template<typename T> requires OnlyNumeric<T>
void splitMerge(std::vector<T>& B, const int begin, const int end, std::vector<T>& A) {
    // If this is true, then we're done merging
    if(end - begin <= 1) {
        return;
    }

    // This is the middle of the next split
    const int middle = (end + begin) / 2;
    
    // Now we want to recursively call this function until we're down to a single element
    splitMerge(A, begin, middle, B);
    splitMerge(A, middle, end, B);
    std::println("Before: ({}, {}, {})", begin, middle, end);
    std::cout << B << "\n";

    merge(B, begin, middle, end, A);
    std::println("After: ({}, {}, {})", begin, middle, end);
    std::cout << B << "\n";
}

template<typename T> requires OnlyNumeric<T>
std::vector<T> Algo::mergeSort(std::vector<T>& toBeSorted) {
    std::vector<T> arrCopy(toBeSorted);

    splitMerge(toBeSorted, 0, toBeSorted.size(), arrCopy);

    return toBeSorted;
}