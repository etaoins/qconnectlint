#include "ConnectCallVisitor.h"

#include <iostream>

#include "clang/Basic/SourceManager.h"
#include "clang/Basic/SourceLocation.h"

#include "Reporter.h"
#include "ConnectCall.h"

ConnectCallVisitor::ConnectCallVisitor(clang::SourceManager &sourceManager) : 
	mReporter(sourceManager)
{
}

bool ConnectCallVisitor::VisitCallExpr(clang::CallExpr *expr)
{
	ConnectCall connectCall(ConnectCall::fromCallExpr(expr));

	// Not a QObject::connect() call
	if (!connectCall.isNull())
	{
		return true;
	}

	mReporter.report(expr) << "Found QObject::connect" << std::endl;

	return true;
}
