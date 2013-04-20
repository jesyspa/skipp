#include "evaluate.hpp"
#include "lambda2ski.hpp"
#include <boost/optional.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

//#define WARN

void warn() {
#ifdef WARN
    std::cin.ignore();
#endif
}

template<typename T, typename... ARGS>
void warn(T t, ARGS... args) {
#ifdef WARN
    std::cout << t << ' ';
#else
    (void)t;
#endif
    warn(args...);
}

namespace {
    using namespace skipp;

    using result = boost::optional<ski::node>;
    using arg_vector = std::vector<ski::node>;

    ski::node extract(arg_vector& vec) {
        auto result = vec.back();
        vec.pop_back();
        return result;
    }

    ski::application& get_app(ski::node const& n) {
        if (auto* p = n.get<ski::application>())
            return *p;
        else
            throw std::logic_error("non-application in arg vector");
    }

    ski::node& get_fun(ski::node const& n) {
        return get_app(n).f;
    }

    ski::node& get_arg(ski::node const& n) {
        return get_app(n).x;
    }

    ski::number const& get_num(ski::node const& n) {
        auto val = eval(n);
        if (auto p = val.get<ski::number>())
            return *p;
        throw std::runtime_error("type error in lhs");
    }

    struct do_evaluate : boost::static_visitor<result> {
        arg_vector& args;
        do_evaluate(arg_vector& args) : args(args) {}

        result operator()(ski::variable const& v) const {
            if (args.empty())
                return {};
            if (v.name == "print") {
                auto base = extract(args);
                std::cout << get_arg(base) << '\n';
                return ski::node{
                    ski::combinator{'I'}};
            }
            if (v.name == "+" || v.name == "add") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_arg(lhs));
                auto rhs_num = get_num(get_arg(rhs));
                return ski::node{
                    ski::application{
                        ski::combinator{'I'},
                        ski::number{lhs_num.val + rhs_num.val}}};
            }
            if (v.name == "-" || v.name == "sub") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_arg(lhs));
                auto rhs_num = get_num(get_arg(rhs));
                return ski::node{
                    ski::application{
                        ski::combinator{'I'},
                        ski::number{lhs_num.val - rhs_num.val}}};
            }
            if (v.name == "<=" || v.name == "le") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_arg(lhs));
                auto rhs_num = get_num(get_arg(rhs));
                if (lhs_num.val < rhs_num.val)
                    return ski::node{
                        ski::application{
                            ski::application{
                                ski::combinator{'R'},
                                ski::combinator{'K'}},
                            ski::combinator{'I'}}};
                else
                    return ski::node{
                        ski::application{
                            ski::combinator{'K'},
                            ski::combinator{'I'}}};
            }
            return {};
        }

        result operator()(ski::number const&) const {
            return {};
        }

        result operator()(ski::combinator const& comb) const {
            if (args.empty())
                return {};
            switch (comb.c) {
            case 'I':
                return get_arg(extract(args));

            case 'K': {
                if (args.size() < 2)
                    return {};
                auto result = extract(args);
                extract(args);
                return get_arg(result);
            }

            case 'R': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto x = extract(args);
                return ski::node{
                    ski::application{
                        get_arg(f),
                        ski::application{
                            get_arg(g),
                            get_arg(x)}}};
            }

            case 'L': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto x = extract(args);
                return ski::node{
                    ski::application{
                        ski::application{
                            get_arg(f),
                            get_arg(x)},
                        get_arg(g)}};
            }

            case 'S': {
                if (args.size() < 3)
                    return {};
                auto f = extract(args);
                auto g = extract(args);
                auto x = extract(args);
                return ski::node{
                    ski::application{
                        ski::application{
                            get_arg(f),
                            get_arg(x)},
                        ski::application{
                            get_arg(g),
                            get_arg(x)}}};
            }

            default:
                throw std::logic_error("unknown combinator");
            }
        }

        result operator()(ski::application const&) const {
            throw std::logic_error("trying to do_evaluate an app");
        }
    };

    bool is_identity(ski::node const& n) {
        if (auto* p = n.get<ski::combinator>())
            return p->c == 'I';
        return false;
    }
}

namespace skipp {

ski::node eval(ski::node const& n_orig) {
    arg_vector args;
    auto n = n_orig;
    while (true) {
        warn(n_orig);
        while (auto* app = n.get<ski::application>()) {
            args.push_back(n);
            warn("back:", args.back());
            n = app->f;
        }

        bool id = is_identity(n);

        auto result = boost::apply_visitor(do_evaluate(args), n);
        if (!result)
            break;
        if (args.empty())
            n = n_orig.update(*result);
        else if (id)
            n = get_fun(args.back()) = *result;
        else if (result->get<ski::application>())
            n = get_fun(args.back()).update(*result);
        else
            n = get_fun(args.back()).update(ski::application{
                    ski::combinator{'I'},
                    *result});
    }
    while (!args.empty())
        n = ski::application{n, get_arg(extract(args))};
    return n;
}

ski::node eval_string(std::string const& s) {
    return eval(string2ski(s));
}

}

