#ifndef _CONNECTCALLCHECKER_H
#define _CONNECTCALLCHECKER_H

#include "ConnectCallVisitor.h"

namespace clang
{
	class CompilerInstance;
}

class ConnectCallChecker : public ConnectCallVisitor
{
public:
	ConnectCallChecker(clang::CompilerInstance &instance);

	bool VisitConnectCall(const ConnectCall &) override;

private:
	Reporter mReporter;
};

#endif
