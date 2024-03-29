#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>

namespace skipp {
namespace lambda_ast {

struct variable;
struct number;
struct lambda;
struct application;

using tree = boost::variant<
    variable,
    number,
    boost::recursive_wrapper<lambda>,
    boost::recursive_wrapper<application>
>;

struct variable {
    std::string name;
};

struct number {
    int val;
};

struct lambda {
    std::string param;
    tree body;
};

struct application {
    tree f, x;
};

std::ostream& operator<<(std::ostream&, tree const&);
bool operator==(variable const&, variable const&);
bool operator==(number const&, number const&);
bool operator==(lambda const&, lambda const&);
bool operator==(application const&, application const&);

} // ast
} // skipp
