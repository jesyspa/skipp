#include "ski.hpp"

namespace skipp {
namespace ski {

struct print_ski : boost::static_visitor<> {
    print_ski(std::ostream& o) : o(o) {}
    std::ostream& o;

    void operator()(value const& val) const {
        o << val;
    }

    void operator()(application const& app) const {
        o << '(';
        boost::apply_visitor(*this, *app.f);
        o << ' ';
        boost::apply_visitor(*this, *app.x);
        o << ')';
    }

    void operator()(variable const& var) const {
        o << var.name;
    }

    void operator()(s_comb_0 const&) const {
        o << 'S';
    }

    void operator()(s_comb_1 const& s) const {
        o << 'S';
        boost::apply_visitor(*this, *s.f);
    }

    void operator()(s_comb_2 const& s) const {
        o << 'S';
        boost::apply_visitor(*this, *s.f);
        boost::apply_visitor(*this, *s.g);
    }

    void operator()(k_comb_0 const&) const {
        o << 'K';
    }

    void operator()(k_comb_1 const& k) const {
        o << 'K';
        boost::apply_visitor(*this, *k.x);
    }

    void operator()(i_comb_0 const&) const {
        o << 'I';
    }
};

node_ptr make_application(node_ptr const& f, node_ptr const& x) {
    return std::make_shared<node>(application{f, x});
}

std::ostream& operator<<(std::ostream& o, node const& n) {
    boost::apply_visitor(print_ski(o), n);
    return o;
}

std::ostream& operator<<(std::ostream& o, node_ptr const& p) {
    if (!p)
        o << "{null}";
    else
        o << *p;
    return o;
}

std::ostream& operator<<(std::ostream& o, value const& v) {
    boost::apply_visitor(print_ski(o), v.impl);
    return o;
}

bool operator==(variable const& lhs, variable const& rhs) {
    return lhs.name == rhs.name;
}

bool operator==(application const& lhs, application const& rhs) {
    return *lhs.f == *rhs.f && *lhs.x == *rhs.x;
}

bool operator==(s_comb_0 const&, s_comb_0 const&) {
    return true;
}

bool operator==(s_comb_1 const& lhs, s_comb_1 const& rhs) {
    return *lhs.f == *rhs.f;
}

bool operator==(s_comb_2 const& lhs, s_comb_2 const& rhs) {
    return *lhs.f == *rhs.f && *lhs.g == *rhs.g;
}

bool operator==(k_comb_0 const&, k_comb_0 const&) {
    return true;
}

bool operator==(k_comb_1 const& lhs, k_comb_1 const& rhs) {
    return *lhs.x == *rhs.x;
}

bool operator==(i_comb_0 const&, i_comb_0 const&) {
    return true;
}

bool operator==(value const& lhs, value const& rhs) {
    return lhs.impl == rhs.impl;
}

node_ptr to_ptr(node const& n) {
    return std::make_shared<node>(n);
}

}
}

