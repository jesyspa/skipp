#include "lambda2ski.hpp"
#include "parser.hpp"
#include <algorithm>
#include <stdexcept>
#include <set>
#include <iterator>

namespace skipp {
namespace lambda2ski {

struct used_variables : boost::static_visitor<std::set<std::string>> {
    std::set<std::string> operator()(lambda_ast::variable const& var) const {
        return {var.name};
    }

    std::set<std::string> operator()(lambda_ast::application const& app) const {
        auto f = boost::apply_visitor(*this, app.f);
        auto x = boost::apply_visitor(*this, app.x);
        std::copy(x.begin(), x.end(), std::inserter(f, f.begin()));
        return f;
    }

    std::set<std::string> operator()(lambda_ast::lambda const& lam) const {
        auto vars = boost::apply_visitor(*this, lam.body);
        auto it = vars.find(lam.param);
        if (it != vars.end())
            vars.erase(it);
        return vars;
    }
};

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

    lambda_ast::tree vars_helper(bool has, lambda_ast::tree const& tree) const {
        if (has)
            return boost::apply_visitor(*this, tree);
        return tree;
    }

    lambda_ast::tree operator()(lambda_ast::application const& app) const {
        auto const vars_f = boost::apply_visitor(used_variables{}, app.f);
        auto const vars_g = boost::apply_visitor(used_variables{}, app.x);
        bool const in_f = vars_f.count(name);
        bool const in_g = vars_g.count(name);
        auto const f = vars_helper(in_f, app.f);
        auto const g = vars_helper(in_g, app.x);
        auto const var = lambda_ast::variable{
            in_f ? (in_g ? "$S" : "$L") : "$R"
        };
        if (in_f || in_g)
            return lambda_ast::application{
                lambda_ast::application{var, f},
                g
            };
        return lambda_ast::application{
            lambda_ast::variable{"$K"},
            lambda_ast::application{f, g}
        };
    }
};

lambda_ast::tree
compile::operator()(lambda_ast::lambda const& lam) const {
    return boost::apply_visitor(remove_parameter(lam.param), lam.body);
}

struct to_ski : boost::static_visitor<ski::node> {
    ski::node operator()(lambda_ast::variable const& var) const {
        if (var.name[0] == '$')
            return ski::combinator{var.name.at(1)};
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
