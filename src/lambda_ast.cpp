#include "lambda_ast.hpp"

namespace skipp {
namespace lambda_ast {

struct print_tree : boost::static_visitor<> {
    std::ostream& o;
    print_tree(std::ostream& o) : o(o) {}

    void operator()(variable const& v) const {
        o << v.name;
    }

    void operator()(lambda const& lam) const {
        o << "(\\" << lam.param << ". ";
        boost::apply_visitor(*this, lam.body);
        o << ")";
    }

    void operator()(application const& app) const {
        o << "(";
        boost::apply_visitor(*this, app.f);
        o << " ";
        boost::apply_visitor(*this, app.x);
        o << ")";
    }
};

std::ostream& operator<<(std::ostream& o, tree const& tr) {
    boost::apply_visitor(print_tree(o), tr);
    return o;
}

bool operator==(variable const& lhs, variable const& rhs) {
    return lhs.name == rhs.name;
}

bool operator==(lambda const& lhs, lambda const& rhs) {
    return lhs.param == rhs.param && lhs.body == rhs.body;
}

bool operator==(application const& lhs, application const& rhs) {
    return lhs.f == rhs.f && lhs.x == rhs.x;
}

} // ast
} // skipp
