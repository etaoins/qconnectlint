#include "TestUtil.h"

namespace ParserTest
{
	void run(clang::CompilerInstance &instance)
	{
		Instance = &instance;

		// Trivial cases
		validType("BasicType");
		validType("BasicType &");
		validType("BasicType&");
		validType("BasicType *");
		validType("BasicType*");
		
		differentTypes("BasicType", "BasicType *");
		differentTypes("BasicType", "BasicType &");

		invalidType("BasicType OtherIdentifier");
		invalidType("const* int");
		invalidType("BasicType & const&");

		validType("class BasicType");
		validType("struct BasicType");
		validType("enum BasicType");
		validType("enum class BasicType");

		// Namespaces
		validType("::Namespace::BasicType");

		differentTypes("Namespace::BasicType", "BasicType");
		differentTypes("Namespace::BasicType", "::Namespace::BasicType");

		invalidType("Namespace::::BasicType");

		// Const-ness
		differentTypes("BasicType", "const BasicType");

		// This is allowed
		validType("const const BasicType");

		sameType("const BasicType", "BasicType const");
		sameType("const BasicType &", "BasicType const&");
		sameType("const BasicType *", "BasicType const*");
		
		// Integer weirdness
		validType("long int");
		validType("short int");
		validType("unsigned long int");
		validType("signed long long");
		validType("int long unsigned");

		// Pointers to pointers
		validType("char**");
		// Const pointers to const
		validType("const char *const");
		
		differentTypes("const char *const", "const char *");

		// Templates
		validType("std::list<std::string>");
		validType("std::pair<unsigned int, std::string>");
		validType("std::list<std::pair<BasicType, std::string>>");
		validType("QList<const char *>");

		// Param decls
		validParamDecl("BasicType");
		validParamDecl("BasicType name");

		validParamDecl("const BasicType &");
		validParamDecl("const BasicType &name");
		validParamDecl("BasicType const& name");

		invalidParamDecl("BasicType name otherName");

		validParamDecl("long int i");

		finalizeOutput();
	}
}
