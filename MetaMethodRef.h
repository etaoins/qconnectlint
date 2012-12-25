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

	MetaMethodType type() const { return mType; }
	bool isValid() const { return type() != MetaMethodType::Invalid; }
	std::string rawString() const { return mRawString; }

	MetaMethodSignature signature() const { return mSignature; }

private:
	std::string mRawString;
	MetaMethodType mType;
	MetaMethodSignature mSignature;
};

#endif
