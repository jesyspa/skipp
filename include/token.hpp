#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>

struct eof {};
struct lambda {};
struct dot {};
struct open_paren {};
struct close_paren {};
struct variable {
    std::string name;
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

using token = boost::variant<eof, lambda, dot, open_paren, close_paren,
      variable>;

bool operator!=(token const& lhs, token const& rhs);
