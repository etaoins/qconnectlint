#ifndef _TYPEDECLARATION_H
#define _TYPEDECLARATION_H

#include <string>
#include <vector>

enum class Indirection
{
	Pointer,
	ConstPointer,
	Reference
};

typedef std::vector<Indirection> IndirectionVector;

class TypeDeclaration
{
public:
	// Creates an invalid declaration
	TypeDeclaration() : 
		mIsConst(false)
	{
	}

	// Creates a valid declaration
	TypeDeclaration(bool isConst, const std::string &typeName, IndirectionVector indirections) : 
		mIsConst(isConst),
		mTypeName(typeName),
		mIndirections(indirections)
	{
	}

	bool isValid() const
	{
		return !mTypeName.empty();
	}

	bool isConst() const
	{
		return mIsConst;
	}
	
	std::string typeName() const
	{
		return mTypeName;
	}

	// The innermost indirection is the first element
	const IndirectionVector &indirections() const
	{
		return mIndirections;
	}

	bool operator==(const TypeDeclaration &other) const;

	bool operator!=(const TypeDeclaration &other) const
	{
		return !(*this == other);
	}

	std::string spelling() const;

private:
	bool mIsConst;
	std::string mTypeName;
	IndirectionVector mIndirections;
};

#endif
