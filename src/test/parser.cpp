#include "parser.hpp"
#include <boost/test/unit_test.hpp>

using namespace skipp;
using namespace lambda_ast;

BOOST_AUTO_TEST_CASE(parse_variable) {
    tree expected{variable{"x"}};
    auto result = parse("x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(parse_lambda) {
    tree expected{lambda{"x", variable{"x"}}};
    auto result = parse("\\x. x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(parse_application) {
    tree expected{application{variable{"f"}, variable{"x"}}};
    auto result = parse("f x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(parse_many_applications) {
    tree expected{application{
        application{variable{"f"}, variable{"x"}},
        variable{"y"}
    }};
    auto result = parse("f x y");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(parse_parentheses) {
    tree expected{application{
        variable{"f"},
        application{variable{"x"},variable{"y"}}
    }};
    auto result = parse("f (x y)");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(parse_numbers) {
    // Lisp-style!
    tree expected{
        application{
            application{
                variable{"+"},
                number{3}},
            number{5}}};
    auto result = parse("+ 3 5");
    BOOST_CHECK_EQUAL(result, expected);
}
