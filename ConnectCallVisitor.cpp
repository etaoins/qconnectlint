#include "ConnectCallVisitor.h"

#include <iostream>

#include "clang/Basic/SourceManager.h"
#include "clang/Basic/SourceLocation.h"

#include "Reporter.h"

namespace
{
	bool isQObjectConnect(clang::CallExpr *expr)
	{
		// These are needed a lot so keep them around
		static const clang::StringRef expectedMethodName("connect");
		static const clang::StringRef expectedClassName("QObject");

		const clang::Decl *calleeDecl = expr->getCalleeDecl();

		// Assume this is a method
		const auto *methodDecl = clang::dyn_cast_or_null<clang::CXXMethodDecl>(calleeDecl);

		if (!methodDecl)
		{
			// Nope
			return false;
		}
		
		if (!methodDecl->isStatic())
		{
			// XXX: We only handle the static version for now
			return false;
		}

		// Check the name of the class and methond
		if (methodDecl->getName() != expectedMethodName)
		{
			return false;
		}
		
		if (methodDecl->getParent()->getName() != expectedClassName)
		{
			return false;
		}

		return true;
	}
}

ConnectCallVisitor::ConnectCallVisitor(clang::SourceManager &sourceManager) : 
	mReporter(sourceManager)
{
}

bool ConnectCallVisitor::VisitCallExpr(clang::CallExpr *expr)
{
	if (!isQObjectConnect(expr))
	{
		return true;
	}

	mReporter.report(expr) << "Found QObject::connect" << std::endl;

	return true;
}
