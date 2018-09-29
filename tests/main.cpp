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
    result = lest::run(test_memory_strategies);
    if (result == 0) {
        std::cout << "test_memory_strategies passed" << std::endl;
    }
    return 0;
}

