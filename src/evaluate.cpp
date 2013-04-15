#include "evaluate.hpp"
#include "lambda2ski.hpp"
#include <boost/optional.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>

namespace skipp {
namespace evaluate {

using result = boost::optional<ski::node>;
using arg_vector = std::vector<ski::node>;

bool can_evaluate(ski::node const& n);
result do_evaluate(ski::node const& f, ski::node const& x);

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

bool can_evaluate(ski::node const& n) {
    return !n.get<ski::variable>();
}

result do_evaluate(ski::variable const& v, arg_vector& args) {
    if (args.empty())
        return {};
    if (v.name == "$I") {
        return get_app(extract(args)).x;
    }
    if (v.name == "$K") {
        if (args.size() < 2)
            return {};
        auto result = extract(args);
        extract(args);
        return get_app(result).x;
    }
    if (v.name == "$R") {
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
    if (v.name == "$L") {
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
    if (v.name == "$S") {
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
    if (v.name == "print") {
        auto base = extract(args);
        auto& app = get_app(base);
        std::cout << app.x << '\n';
        app.f = ski::variable{"$I"};
        app.x = ski::variable{"$I"};
        return base;
    }
    return {};
}


std::ostream& operator<<(std::ostream& o, arg_vector const& v) {
    bool b = false;
    o << '{';
    for (auto const& e : v) {
        if (b)
            o << ", ";
        o << e;
        b = true;
    }
    o << '}';
    return o;
}

ski::node eval(ski::node n) {
    arg_vector args;
    while (true) {
        while (auto* app = n.get<ski::application>()) {
            args.push_back(n);
            n = app->f;
        }
        // Seeing as it isn't an app, it's a variable
        auto var = n.get<ski::variable>();
        if (!var)
            throw std::logic_error("neither app nor var");

        if (auto result = do_evaluate(*var, args))
            n = *result;
        else
            break;
    }
    while (!args.empty())
        n = ski::application{n, get_app(extract(args)).x};
    return n;
}

ski::node eval_string(std::string const& s) {
    return eval(lambda2ski::string2ski(s));
}

}
}

