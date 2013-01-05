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
