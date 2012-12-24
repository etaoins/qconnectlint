#ifndef _METAMETHODREF_H
#define _METAMETHODREF_H

#include <string>

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

	explicit MetaMethodRef(std::string rawString);

	MetaMethodType type() const { return mType; }
	bool isValid() const { return type() != MetaMethodType::Invalid; }
	std::string rawString() const { return mRawString; }

private:
	std::string mRawString;
	MetaMethodType mType;
};

#endif
