#include "ski.hpp"
#include <map>

namespace skipp {
namespace ski {

struct print_ski : boost::static_visitor<> {
    print_ski(std::ostream& o) : o(o) {}
    std::ostream& o;

    void operator()(application const& app) const {
        o << '(';
        app.f.apply_visitor(*this);
        o << ' ';
        app.x.apply_visitor(*this);
        o << ')';
    }

    void operator()(variable const& var) const {
        o << var.name;
    }
};

node::node(detail::node_impl const& n) : ptr(to_ptr(n)) {}
node::node(application const& n) : node(impl(n)) {}
node::node(variable const& n) : node(impl(n)) {}

void node::update(node const& n) const {
    *ptr = *n.ptr;
}

std::ostream& operator<<(std::ostream& o, node const& n) {
    n.apply_visitor(print_ski{o});
    return o;
}

bool operator==(variable const& lhs,
                variable const& rhs) {
    return lhs.name == rhs.name;
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

