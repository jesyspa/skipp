#pragma once

#include <string>
#include <ostream>
#include <boost/variant.hpp>
#include <memory>

namespace skipp {
namespace ski {

struct value;
struct application;

using node = boost::variant<
    value,
    application
>;

// Despite the fact this being a pointer, we assume non-nullness.
using node_ptr = std::shared_ptr<node>;

struct application {
    node_ptr f, x;
};

struct variable {
    std::string name;
};

// S f g x = f x (g x)
struct s_comb_0 {};
struct s_comb_1 {
    node_ptr f;
};
struct s_comb_2 {
    node_ptr f, g;
};
// K x y = y
struct k_comb_0 {};
struct k_comb_1 {
    node_ptr x;
};
// I x = x
struct i_comb_0 {};

using value_impl = boost::variant<
    variable,
    s_comb_0,
    s_comb_1,
    s_comb_2,
    k_comb_0,
    k_comb_1,
    i_comb_0
>;

struct value {
    value_impl impl;
};

template<typename T>
node_ptr make_value(T const& t) {
    return std::make_shared<node>(value{t});
}

node_ptr make_application(node_ptr const&, node_ptr const&);

std::ostream& operator<<(std::ostream&, node const&);
std::ostream& operator<<(std::ostream&, node_ptr const&);
std::ostream& operator<<(std::ostream&, value const&);
bool operator==(application const&, application const&);
bool operator==(variable const&, variable const&);
bool operator==(s_comb_0 const&, s_comb_0 const&);
bool operator==(s_comb_1 const&, s_comb_1 const&);
bool operator==(s_comb_2 const&, s_comb_2 const&);
bool operator==(k_comb_0 const&, k_comb_0 const&);
bool operator==(k_comb_1 const&, k_comb_1 const&);
bool operator==(i_comb_0 const&, i_comb_0 const&);
bool operator==(value const&, value const&);
node_ptr to_ptr(node const&);

} // ski
} // skipp
