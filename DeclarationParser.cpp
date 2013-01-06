#include <utility>

#include "DeclarationParser.h"

namespace 
{
	struct BaseType
	{
		BaseType() : isConst(false)
		{
		}

		bool isValid() const
		{
			return !typeName.empty();
		}

		bool isConst;
		std::string typeName;
	};
	
	std::string identifierString(const clang::Token &token)
	{
		return std::string(token.getRawIdentifierData(), token.getLength());
	}

	// Consumes an entire template parameter and any nested parameters
	// "token" should point to the opening <
	// This doesn't do any parsing; it just reconstructs the string from tokens until the parameter list is over
	std::string consumeTemplateParam(TokenVector::const_iterator &token, TokenVector::const_iterator end)
	{
		int templateDepth = 0;
		std::string ret;

		if ((token == end) || !token->is(clang::tok::less))
		{
			// Not what we expected
			return std::string();
		}

		while(token != end)
		{
			if (token->is(clang::tok::less))
			{
				ret += "<";
				templateDepth++;
			}
			else if (token->is(clang::tok::greater) || token->is(clang::tok::greatergreater))
			{
				// This is invalid C++03 but valid C++11
				if (token->is(clang::tok::greatergreater))
				{
					ret += ">>";
					templateDepth -= 2;
				}
				else
				{
					ret += ">";
					templateDepth--;
				}

				if (templateDepth == 0)
				{
					// We're done
					return ret;
				}
				else if (templateDepth < 0)
				{
					// Closed a template that wasn't open
					// This can happen only with greatergreater
					return std::string();
				}
			}
			else if (token->is(clang::tok::colon))
			{
				ret += ":"; 
			}
			else if (token->is(clang::tok::comma))
			{
				ret += ", ";
			}
			else if (token->is(clang::tok::raw_identifier))
			{
				const std::string identifier(identifierString(*token));

				if ((token - 1)->is(clang::tok::raw_identifier))
				{
					// Space out consecutive identifiers
					ret += " ";
				}

				ret += identifier;
			}
			else if (token->is(clang::tok::star))
			{
				ret += "*";
			}
			else
			{
				// Unknown token
				return std::string();
			}

			token++;
		}


		if (templateDepth > 0)
		{
			// We ran out of tokens early
			return std::string();
		}

		return ret;
	}

	BaseType consumeBaseType(TokenVector::const_iterator &token, TokenVector::const_iterator end, bool allowIdentifier, const TokenKindSet stopTokens)
	{
		// Additionally stop on & and *
		TokenKindSet allStopTokens(stopTokens);
		allStopTokens.insert(clang::tok::star);
		allStopTokens.insert(clang::tok::amp);

		BaseType ret;
		
		// Track our parse state
		enum class ParseState
		{
			Begin,
			Identifier,
			TypeSpecifier,
			ColonColon,
		};
		
		ParseState state = ParseState::Begin;
		bool haveBaseName = false;

		const std::set<std::string> typeSpecifiers({"short", "long", "unsigned", "signed", "class", "struct", "union", "enum"});

		// Keep going until we run out of tokens or hit a stop token
		while(token != end && (allStopTokens.count(token->getKind()) == 0))
		{
			if (token->is(clang::tok::colon) && 
			    ((state == ParseState::Begin) ||
			     (state == ParseState::Identifier))) 
			{
				// The next token must be :
				token++;

				if ((token == end) || token->isNot(clang::tok::colon))
				{
					// Not expected
					return BaseType();
				}

				ret.typeName += "::";
				state = ParseState::ColonColon;
			}
			else if (token->is(clang::tok::less) && (state == ParseState::Identifier) && haveBaseName)
			{
				const std::string templateParam(consumeTemplateParam(token, end));

				if (templateParam.empty())
				{
					return BaseType();
				}

				ret.typeName += templateParam;	

				state = ParseState::Identifier;
			}
			else if (token->is(clang::tok::raw_identifier))
			{
				std::string identifier(identifierString(*token));

				if (identifier == "const")
				{
					ret.isConst = true;
					state = ParseState::Identifier;
				}
				else if (typeSpecifiers.count(identifier) > 0)
				{
					if (state == ParseState::TypeSpecifier)
					{
						ret.typeName += " ";
					}

					ret.typeName += identifier;
					state = ParseState::TypeSpecifier;
				}
				else if (!haveBaseName || (state == ParseState::ColonColon))
				{
					if (state == ParseState::TypeSpecifier)
					{
						ret.typeName += " ";
					}

					ret.typeName += identifier;

					haveBaseName = true;
					state = ParseState::Identifier;
				}
				else if (allowIdentifier)
				{
					break;
				}
				else
				{
					// Not expected
					return BaseType();
				}
			}
			else
			{
				// Unknown token
				return BaseType();
			}

			token++;
		}

		return ret;
	}
}

namespace DeclarationParser
{
	TypeDeclaration consumeType(TokenVector::const_iterator &token, TokenVector::const_iterator end, bool allowIdentifier, const TokenKindSet &stopTokens)
	{
		IndirectionVector indirections;
		BaseType baseType = consumeBaseType(token, end, allowIdentifier, stopTokens);

		if (!baseType.isValid())
		{
			return TypeDeclaration();
		}

		// Keep going until we run out of tokens or hit a stop token
		while(token != end && (stopTokens.count(token->getKind()) == 0))
		{
			if (token->is(clang::tok::raw_identifier))
			{
				const std::string identifier(identifierString(*token));

				if (identifier == "const")
				{
					// Is this applying to a *
					if (!indirections.empty() && (indirections.back() == Indirection::Pointer))
					{
						// Convert to a const *
						indirections.back() = Indirection::ConstPointer;
					}
					else
					{
						// Unexpected
						return TypeDeclaration();
					}
				}
				else if (allowIdentifier && (++token == end))
				{
					// Trailing identifier
					break;
				}
				else
				{
					// Unexpected
					return TypeDeclaration();
				}
			}
			else if (token->is(clang::tok::amp))
			{
				indirections.push_back(Indirection::Reference);
			}
			else if (token->is(clang::tok::star))
			{
				// Assume non-const, following "const" may modify it
				indirections.push_back(Indirection::Pointer);
			}
			else
			{
				// Something we don't know about
				return TypeDeclaration();
			}
			   
			token++;
		}

		return TypeDeclaration(baseType.isConst, baseType.typeName, indirections);
	}
}
