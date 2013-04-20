#include "token.hpp"
#include "lexer.hpp"
#include <boost/variant.hpp>
#include <boost/test/unit_test.hpp>

using namespace skipp;
using namespace token;

BOOST_AUTO_TEST_CASE(token_visitor_creation) {
#define MAKE_OPERATOR(type, value) \
    int operator()(type) const {\
        return value;\
    }

    struct test_visitor : boost::static_visitor<int> {
        MAKE_OPERATOR(eof, 0)
        MAKE_OPERATOR(lambda, 1)
        MAKE_OPERATOR(dot, 2)
        MAKE_OPERATOR(open_paren, 3)
        MAKE_OPERATOR(close_paren, 4)
        MAKE_OPERATOR(variable, 5)
        MAKE_OPERATOR(number, 6)
    };

#undef MAKE_OPERATOR

    test_visitor v;
    auto app = boost::apply_visitor(v);
    token::token eof_{eof{}}, lambda_{lambda{}}, dot_{dot{}},
          open_paren_{open_paren{}}, close_paren_{close_paren{}},
          variable_{variable{"x"}}, number_{number{1}};
    BOOST_CHECK_EQUAL(app(eof_), 0);
    BOOST_CHECK_EQUAL(app(lambda_), 1);
    BOOST_CHECK_EQUAL(app(dot_), 2);
    BOOST_CHECK_EQUAL(app(open_paren_), 3);
    BOOST_CHECK_EQUAL(app(close_paren_), 4);
    BOOST_CHECK_EQUAL(app(variable_), 5);
    BOOST_CHECK_EQUAL(app(number_), 6);
}

BOOST_AUTO_TEST_CASE(token_equality) {
    token::token lambda_{lambda{}}, x{variable{"x"}}, y{variable{"y"}};
    BOOST_CHECK_EQUAL(lambda_, lambda_);
    BOOST_CHECK_EQUAL(x, x);
    BOOST_CHECK_NE(lambda_, x);
    BOOST_CHECK_NE(x, y);
    x != y;
}

BOOST_AUTO_TEST_CASE(lex_simple) {
    tokens simple = {lambda{}, variable{"x"}, dot{}, open_paren{},
        variable{"x"}, close_paren{}, eof{}};
    auto result = lex("\\x. (x)");
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
            simple.begin(), simple.end());
}

BOOST_AUTO_TEST_CASE(lex_long_identifier) {
    tokens ident = {variable{"abcd"}, eof{}};
    auto result = lex("abcd");
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
            ident.begin(), ident.end());
}

BOOST_AUTO_TEST_CASE(lex_two_identifiers) {
    tokens two_idents = {variable{"x"}, variable{"y"}, eof{}};
    auto result = lex("x y");
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
            two_idents.begin(), two_idents.end());
}

BOOST_AUTO_TEST_CASE(lex_numbers) {
    tokens numbers = {number{1}, number{42}, eof{}};
    auto result = lex("1 42");
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
            numbers.begin(), numbers.end());
}
