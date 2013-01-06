#include <string>
#include <iostream>

#include "TypeDeclaration.h"
#include "DeclarationParser.h"
#include "Tokenizer.h"

namespace
{
	clang::CompilerInstance *Instance = nullptr;

	const std::string sgrCode(const char *sgrCode)
	{
		return std::string("\x1b[") + sgrCode + "m";
	}

	const std::string failColorCode()
	{
		// Bold + red
		return sgrCode("31") + sgrCode("1");
	}
	
	const std::string resetCode()
	{
		return sgrCode("0");
	}
	
	void showPass()
	{
		std::cout << ".";
	}
	
	void showFail(const std::string &message)
	{
		std::cout << std::endl;
		std::cout << "[" << failColorCode() << "FAIL" << resetCode() << "] ";
		std::cout << message << std::endl;
	}

	void finalizeOutput()
	{
		std::cout << std::endl;
	}

	TypeDeclaration parseTypeString(const std::string &type, bool allowIdentifier, bool expectFail)
	{
		TokenVector tokens = Tokenizer::tokenizeString(type, *Instance); 
		TokenVector::const_iterator token = tokens.begin();

		TypeDeclaration declaration = DeclarationParser::consumeType(token, tokens.end(), allowIdentifier);

		if ((token != tokens.end()) && declaration.isValid() && !expectFail)
		{
			showFail("Unconsumed tokens after parsing '" + type + "'");
		}

		return declaration;
	}

	void validDecl(const std::string &type, bool allowIdentifier, const std::string &declKind)
	{
		if (!parseTypeString(type, allowIdentifier, false).isValid())
		{
			showFail("Invalid " + declKind + ": '" + type + "'");
		}
		else
		{
			showPass();
		}
	}
	
	void invalidDecl(const std::string &type, bool allowIdentifier, const std::string &declKind)
	{
		TypeDeclaration parsedType = parseTypeString(type, allowIdentifier, true);
		if (parsedType.isValid())
		{
			showFail("Invalid " + declKind + " string parsed as valid: '" + type + "' (parsed as '" + parsedType.spelling() + "')");
		}
		else
		{
			showPass();
		}
	}

	void validType(const std::string &type)
	{
		validDecl(type, false, "type");
	}
	
	void invalidType(const std::string &type)
	{
		invalidDecl(type, false, "type");
	}

	void validParamDecl(const std::string &decl)
	{
		validDecl(decl, true, "param decl");
	}
	
	void invalidParamDecl(const std::string &decl)
	{
		invalidDecl(decl, true, "param decl");
	}

	void compareTypes(const std::string &left, const std::string &right, bool shouldMatch)
	{
		TypeDeclaration leftType = parseTypeString(left, false, false);
		TypeDeclaration rightType = parseTypeString(right, false, false);

		if (!leftType.isValid())
		{
			showFail("Invalid type: '" + left + "'");
			return;
		}
		
		if (!rightType.isValid())
		{
			showFail("Invalid type: '" + right + "'");
			return;
		}

		if ((leftType == rightType) != shouldMatch)
		{
			std::string message;

			message  = "Type string '" + left + "' (parsed as '" + leftType.spelling() + "') ";

			if (shouldMatch)
			{
				message += "does not match ";
			}
			else
			{
				message += "matches ";
			}

			message += "type string '" + right + "' (parsed as '" + rightType.spelling() + "')";

			showFail(message);
			return;
		}

		showPass();
	}
	
	void sameType(const std::string &left, const std::string &right)
	{
		compareTypes(left, right, true);
	}
	
	void differentTypes(const std::string &left, const std::string &right)
	{
		compareTypes(left, right, false);
	}
}
