#include "MetaMethodSignature.h"

#include <cassert>

#include "clang/Lex/Token.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"

#include "Tokenizer.h"

namespace
{
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}
}
	
MetaMethodSignature::MetaMethodSignature(std::string declaration, clang::CompilerInstance &instance) :
	mValid(false)
{
	TokenVector tokens = Tokenizer::tokenizeString(declaration, instance);
	mValid = parseConnectCallRefTokens(tokens.cbegin(), tokens.cend());
}

MetaMethodSignature::MetaMethodSignature(const clang::FunctionDecl *decl, clang::CompilerInstance &instance) :
	mValid(false)
{
	mMethodName = decl->getDeclName().getAsString();
	mValid = true;

	// XXX: Handle params
}

bool MetaMethodSignature::parseConnectCallRefTokens(TokenVector::const_iterator token, TokenVector::const_iterator end)
{
	// We need at least an identifier, open bracket and close bracket
	if ((end - token) < 3)
	{
		return false;
	}

	// Must end with )
	if ((end - 1)->isNot(clang::tok::r_paren))
	{
		return false;
	}

	// Start parsing
	if (token->isNot(clang::tok::raw_identifier))
	{
		return false;
	}
	
	mMethodName = identifierString(*token);
	token++;

	// Expect ( after the identifier
	if (token->isNot(clang::tok::l_paren))
	{
		return false;
	}
	
	// XXX: Handle params
	
	return true;
}
	
std::string MetaMethodSignature::spelling() const
{
	if (!isValid())
	{
		return "(invalid)";
	}
	
	std::string ret;

	ret += methodName() + "(";

	// XXX: Handle params

	ret += ")";

	return ret;
}
