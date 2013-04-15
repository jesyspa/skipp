#include "evaluate.hpp"
#include "lambda2ski.hpp"
#include <boost/optional.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>

namespace {
    using namespace skipp;

    using result = boost::optional<ski::node>;
    using arg_vector = std::vector<ski::node>;

    ski::node extract(arg_vector& vec) {
        auto result = vec.back();
        vec.pop_back();
        return result;
    }

    ski::application& get_app(ski::node& n) {
        if (auto* p = n.get<ski::application>())
            return *p;
        else
            throw std::logic_error("non-application in arg vector");
    }

    ski::application const& get_app(ski::node const& n) {
        if (auto* p = n.get<ski::application>())
            return *p;
        else
            throw std::logic_error("non-application in arg vector");
    }

    struct do_evaluate : boost::static_visitor<result> {
        arg_vector& args;
        do_evaluate(arg_vector& args) : args(args) {}

        result operator()(ski::variable const& v) const {
            if (args.empty())
                return {};
            if (v.name == "print") {
                auto base = extract(args);
                auto& app = get_app(base);
                std::cout << app.x << '\n';
            app.f = ski::combinator{'I'};
            app.x = ski::combinator{'I'};
            return base;
            }
            return {};
        }

        result operator()(ski::combinator const& comb) const {
            if (args.empty())
                return {};
            switch (comb.c) {
            case 'I':
                return get_app(extract(args)).x;

            case 'K': {
                if (args.size() < 2)
                    return {};
                auto result = extract(args);
                extract(args);
                return get_app(result).x;
            }

            case 'R': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto base = extract(args);
                auto& app = get_app(base);
                auto x = app.x;
                app.f = get_app(f).x;
                app.x = ski::application{get_app(g).x, x};
                return base;
            }

            case 'L': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto base = extract(args);
                auto& app = get_app(base);
                auto x = app.x;
                app.f = ski::application{get_app(f).x, x};
                app.x = get_app(g).x;
                return base;
            }

            case 'S': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto base = extract(args);
                auto& app = get_app(base);
                auto x = app.x;
                app.f = ski::application{get_app(f).x, x};
                app.x = ski::application{get_app(g).x, x};
                return base;
            }

            default:
                throw std::logic_error("unknown combinator");
            }
        }

        result operator()(ski::application const&) const {
            throw std::logic_error("trying to do_evaluate an app");
        }
    };
}

namespace skipp {

ski::node eval(ski::node n) {
    arg_vector args;
    while (true) {
        while (auto* app = n.get<ski::application>()) {
            args.push_back(n);
            n = app->f;
        }

        if (auto result = boost::apply_visitor(do_evaluate(args), n))
            n = *result;
        else
            break;
    }
    while (!args.empty())
        n = ski::application{n, get_app(extract(args)).x};
    return n;
}

ski::node eval_string(std::string const& s) {
    return eval(string2ski(s));
}

}

