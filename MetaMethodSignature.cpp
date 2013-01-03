#include "MetaMethodSignature.h"

#include <cassert>

#include "clang/Lex/Token.h"

namespace
{
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}
}
	
MetaMethodSignature::MetaMethodSignature(const TokenVector &tokens, ArgumentParseMode mode) :
	mValid(false)
{
	// We need at least an identifier, open bracket and close bracket
	if (tokens.size() < 3)
	{
		return;
	}

	// Must end with )
	if (tokens.back().isNot(clang::tok::r_paren))
	{
		return;
	}

	// Start parsing
	TokenVector::const_iterator token = tokens.cbegin();

	if (token->isNot(clang::tok::raw_identifier))
	{
		return;
	}
	
	mMethodName = identifierString(*token);
	token++;

	// Expect ( after the identifier
	if (token->isNot(clang::tok::l_paren))
	{
		return;
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
			return;
		}
		else
		{
			// We have an argument candidate
			MetaMethodArgument arg = MetaMethodArgument(token, argLookAhead, mode);

			if (!arg.isValid())
			{
				return;
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

	mValid = true;
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
