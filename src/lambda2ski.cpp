#include "lambda2ski.hpp"
#include "parser.hpp"

namespace skipp {
namespace lambda2ski {

struct compile : boost::static_visitor<ski::node> {
    ski::node operator()(lambda_ast::variable const&) const;
    ski::node operator()(lambda_ast::lambda const&) const;
    ski::node operator()(lambda_ast::application const&) const;
};

struct remove_parameter : boost::static_visitor<ski::node> {
    std::string name;

    ski::node operator()(lambda_ast::variable const&) const;
    ski::node operator()(lambda_ast::lambda const&) const;
    ski::node operator()(lambda_ast::application const&) const;
};

ski::node compile::operator()(lambda_ast::variable const& var) const {
    return ski::value{ski::variable{var.name}};
}

ski::node compile::operator()(lambda_ast::lambda const&) const {
    return {};
}

ski::node compile::operator()(lambda_ast::application const& app) const {
    return ski::application{
        to_ptr(boost::apply_visitor(*this, app.f)),
        to_ptr(boost::apply_visitor(*this, app.x))
    };
}

ski::node lambda2ski(lambda_ast::tree const& tree) {
    return boost::apply_visitor(compile{}, tree);
}

ski::node string2ski(std::string const& s) {
    return lambda2ski(parser::parse(s));
}

}
}
