#ifndef _METAMETHODSIGNATURE_H
#define _METAMETHODSIGNATURE_H

#include <string>

#include "MetaMethodArgument.h"
#include "TokenVector.h"

class MetaMethodSignature
{
public:
	// Create an invalid signature
	MetaMethodSignature() : mValid(false)
	{
	}

	MetaMethodSignature(const TokenVector &tokens, ArgumentParseMode);
	
	bool isValid() const { return mValid; }

	std::string methodName() const { return mMethodName; }
	std::vector<MetaMethodArgument> arguments() const { return mArguments; }

	std::string spelling() const;

private:
	bool mValid;
	std::string mMethodName;
	std::vector<MetaMethodArgument> mArguments;
};

#endif
