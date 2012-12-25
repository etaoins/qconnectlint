#include "ConnectCallVisitor.h"

#include <iostream>

#include "clang/Basic/SourceManager.h"
#include "clang/Basic/SourceLocation.h"

#include "Reporter.h"
#include "ConnectCall.h"
	
ConnectCallVisitor::ConnectCallVisitor(clang::CompilerInstance &instance) :
	mInstance(instance)
{
}

bool ConnectCallVisitor::VisitCallExpr(clang::CallExpr *expr)
{
	ConnectCall connectCall(ConnectCall::fromCallExpr(expr, mInstance));

	// Not a QObject::connect() call
	if (!connectCall.isNull())
	{
		return true;
	}

	return VisitConnectCall(connectCall);
}
