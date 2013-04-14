#pragma once

#include "token.hpp"
#include <vector>

using tokens = std::vector<token>;
tokens lex(std::string const&);
