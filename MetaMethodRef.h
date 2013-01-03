#ifndef _METAMETHODREF_H
#define _METAMETHODREF_H

#include <string>

#include "MetaMethodSignature.h"

namespace clang
{
	class CompilerInstance;
}

enum class MetaMethodType
{
	Signal,
	Slot,
	Invalid
};

class MetaMethodRef
{
public:
	MetaMethodRef() : mType(MetaMethodType::Invalid)
	{
	}

	MetaMethodRef(std::string rawString, clang::CompilerInstance &);

	bool isValid() const { return type() != MetaMethodType::Invalid; }
	
	std::string rawString() const { return mRawString; }
	// User friendly, parsed version of rawString. This won't return anything useful for an invalid MetaMethodRef
	std::string spelling() const;

	MetaMethodSignature signature() const { return mSignature; }

	MetaMethodType type() const { return mType; }
	std::string typeSpelling() const;

private:
	std::string mRawString;
	MetaMethodType mType;
	MetaMethodSignature mSignature;
};

#endif
