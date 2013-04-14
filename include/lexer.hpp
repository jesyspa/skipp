#pragma once

#include "token.hpp"
#include <vector>

namespace skipp {
namespace lexer {

using token::tokens;
tokens lex(std::string const&);

} // lexer
} // skipp
