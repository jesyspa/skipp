#pragma once

#include "lambda_ast.hpp"
#include "ski.hpp"
#include <string>

namespace skipp {
namespace lambda2ski {

ski::node_ptr lambda2ski(lambda_ast::tree const&);
ski::node_ptr string2ski(std::string const&);

}
}
