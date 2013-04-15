#pragma once

#include "lambda_ast.hpp"
#include "ski.hpp"
#include <string>

namespace skipp {

ski::node lambda2ski(lambda_ast::tree const&);
ski::node string2ski(std::string const&);

}
