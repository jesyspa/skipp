#include "lambda2ski.hpp"
#include "parser.hpp"
#include <stdexcept>

namespace skipp {
namespace lambda2ski {

struct compile : boost::static_visitor<lambda_ast::tree> {
    lambda_ast::tree operator()(lambda_ast::variable const& var) const {
        return var;
    }

    lambda_ast::tree operator()(lambda_ast::application const& app) const {
        return lambda_ast::application{
            boost::apply_visitor(*this, app.f),
            boost::apply_visitor(*this, app.x)
        };
    }

    lambda_ast::tree operator()(lambda_ast::lambda const&) const;
};

struct remove_parameter : boost::static_visitor<lambda_ast::tree> {
    remove_parameter(std::string const& s) : name(s) {}
    std::string name;

    lambda_ast::tree operator()(lambda_ast::variable const& var) const {
        if (var.name == name)
            return lambda_ast::variable{"$I"};
        return lambda_ast::application{
            lambda_ast::variable{"$K"},
            lambda_ast::variable{var.name}
        };
    }

    lambda_ast::tree operator()(lambda_ast::lambda const& lam) const {
        auto part = boost::apply_visitor(
            remove_parameter(lam.param),
            lam.body
        );
        return boost::apply_visitor(*this, part);
    }

    lambda_ast::tree operator()(lambda_ast::application const& app) const {
        auto f = boost::apply_visitor(*this, app.f);
        auto g = boost::apply_visitor(*this, app.x);
        return lambda_ast::application{
            lambda_ast::application{
                lambda_ast::variable{"$S"},
                f
            },
            g
        };
    }
};

lambda_ast::tree
compile::operator()(lambda_ast::lambda const& lam) const {
    return boost::apply_visitor(remove_parameter(lam.param), lam.body);
}

struct to_ski : boost::static_visitor<ski::node> {
    ski::node operator()(lambda_ast::variable const& var) const {
        return ski::variable{var.name};
    }

    ski::node operator()(lambda_ast::lambda const&) const {
        throw std::runtime_error("unreduced lambda");
    }

    ski::node operator()(lambda_ast::application const& app) const {
        return ski::application{
            boost::apply_visitor(*this, app.f),
            boost::apply_visitor(*this, app.x)
        };
    }
};

ski::node lambda2ski(lambda_ast::tree const& tree) {
    auto compiled = boost::apply_visitor(compile{}, tree);
    return boost::apply_visitor(to_ski{}, compiled);
}

ski::node string2ski(std::string const& s) {
    return lambda2ski(parser::parse(s));
}

}
}
