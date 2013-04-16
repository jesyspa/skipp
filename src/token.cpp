#include "token.hpp"

namespace skipp {
namespace token {

std::ostream& operator<<(std::ostream& o, variable const& v) {
    o << "{variable \"" << v.name << "\"}";
    return o;
}

bool operator==(variable const& lhs, variable const& rhs) {
    return lhs.name == rhs.name;
}

std::ostream& operator<<(std::ostream& o, number const& n) {
    o << "{number \"" << n.val << "\"}";
    return o;
}

bool operator==(number const& lhs, number const& rhs) {
    return lhs.val == rhs.val;
}

bool operator!=(token const& lhs, token const& rhs) {
    return !(lhs == rhs);
}

} // token
} // skipp

