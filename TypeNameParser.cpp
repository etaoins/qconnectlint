#include "TypeNameParser.h"

#include "clang/Lex/Token.h"
	
namespace
{
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}
}

namespace TypeNameParser
{
	std::string consumeTypeName(TokenVector::const_iterator &token, TokenVector::const_iterator end)
	{
		std::string typeNameStr;

		int templateDepth = 0;
		clang::tok::TokenKind lastKind(clang::tok::unknown);
		
		do
		{
			if (token->is(clang::tok::colon) && 
				((lastKind == clang::tok::unknown) || (lastKind == clang::tok::raw_identifier) || (lastKind == clang::tok::less)))
			{
				typeNameStr += "::";
				token++;

				// The next token must also be a colon
				if ((token == end) || token->isNot(clang::tok::colon))
				{
					// Not valid
					return std::string();
				}
			}
			else if (token->is(clang::tok::less))
			{
				// Starting a templated parameter
				typeNameStr += "<";
				templateDepth++;
			}
			else if (token->is(clang::tok::greater))
			{
				typeNameStr += ">";
				templateDepth--;

				if (templateDepth < 0)
				{
					// We closed a template parameter list that wasn't open
					return std::string();
				}
			}
			else if (token->is(clang::tok::raw_identifier) && 
			         (lastKind != clang::tok::raw_identifier))
			{
				typeNameStr += identifierString(*token);
			}
			else
			{
				// Something else
				break;
			}

			// Store this kind for the next iteration
			lastKind = token->getKind();
			token++;
		}
		while(token != end);

		if (templateDepth > 1)
		{
			// Template parameter list unclosed
			return std::string();
		}

		return typeNameStr;
	}
}
