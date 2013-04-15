#pragma once

#include "ski.hpp"
#include <string>

namespace skipp {
namespace evaluate {

ski::node eval(ski::node p);
ski::node eval_string(std::string const& s);

}
}
