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
                std::cout << get_app(base).x << '\n';
                return ski::node{ski::application{
                    ski::combinator{'I'},
                    ski::combinator{'I'}}};
            }
            if (v.name == "+") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_app(lhs).x);
                auto rhs_num = get_num(get_app(rhs).x);
                return ski::node{
                    ski::application{
                        ski::combinator{'I'},
                        ski::number{lhs_num.val + rhs_num.val}}};
            }
            if (v.name == "-") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_app(lhs).x);
                auto rhs_num = get_num(get_app(rhs).x);
                return ski::node{
                    ski::application{
                        ski::combinator{'I'},
                        ski::number{lhs_num.val - rhs_num.val}}};
            }
            if (v.name == "<") {
                auto lhs = extract(args);
                auto rhs = extract(args);
                auto lhs_num = get_num(get_app(lhs).x);
                auto rhs_num = get_num(get_app(rhs).x);
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
                auto x = extract(args);
                return ski::node{
                    ski::application{
                        get_app(f).x,
                        ski::application{
                            get_app(g).x,
                            get_app(x).x}}};
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
                            get_app(f).x,
                            get_app(x).x},
                        get_app(g).x}};
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
                            get_app(f).x,
                            get_app(x).x},
                        ski::application{
                            get_app(g).x,
                            get_app(x).x}}};
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
            n = get_app(args.back()).f = *result;
        else
            n = get_app(args.back()).f.update(*result);
    }
    while (!args.empty())
        n = ski::application{n, get_app(extract(args)).x};
    return n;
}

ski::node eval_string(std::string const& s) {
    return eval(string2ski(s));
}

}

