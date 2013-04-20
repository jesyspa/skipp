#include "lambda2ski.hpp"
#include <boost/test/unit_test.hpp>

using namespace skipp;
using namespace ski;

BOOST_AUTO_TEST_CASE(compile_var) {
    auto expected = variable{"x"};
    auto result = string2ski("x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_app) {
    auto expected = application{
        variable{"f"}, variable{"x"}};
    auto result = string2ski("f x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_i) {
    auto expected = combinator{'I'};
    auto result = string2ski("\\x. x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_k) {
    auto expected = application{
        combinator{'K'},
        variable{"y"}};
    auto result = string2ski("\\x. y");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_s) {
    auto expected = application{
        application{
            combinator{'S'},
            combinator{'I'}},
        combinator{'I'}};
    auto result = string2ski("\\x. x x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_r) {
    auto expected = application{
        application{
            combinator{'R'},
            variable{"y"}},
        combinator{'I'}};
    auto result = string2ski("\\x. y x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_l) {
    auto expected = application{
        application{
            combinator{'L'},
            combinator{'I'}},
        variable{"y"}};
    auto result = string2ski("\\x. x y");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(compile_lambda) {
    auto expected = application{
        application{
            combinator{'L'},
            combinator{'I'}},
        combinator{'I'}};
    auto result = string2ski("(\\y. y (\\f. f))");
    BOOST_CHECK_EQUAL(result, expected);
}
