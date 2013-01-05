#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <vector>
#include <string>

#include "clang/Lex/Token.h"

namespace clang
{
	class CompilerInstance;
}

typedef std::vector<clang::Token> TokenVector;

namespace Tokenizer
{
	TokenVector tokenizeString(const std::string &string, clang::CompilerInstance &instance);
}

#endif
