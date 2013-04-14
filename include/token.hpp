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

using token = boost::variant<eof, lambda, dot, open_paren,
                             close_paren, variable>;
using tokens = std::vector<token>;

bool operator!=(token const& lhs, token const& rhs);

namespace detail {
    template<typename T>
    struct is_impl : boost::static_visitor<bool> {
        bool operator()(T const&) const {
            return true;
        }

        template<typename U>
        bool operator()(U const&) const {
            return false;
        }
    };
}

template<typename T>
bool is(token const& tk) {
    return boost::apply_visitor(detail::is_impl<T>{}, tk);
}

} // token
} // skipp
