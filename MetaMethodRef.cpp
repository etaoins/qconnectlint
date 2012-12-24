#include "MetaMethodRef.h"

MetaMethodRef::MetaMethodRef(std::string rawString) :
	mRawString(rawString),
	mType(MetaMethodType::Invalid)
{
}
