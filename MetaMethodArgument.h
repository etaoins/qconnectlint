#ifndef _METHODMETHODARGUMENT_H
#define _METHODMETHODARGUMENT_H

#include "TypeDeclaration.h"

class MetaMethodArgument
{
public:
	MetaMethodArgument(const TypeDeclaration &type, bool hasDefault) :
		mType(type),
		mHasDefault(hasDefault)
	{
	}

	const TypeDeclaration &type() const { return mType; }
	bool hasDefault() const { return mHasDefault; }

private:
	TypeDeclaration mType;
	bool mHasDefault;
};

#endif
