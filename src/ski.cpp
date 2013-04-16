#include "ski.hpp"
#include <map>

namespace skipp {
namespace ski {

struct print_ski : boost::static_visitor<> {
    print_ski(std::ostream& o) : o(o) {}
    std::ostream& o;

    void operator()(variable const& var) const {
        o << var.name;
    }

    void operator()(number const& num) const {
        o << num.val;
    }

    void operator()(combinator const& comb) const {
        o << comb.c;
    }

    void operator()(application const& app) const {
        o << '(';
        app.f.apply_visitor(*this);
        o << ' ';
        app.x.apply_visitor(*this);
        o << ')';
    }
};

node::node(detail::node_impl const& n) : ptr(to_ptr(n)) {}
node::node(variable const& n) : node(impl(n)) {}
node::node(number const& n) : node(impl(n)) {}
node::node(combinator const& n) : node(impl(n)) {}
node::node(application const& n) : node(impl(n)) {}

node node::update(node const& n) const {
    *ptr = *n.ptr;
    return *this;
}

std::ostream& operator<<(std::ostream& o, node const& n) {
    n.apply_visitor(print_ski{o});
    return o;
}

bool operator==(variable const& lhs,
                variable const& rhs) {
    return lhs.name == rhs.name;
}

bool operator==(number const& lhs,
                number const& rhs) {
    return lhs.val == rhs.val;
}

bool operator==(combinator const& lhs,
                combinator const& rhs) {
    return lhs.c == rhs.c;
}

bool operator==(application const& lhs,
                application const& rhs) {
    return lhs.f == rhs.f && lhs.x == rhs.x;
}

bool operator==(node const& lhs, node const& rhs) {
    return *lhs.ptr == *rhs.ptr;
}

}
}

