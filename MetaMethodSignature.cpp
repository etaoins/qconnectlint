#include "MetaMethodSignature.h"

#include <cassert>

#include "clang/Lex/Token.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Frontend/CompilerInstance.h"

#include "Tokenizer.h"
#include "DeclarationParser.h"

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

	for(auto it = decl->param_begin();
	    it != decl->param_end();
		 it++)
	{
		const clang::ParmVarDecl *paramDecl = *it;
		const clang::SourceManager &sm = instance.getSourceManager();

		bool ateDefault = false;
		clang::SourceRange range = paramDecl->getSourceRange();
		const clang::SourceRange defaultArgRange = paramDecl->getDefaultArgRange();

		// Chop off the default arg
		if (defaultArgRange.isValid())
		{
			ateDefault = true;
			range.getEnd() = defaultArgRange.getBegin().getLocWithOffset(-1);
		}

		const std::string sourceText(
			sm.getCharacterData(range.getBegin()),
			sm.getCharacterData(range.getEnd()));

		TokenVector tokens = Tokenizer::tokenizeString(sourceText, instance);

		if (ateDefault && !tokens.empty() && (tokens.back().is(clang::tok::equal)))
		{
			// Chop of the trailing =
			tokens.resize(tokens.size() - 1);
		}
		
		TokenVector::const_iterator token = tokens.begin();
		TypeDeclaration typeDecl = DeclarationParser::consumeType(token, tokens.end(), true);; 

		if (!typeDecl.isValid())
		{
			return;
		}

		mArguments.push_back(typeDecl);
	}

	mValid = true;
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

	// Stop parsing an argument when we hit a comma or )
	TokenKindSet stopTokens({clang::tok::comma, clang::tok::r_paren});

	for(token++; token->isNot(clang::tok::r_paren) && (token != end); token++)
	{
		TypeDeclaration typeDecl = DeclarationParser::consumeType(token, end, false, stopTokens); 

		if (!typeDecl.isValid())
		{
			return false;
		}

		mArguments.push_back(typeDecl);
	}
	
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

	for(auto it = mArguments.begin();
	    it != mArguments.end();
	    it++)
	{
		ret += it->spelling();

		if ((it + 1) != mArguments.end())
		{
			ret += ", ";
		}
	}

	ret += ")";

	return ret;
}
