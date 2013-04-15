#include "lexer.hpp"
#include "parser.hpp"
#include "lambda2ski.hpp"
#include "evaluate.hpp"
#include <iostream>
#include <string>
#include <exception>

int main() {
    std::string line;
    while (std::cout << ">>> " && std::getline(std::cin, line)) {
        try {
            auto lexed = skipp::lexer::lex(line);
            auto parsed = skipp::parser::parse(lexed);
            std::cout << "PARSED: " << parsed << '\n';
            auto ski = skipp::lambda2ski::lambda2ski(parsed);
            std::cout << "SKI: " << ski << '\n';
            auto result = skipp::evaluate::eval(ski);
            std::cout << "RESULT: " << result << '\n';
        }
        catch (std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}
