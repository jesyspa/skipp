#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>
#include <memory>

namespace skipp {
namespace ski {

extern int counter;

struct application;
struct variable {
    std::string name;
};

namespace detail {
    using node_impl = boost::variant<
        variable,
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
    node(application const& n);
    node(variable const&);
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

    void update(node const&) const;
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
bool operator==(application const&, application const&);
bool operator==(variable const&, variable const&);

} // ski
} // skipp
