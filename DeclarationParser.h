#ifndef _DECLARATIONPARSER_H
#define _DECLARATIONPARSER_H

#include <set>

#include "Tokenizer.h"
#include "TypeDeclaration.h"

typedef std::set<clang::Token> TokenSet;

namespace DeclarationParser
{
	TypeDeclaration consumeType(TokenVector::const_iterator &begin, TokenVector::const_iterator end, bool allowIdentifier, const TokenSet &stopTokens = TokenSet());
}

#endif
