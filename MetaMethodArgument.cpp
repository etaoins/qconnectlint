#include "MetaMethodArgument.h"

#include <string>
#include <vector>

#include "clang/Lex/Token.h"

namespace
{
	enum class ArgTokenKind
	{
		Const,
		Star,
		Amp,
		TypeName
	};

	struct ArgToken
	{
		explicit ArgToken(ArgTokenKind kind, std::string string = std::string()) :
			mKind(kind),
			mString(string)
		{
		}

		ArgTokenKind kind() const { return mKind; }
		std::string string() const { return mString; }

		bool is(ArgTokenKind expected) const { return kind() == expected; }

	private:
		ArgTokenKind mKind;
		std::string mString;
	};
	
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}

	std::string consumeTypeName(TokenVector::const_iterator &token, TokenVector::const_iterator end)
	{
		std::string typeNameStr;

		int templateDepth = 0;
		bool colonColonAllowed = true;
		
		do
		{
			if (token->is(clang::tok::colon) && colonColonAllowed)
			{
				typeNameStr += "::";
				token++;

				// The next token must also be a colon
				if ((token == end) || token->isNot(clang::tok::colon))
				{
					// Not valid
					return std::string();
				}

				token++;

				colonColonAllowed = false;
			}
			else if (token->is(clang::tok::less))
			{
				// Starting a templated parameter
				typeNameStr += "<";
				templateDepth++;
				token++;
				
				colonColonAllowed = true;
			}
			else if (token->is(clang::tok::greater))
			{
				typeNameStr += ">";
				templateDepth--;
				token++;

				if (templateDepth < 0)
				{
					// We closed a template parameter list that wasn't open
					return std::string();
				}
				
				colonColonAllowed = false;
			}
			else if (token->is(clang::tok::raw_identifier))
			{
				typeNameStr += identifierString(*token);
				token++;
				
				colonColonAllowed = true;
			}
			else
			{
				// Something else
				break;
			}
		}
		while(token != end);

		if (templateDepth > 1)
		{
			// Template parameter list unclosed
			return std::string();
		}

		return typeNameStr;
	}

	// Converts Clang tokens in to higher-level tokens
	std::vector<ArgToken> retokenize(TokenVector::const_iterator begin, TokenVector::const_iterator end)
	{
		std::vector<ArgToken> argTokens;

		auto token = begin;
		while(token != end)
		{
			if (token->is(clang::tok::star))
			{
				argTokens.push_back(ArgToken(ArgTokenKind::Star));
				token++;
			}
			else if (token->is(clang::tok::amp))
			{
				argTokens.push_back(ArgToken(ArgTokenKind::Amp));
				token++;
			}
			else if (token->is(clang::tok::raw_identifier))
			{
				const std::string identStr(identifierString(*token));

				if (identStr == std::string("const"))
				{
					argTokens.push_back(ArgToken(ArgTokenKind::Const));
					token++;
				}
				else
				{
					// Treat this as a type name
					std::string typeNameStr = consumeTypeName(token, end);

					if (typeNameStr.empty())
					{
						// Couldn't parse the type name
						return std::vector<ArgToken>();
					}

					argTokens.push_back(ArgToken(ArgTokenKind::TypeName, typeNameStr));
				}
			}
			else
			{
				// Don't know this token
				return std::vector<ArgToken>();
			}
		}

		return argTokens;
	}
}
	
MetaMethodArgument::MetaMethodArgument(TokenVector::const_iterator begin, TokenVector::const_iterator end, ArgumentParseMode mode) : 
	mBasicType(BasicType::Invalid),
	mConst(false)
{
	if (begin != end)
	{
		parse(begin, end);

		// Could we not parse and we're in declaration mode?
		if (!isValid() & (mode == ArgumentParseMode::Declaration))
		{
			TokenVector::const_iterator argumentName = (end - 1);

			// Is the end an identifier?
			if (argumentName->is(clang::tok::raw_identifier))
			{
				// Try parsing again without the trailing identifier
				parse(begin, argumentName);
			}
		}
	}
}
	
void MetaMethodArgument::parse(TokenVector::const_iterator begin, TokenVector::const_iterator end)
{
	const std::vector<ArgToken> argTokens = retokenize(begin, end);

	// This is really, really dumb
	if ((argTokens.size() == 1) && argTokens[0].is(ArgTokenKind::TypeName))
	{
		mBasicType = BasicType::Value;
		mTypeIdentifier = argTokens[0].string();
	}
	else if ((argTokens.size() == 2) && argTokens[0].is(ArgTokenKind::TypeName))
	{
		if (argTokens[1].is(ArgTokenKind::Star))
		{
			mBasicType = BasicType::Pointer;
			mTypeIdentifier = argTokens[0].string();
		}
		else if (argTokens[1].is(ArgTokenKind::Amp))
		{
			mBasicType = BasicType::Reference;
			mTypeIdentifier = argTokens[0].string();
		}
	}
	else if (argTokens.size() == 3) 
	{
		if (argTokens[0].is(ArgTokenKind::Const) &&
			 argTokens[1].is(ArgTokenKind::TypeName) &&
			 argTokens[2].is(ArgTokenKind::Amp))
		{
			mBasicType = BasicType::Value;
			mTypeIdentifier = argTokens[1].string();
		}
		else if (argTokens[0].is(ArgTokenKind::Const) &&
			      argTokens[1].is(ArgTokenKind::Amp) &&
			      argTokens[2].is(ArgTokenKind::TypeName))
		{
			mBasicType = BasicType::Value;
			mTypeIdentifier = argTokens[2].string();
		}
		else if (argTokens[0].is(ArgTokenKind::Const) &&
			      argTokens[1].is(ArgTokenKind::TypeName) &&
			      argTokens[2].is(ArgTokenKind::Star))
		{
			mBasicType = BasicType::Pointer;
			mTypeIdentifier = argTokens[1].string();
			mConst = true;
		}
	}
}
	
std::string MetaMethodArgument::spelling() const
{
	if (!isValid())
	{
		return "(invalid)";
	}

	std::string ret;

	if (isConst())
	{
		ret += "const ";
	}

	ret += typeIdentifier();

	switch(basicType())
	{
	case BasicType::Pointer:
		ret += " *";
		break;

	case BasicType::Reference:
		ret += " &";
		break;

	case BasicType::Invalid:
		// Fall through
	case BasicType::Value:
		break;
	}

	return ret;
}
