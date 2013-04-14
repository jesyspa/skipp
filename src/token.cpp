#include "token.hpp"

std::ostream& operator<<(std::ostream& o, variable const& v) {
    o << "{variable \"" << v.name << "\"}";
    return o;
}

bool operator==(variable const& lhs, variable const& rhs) {
    return lhs.name == rhs.name;
}

bool operator!=(token const& lhs, token const& rhs) {
    return !(lhs == rhs);
}
