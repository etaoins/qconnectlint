#ifndef _METAMETHODSIGNATURE_H
#define _METAMETHODSIGNATURE_H

#include <string>

#include "TokenVector.h"

namespace clang
{
	class CompilerInstance;
	class FunctionDecl;
}

class MetaMethodSignature
{
public:
	// Create an invalid signature
	MetaMethodSignature() : mValid(false)
	{
	}

	///
	/// Parses a connect call meta method reference signature from a string
	///
	MetaMethodSignature(std::string str, clang::CompilerInstance &instance);

	///
	/// Parses a declaration signature from a Clang function decl
	///
	MetaMethodSignature(const clang::FunctionDecl *decl, clang::CompilerInstance &instance);
	
	bool isValid() const { return mValid; }

	std::string methodName() const { return mMethodName; }

	std::string spelling() const;

private:
	bool parseConnectCallRefTokens(TokenVector::const_iterator begin, TokenVector::const_iterator end);

	bool mValid;
	std::string mMethodName;
};

#endif
