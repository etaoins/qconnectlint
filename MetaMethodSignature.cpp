#include "MetaMethodSignature.h"

#include <cassert>

#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "llvm/Support/MemoryBuffer.h"

#include "clang/Frontend/CompilerInstance.h"

namespace
{
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}

	TokenVector tokenizeString(const std::string &string, clang::CompilerInstance &instance)
	{
		// Try to lex it
		llvm::MemoryBuffer *buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(string));
		clang::FileID fileId = instance.getSourceManager().createFileIDForMemBuffer(buffer);

		clang::Lexer lex(fileId, buffer, instance.getSourceManager(), clang::LangOptions());
		lex.SetKeepWhitespaceMode(false);

		// Fetch all of the tokens
		TokenVector tokens;
		while(true)
		{
			clang::Token token;
			lex.LexFromRawLexer(token);

			if (token.getKind() == clang::tok::eof)
			{
				// We're done
				break;
			}

			tokens.push_back(token);
		}

		return tokens;
	}
}
	
MetaMethodSignature::MetaMethodSignature(std::string declaration, clang::CompilerInstance &instance) :
	mValid(false)
{
	TokenVector tokens = tokenizeString(declaration, instance);
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

		TokenVector tokens = tokenizeString(sourceText, instance);

		if (ateDefault && !tokens.empty() && (tokens.back().is(clang::tok::equal)))
		{
			// Chop of the trailing =
			tokens.resize(tokens.size() - 1);
		}

		MetaMethodArgument arg = MetaMethodArgument(tokens.cbegin(), tokens.cend(), ArgumentParseMode::Declaration);

		if (!arg.isValid())
		{
			return;
		}

		mArguments.push_back(arg);
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
	token++;

	// Keep going until we get )
	while(token->isNot(clang::tok::r_paren))
	{
		TokenVector::const_iterator argLookAhead = token;

		// Look ahead until comma or )
		for(;
		    argLookAhead->isNot(clang::tok::r_paren) && argLookAhead->isNot(clang::tok::comma);
			 argLookAhead++)
		{
		}

		if (token == argLookAhead)
		{
			// We hit two commas in a row
			return false;
		}
		else
		{
			// We have an argument candidate
			MetaMethodArgument arg = MetaMethodArgument(token, argLookAhead, ArgumentParseMode::ConnectCall);

			if (!arg.isValid())
			{
				return false;
			}

			// Add the argument on and continue parsing
			mArguments.push_back(arg);
			token = argLookAhead;

			if (token->is(clang::tok::r_paren))
			{
				// We're done
				break;
			}
			token++;
		}
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
