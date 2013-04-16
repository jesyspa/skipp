#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>
#include <memory>

namespace skipp {
namespace ski {

extern int counter;

struct variable {
    std::string name;
};
struct number {
    int val;
};
struct combinator {
    char c;
};
struct application;

namespace detail {
    using node_impl = boost::variant<
        variable,
        number,
        combinator,
        application
    >;
}

class node {
    using impl = detail::node_impl;
    // Guaranteed non-null
    std::shared_ptr<impl> ptr;

    static std::shared_ptr<impl> to_ptr(impl const& n) {
        return std::make_shared<impl>(n);
    }

public:
    node() = delete;
    node(detail::node_impl const& n);
    node(variable const&);
    node(number const&);
    node(combinator const&);
    node(application const&);
    node(node const&) = default;
    node(node&&) = default;

    node& operator=(node const&) = default;
    node& operator=(node&&) = default;

    template<typename T>
    T* get();

    template<typename T>
    T const* get() const;

    friend bool operator==(node const&, node const&);

    template<typename T>
    auto apply_visitor(T&& sv) const;
};

struct application {
    node f, x;
};

template<typename T>
T* node::get() {
    return boost::get<T>(ptr.get());
}

template<typename T>
T const* node::get() const {
    return boost::get<T const>(ptr.get());
}

template<typename T>
auto node::apply_visitor(T&& sv) const {
    return boost::apply_visitor(sv, *ptr);
}

std::ostream& operator<<(std::ostream&, node const&);
bool operator==(variable const&, variable const&);
bool operator==(number const&, number const&);
bool operator==(combinator const&, combinator const&);
bool operator==(application const&, application const&);

} // ski
} // skipp
