#include <cassert> // assert macro
#include <iostream> // std::cout

#include "FindTheDuplicate.hpp"

int findTheDuplicate(const int rangeBegin, const int rangeEnd, const std::vector<int>& values) {
    assert(rangeEnd > rangeBegin);
    
    // This works because 1^2^....^10
    int duplicate = 0;
    for(int i = rangeBegin; i <= rangeEnd; i++) {
        duplicate ^= i;
    }

    // And then all of the values in the range will cancel out except for the duplicate!
    for(const auto& val : values) {
        duplicate ^= val;
    }

    return duplicate;
}

int main() {
    std::vector<int> values {
        10, 2, 6, 3, 9, 4, 5, 6, 1, 7, 8
    };

    std::cout << "The duplicate is " << findTheDuplicate(1, 10, values) << "\n";
}