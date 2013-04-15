#include "evaluate.hpp"
#include "lambda2ski.hpp"
#include <boost/optional.hpp>
#include <stdexcept>
#include <stack>
#include <string>
#include <sstream>

namespace skipp {
namespace evaluate {

using result = boost::optional<ski::node>;

bool can_evaluate(ski::node const& n);
result do_evaluate(ski::node const& f, ski::node const& x);

struct eval_one : boost::static_visitor<boost::optional<ski::node>> {
    ski::node arg;

    eval_one(ski::node const& n) : arg(n) {}

    result operator()(ski::variable const&) {
        throw std::runtime_error("trying to evaluate variable");
    }

    result operator()(ski::application const& app) {
        return do_evaluate(eval(app), arg);
    }

    result operator()(ski::s_comb_0 const&) {
        return ski::node(ski::s_comb_1{arg});
    }

    result operator()(ski::s_comb_1 const& s) {
        return ski::node(ski::s_comb_2{s.f, arg});
    }

    result operator()(ski::s_comb_2 const& s) {
        return ski::node(ski::application{ski::application{s.f, arg},
                                         ski::application{s.g, arg}});
    }

    result operator()(ski::k_comb_0 const&) {
        return ski::node(ski::k_comb_1{arg});
    }

    result operator()(ski::k_comb_1 const& k) {
        return k.x;
    }

    result operator()(ski::i_comb_0 const&) {
        return arg;
    }
};

bool can_evaluate(ski::node const& n) {
    return !n.get<ski::variable>();
}

result do_evaluate(ski::node const& f, ski::node const& x) {
    if (!can_evaluate(f))
        return {};
    return f.apply_visitor(eval_one{x});
}

ski::node eval(ski::node n) {
    while (auto* app = n.get<ski::application>()) {
        if (auto result = do_evaluate(app->f, app->x))
            n = *result;
        else
            break;
    }
    return n;
}

ski::node eval_string(std::string const& s) {
    return eval(lambda2ski::string2ski(s));
}

}
}

