#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <boost/variant.hpp>

namespace skipp {
namespace token {

struct eof {};
struct lambda {};
struct dot {};
struct open_paren {};
struct close_paren {};
struct variable {
    std::string name;
};
struct number {
    int val;
};

#define MAKE_OPERATORS(type)\
    inline std::ostream& operator<<(std::ostream& o, type const&) {\
        o << "{" #type "}";\
        return o;\
    }\
    \
    inline bool operator==(type const&, type const&) {\
        return true;\
    }

MAKE_OPERATORS(eof)
MAKE_OPERATORS(lambda)
MAKE_OPERATORS(dot)
MAKE_OPERATORS(open_paren)
MAKE_OPERATORS(close_paren)

#undef MAKE_OPERATORS

std::ostream& operator<<(std::ostream&, variable const&);
bool operator==(variable const&, variable const&);
std::ostream& operator<<(std::ostream&, number const&);
bool operator==(number const&, number const&);

using token = boost::variant<eof, lambda, dot, open_paren,
                             close_paren, variable, number>;
using tokens = std::vector<token>;

bool operator!=(token const& lhs, token const& rhs);

template<typename T>
bool is(token const& tk) {
    return boost::get<T>(&tk);
}

} // token
} // skipp
