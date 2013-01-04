#ifndef _CONNECTCALLCHECKER_H
#define _CONNECTCALLCHECKER_H

#include <string>
#include "ConnectCallVisitor.h"

namespace clang
{
	class CompilerInstance;
	class CXXRecordDecl;
	class Decl;
}

class MetaMethodRef;
class MetaMethodSignature;

class ConnectCallChecker : public ConnectCallVisitor
{
public:
	ConnectCallChecker(clang::CompilerInstance &instance);

	bool VisitConnectCall(const ConnectCall &) override;

private:
	std::string sourceForDecl(const clang::Decl *decl);
	bool referencedMethodExists(const clang::CXXRecordDecl *recordDecl, const MetaMethodRef &ref);

	Reporter mReporter;
};

#endif
