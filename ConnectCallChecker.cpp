#include "ConnectCallChecker.h"

ConnectCallChecker::ConnectCallChecker(clang::SourceManager &sourceManager) : 
	mReporter(sourceManager)
{
}

bool ConnectCallChecker::VisitConnectCall(const ConnectCall &call)
{
	mReporter.report(call) << "Found QObject::connect" << std::endl;
	return true;
}
