#include "ConnectCall.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
	
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

ConnectCall ConnectCall::fromCallExpr(clang::CallExpr *expr)
{
	ConnectCall call;

	if (!isQObjectConnect(expr))
	{
		return call;
	}

	// Looks like a connect!
	call.mExpr = expr;

	return call;
}
