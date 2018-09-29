#include "lest/lest_basic.hpp"
#include "test_memory.cpp"
#include <iostream>

int main() {
    auto result = lest::run(test_memory_operations);
    if (result == 0) {
        std::cout << "test_memory_operations passed" << std::endl;
    }
    result = lest::run(test_memory_requests);
    if (result == 0) {
        std::cout << "test_memory_requests passed" << std::endl;
    }
    result = lest::run(test_memory_first_appropriate_strategy);
    if (result == 0) {
        std::cout << "test_memory_first_appropriate_strategy passed" << std::endl;
    }
    result = lest::run(test_memory_most_appropriate_strategy);
    if (result == 0) {
        std::cout << "test_memory_most_appropriate_strategy passed" << std::endl;
    }
    result = lest::run(test_memory_least_appropriate_strategy);
    if (result == 0) {
        std::cout << "test_memory_least_appropriate_strategy passed" << std::endl;
    }
    return 0;
}

