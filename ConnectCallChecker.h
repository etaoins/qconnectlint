#ifndef _CONNECTCALLCHECKER_H
#define _CONNECTCALLCHECKER_H

#include "ConnectCallVisitor.h"

class ConnectCallChecker : public ConnectCallVisitor
{
public:
	ConnectCallChecker(clang::SourceManager &sourceManager);

	bool VisitConnectCall(const ConnectCall &) override;

private:
	Reporter mReporter;
};

#endif
