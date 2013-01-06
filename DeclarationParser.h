#ifndef _DECLARATIONPARSER_H
#define _DECLARATIONPARSER_H

#include <set>

#include "Tokenizer.h"
#include "TypeDeclaration.h"

typedef std::set<clang::tok::TokenKind> TokenKindSet;

namespace DeclarationParser
{
	TypeDeclaration consumeType(TokenVector::const_iterator &begin, TokenVector::const_iterator end, bool allowVarDecl, const TokenKindSet &stopTokens = TokenKindSet());
}

#endif
