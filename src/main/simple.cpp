#include "evaluate.hpp"
#include <iostream>
#include <string>
#include <exception>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::cout << skipp::evaluate::eval_string(line) << '\n';
        }
        catch (std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}
