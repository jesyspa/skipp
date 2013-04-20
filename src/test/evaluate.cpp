#include "evaluate.hpp"
#include <boost/test/unit_test.hpp>

using namespace skipp;
using namespace ski;

BOOST_AUTO_TEST_CASE(eval_variable) {
    auto expected = variable{"x"};
    auto result = eval_string("x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(eval_i) {
    auto expected = variable{"x"};
    auto result = eval_string("(\\y. y) x");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(eval_k) {
    auto expected = variable{"x"};
    auto result = eval_string("(\\y. x) z");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(eval_s) {
    auto expected = variable{"x"};
    auto result = eval_string("(\\a. \\b. a) x y");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(eval_number) {
    auto expected = number{9};
    auto result = eval_string("+ 5 4");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(conditional) {
    auto expected = number{1};
    auto result = eval_string("<= 4 6 1 0");
    BOOST_CHECK_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(proper_update_inplace) {
    auto x = ski::variable{"x"};
    auto y = ski::variable{"y"};
    auto z = ski::variable{"z"};
    auto expected = application{
        x,
        application{
            y,
            z}};
    auto initial = ski::node{application{
        application{
            application{
                ski::combinator{'R'},
                x},
            y},
        z}};
    eval(initial);
    BOOST_CHECK_EQUAL(initial, expected);
}

BOOST_AUTO_TEST_CASE(proper_update_withargs) {
    auto x = ski::variable{"x"};
    auto y = ski::variable{"y"};
    auto z = ski::variable{"z"};
    auto expected = application{x, z};
    auto initial = ski::node{application{
        application{
            application{
                ski::combinator{'K'},
                x},
            y},
        z}};
    eval(initial);
    BOOST_CHECK_EQUAL(initial, expected);
}
