#pragma once

#include <vector>
#include <string>

#include "token.hpp"
#include "lambda_ast.hpp"

namespace skipp {

lambda_ast::tree parse(token::tokens const&);
lambda_ast::tree parse(std::string const&);

}
