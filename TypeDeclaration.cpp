#include "TypeDeclaration.h"

bool TypeDeclaration::operator==(const TypeDeclaration &other) const
{
	if (isConst() != other.isConst())
	{
		return false;
	}

	if (typeName() != other.typeName())
	{
		return false;
	}

	if (indirections().size() != other.indirections().size())
	{
		return false;
	}

	auto otherIt = other.indirections().begin();

	for(Indirection i : indirections())
	{
		if (i != *(otherIt++))
		{
			return false;
		}
	}

	return true;
}
	
std::string TypeDeclaration::spelling() const
{
	if (!isValid())
	{
		return "(invalid)";
	}

	// Start with the type name
	std::string ret(typeName());

	// If we're const prepend "const"
	// We could append here but it looks weird with pointers
	if (isConst())
	{
		ret = "const " + ret;
	}

	for(Indirection indirection : indirections())
	{
		switch(indirection)
		{
		case Indirection::Pointer:
			ret += " *";
			break;
		case Indirection::ConstPointer:
			ret += " const*";
			break;
		case Indirection::Reference:
			ret += " &";
			break;
		}
	}

	return ret;
}
