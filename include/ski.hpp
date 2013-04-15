#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>
#include <memory>

namespace skipp {
namespace ski {

struct application;
struct variable {
    std::string name;
};

// S f g x = f x (g x)
struct s_comb_0 {};
struct s_comb_1;
struct s_comb_2;
// K x y = y
struct k_comb_0 {};
struct k_comb_1;
// I x = x
struct i_comb_0 {};

namespace detail {
    using node_impl = boost::variant<
        variable,
        s_comb_0,
        boost::recursive_wrapper<s_comb_1>,
        boost::recursive_wrapper<s_comb_2>,
        k_comb_0,
        boost::recursive_wrapper<k_comb_1>,
        i_comb_0,
        boost::recursive_wrapper<application>
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
    node(s_comb_0 const&);
    node(s_comb_1 const&);
    node(s_comb_2 const&);
    node(k_comb_0 const&);
    node(k_comb_1 const&);
    node(i_comb_0 const&);
    node(node const&) = default;
    node(node&&) = default;

    node& operator=(node const&) = default;
    node& operator=(node&&) = default;

    template<typename T>
    T* get() {
        return boost::get<T>(ptr.get());
    }

    template<typename T>
    T const* get() const {
        return boost::get<T const>(ptr.get());
    }

    friend bool operator==(node const&, node const&);

    template<typename T>
    auto apply_visitor(T&& sv) {
        return boost::apply_visitor(sv, *ptr);
    }

    template<typename T>
    auto apply_visitor(T&& sv) const {
        return boost::apply_visitor(sv, const_cast<impl const&>(*ptr));
    }
};

struct application {
    node f, x;
};
struct s_comb_1 {
    node f;
};
struct s_comb_2 {
    node f, g;
};
struct k_comb_1 {
    node x;
};

std::ostream& operator<<(std::ostream&, node const&);
bool operator==(application const&, application const&);
bool operator==(variable const&, variable const&);
bool operator==(s_comb_0 const&, s_comb_0 const&);
bool operator==(s_comb_1 const&, s_comb_1 const&);
bool operator==(s_comb_2 const&, s_comb_2 const&);
bool operator==(k_comb_0 const&, k_comb_0 const&);
bool operator==(k_comb_1 const&, k_comb_1 const&);
bool operator==(i_comb_0 const&, i_comb_0 const&);

} // ski
} // skipp
