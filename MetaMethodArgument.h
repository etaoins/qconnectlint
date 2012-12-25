#ifndef _METAMETHODARGUMENT_H
#define _METAMETHODARGUMENT_H

#include <string>

#include "TokenVector.h"

enum class ArgumentParseMode
{
	// Argument names aren't allowed, comments aren't allowed
	ConnectCall,
	// Argument names allowed, comments allowed
	Declaration
};

enum class BasicType
{
	Invalid,
	Value,
	Pointer,
	Reference
};

class MetaMethodSignature;

class MetaMethodArgument
{
	friend class MetaMethodSignature;
public:
	BasicType basicType() const { return mBasicType; }
	std::string typeIdentifier() const { return mTypeIdentifier; }
	bool isConst() const { return mConst; }

	bool isValid() const { return mBasicType != BasicType::Invalid; }

private:
	MetaMethodArgument(TokenVector::const_iterator begin, TokenVector::const_iterator end, ArgumentParseMode mode);

	void parse(TokenVector::const_iterator begin, TokenVector::const_iterator end);

	BasicType mBasicType;
	std::string mTypeIdentifier;
	bool mConst;
};


#endif
