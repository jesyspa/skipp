#pragma once

#include <vector>
#include <string>

#include "token.hpp"
#include "lambda_ast.hpp"

namespace skipp {
namespace parser {

using tree = lambda_ast::tree;
tree parse(token::tokens const&);

tree parse(std::string const&);

}
}
