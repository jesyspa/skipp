#include "ski.hpp"

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

    void operator()(s_comb_0 const&) const {
        o << 'S';
    }

    void operator()(s_comb_1 const& s) const {
        o << "S[";
        s.f.apply_visitor(*this);
        o << "]";
    }

    void operator()(s_comb_2 const& s) const {
        o << "S[";
        s.f.apply_visitor(*this);
        o << ", ";
        s.g.apply_visitor(*this);
        o << "]";
    }

    void operator()(k_comb_0 const&) const {
        o << 'K';
    }

    void operator()(k_comb_1 const& k) const {
        o << "S[";
        k.x.apply_visitor(*this);
        o << "]";
    }

    void operator()(i_comb_0 const&) const {
        o << 'I';
    }
};

node::node(detail::node_impl const& n) : ptr(to_ptr(n)) {}
node::node(application const& n) : node(impl(n)) {}
node::node(variable const& n) : node(impl(n)) {}
node::node(s_comb_0 const& n) : node(impl(n)) {}
node::node(s_comb_1 const& n) : node(impl(n)) {}
node::node(s_comb_2 const& n) : node(impl(n)) {}
node::node(k_comb_0 const& n) : node(impl(n)) {}
node::node(k_comb_1 const& n) : node(impl(n)) {}
node::node(i_comb_0 const& n) : node(impl(n)) {}

std::ostream& operator<<(std::ostream& o, node const& n) {
    n.apply_visitor(print_ski(o));
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

bool operator==(s_comb_0 const&,
                s_comb_0 const&) {
    return true;
}

bool operator==(s_comb_1 const& lhs,
                s_comb_1 const& rhs) {
    return lhs.f == rhs.f;
}

bool operator==(s_comb_2 const& lhs,
                s_comb_2 const& rhs) {
    return lhs.f == rhs.f && lhs.g == rhs.g;
}

bool operator==(k_comb_0 const&, k_comb_0 const&) {
    return true;
}

bool operator==(k_comb_1 const& lhs,
                k_comb_1 const& rhs) {
    return lhs.x == rhs.x;
}

bool operator==(i_comb_0 const&, i_comb_0 const&) {
    return true;
}

bool operator==(node const& lhs, node const& rhs) {
    return *lhs.ptr == *rhs.ptr;
}

}
}

