#include "MetaMethodRef.h"
#include "MetaMethodSignature.h"

#include <vector>

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/CompilerInstance.h"

MetaMethodRef::MetaMethodRef(std::string rawString, clang::CompilerInstance &instance) :
	mRawString(rawString),
	mType(MetaMethodType::Invalid)
{
	if (rawString.length() < 1)
	{
		// XXX: How?
		return;
	}

	// Skip the type part for now
	std::string declaration(rawString.substr(1));

	mSignature = MetaMethodSignature(declaration, instance);

	if (!mSignature.isValid())
	{
		// Not a valid signature
		return;
	}

	// Looks valid!
	switch(rawString[0])
	{
	case '1':
		mType = MetaMethodType::Slot;
		break;
	case '2':
		mType = MetaMethodType::Signal;
		break;
	}
}

std::string MetaMethodRef::spelling() const
{
	if (!isValid())
	{
		return "(invalid)";
	}

	return typeSpelling() + " " + signature().spelling();
}
	
std::string MetaMethodRef::typeSpelling() const
{
	switch(type())
	{
	case MetaMethodType::Invalid:
		return "(invalid)";
	case MetaMethodType::Signal:
		return "signal";
	case MetaMethodType::Slot:
		return "slot";
	}
}
