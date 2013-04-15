#include "parser.hpp"
#include "lexer.hpp"
#include <stack>
#include <stdexcept>

namespace {
using namespace skipp;
using namespace lambda_ast;

using iter = token::tokens::const_iterator;

template<typename T>
T const& expect(iter& it, std::string const& error) {
    if (auto* p = boost::get<T>(&*it)) {
        ++it;
        return *p;
    } else {
        throw std::runtime_error(error);
    }
}

tree parse_atom(iter& it);
tree parse_variable(iter& it);
tree parse_lambda(iter& it);
tree parse_parentheses(iter& it);
tree parse_expr(iter& it);

tree parse_atom(iter& it) {
    if (token::is<token::eof>(*it))
        throw std::runtime_error("expecting atom, got eof");
    if (token::is<token::variable>(*it))
        return parse_variable(it);
    if (token::is<token::lambda>(*it))
        return parse_lambda(it);
    if (token::is<token::open_paren>(*it))
        return parse_parentheses(it);
    throw std::runtime_error("expected atom");
}

tree parse_variable(iter& it) {
    auto& v = expect<token::variable>(it, "expected variable");
    return lambda_ast::variable{v.name};
}

tree parse_lambda(iter& it) {
    expect<token::lambda>(it, "expected lambda");
    auto& var = expect<token::variable>(it, "expected parameter name");
    expect<token::dot>(it, "expected dot");
    auto expr = parse_expr(it);
    return lambda_ast::lambda{var.name, expr};
}

tree parse_parentheses(iter& it) {
    expect<token::open_paren>(it, "expected opening parenthesis");
    auto result = parse_expr(it);
    expect<token::close_paren>(it, "expected closing parenthesis");
    return result;
}

tree parse_expr(iter& it) {
    if (token::is<token::eof>(*it))
        throw std::runtime_error("expecting expression, got eof");
    auto x = parse_atom(it);
    while (!token::is<token::eof>(*it)
        && !token::is<token::close_paren>(*it))
        x = lambda_ast::application{x, parse_atom(it)};
    return x;
}
}

namespace skipp {

tree parse(token::tokens const& toks) {
    auto it = toks.begin();
    auto result = parse_expr(it);
    if (!token::is<token::eof>(*it))
        throw std::runtime_error("input left over after parse");
    return result;
}

tree parse(std::string const& s) {
    return parse(lex(s));
}

}
