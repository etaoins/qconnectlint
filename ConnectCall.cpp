#include "ConnectCall.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "MetaMethodRef.h"
	
namespace
{
	bool isQObjectConnect(const clang::CallExpr *expr)
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
		
		if (!methodDecl->isStatic() || (expr->getNumArgs() != 5))
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

	const clang::CXXRecordDecl *declForArg(const clang::Expr *rawExpr)
	{
		// connect calls generally have a lot of casting
		const clang::Expr *expr = rawExpr->IgnoreImpCasts();

		// Is this something we trust?
		if (clang::isa<clang::CXXThisExpr>(expr) || clang::isa<clang::DeclRefExpr>(expr))
		{
			return expr->getType().getTypePtr()->getPointeeCXXRecordDecl();
		}

		return nullptr;
	}

	MetaMethodRef metaMethodRefForArg(const clang::Expr *expr, clang::CompilerInstance &instance)
	{
		const auto *callExpr = clang::dyn_cast<clang::CallExpr>(expr);

		if (!callExpr) 
		{
			// XXX: Not a call?
			return MetaMethodRef();
		}
		
		const clang::FunctionDecl *functionDecl = callExpr->getDirectCallee();
		
		if (!functionDecl || (functionDecl->getName() != clang::StringRef("qFlagLocation")))
		{
			// Not a call to qFlagLocation
			return MetaMethodRef();
		}

		if (callExpr->getNumArgs() != 1)
		{
			// I don't get it
			return MetaMethodRef();
		}

		const auto *stringLiteral = clang::dyn_cast<clang::StringLiteral>(
				callExpr->getArg(0)->IgnoreImpCasts());

		// Use Data() so we chop the location data after the \000
		return MetaMethodRef(stringLiteral->getString().data(), instance);
	}
}

ConnectCall ConnectCall::fromCallExpr(const clang::CallExpr *expr, clang::CompilerInstance &instance)
{
	return ConnectCall(expr, instance);
}
	
ConnectCall::ConnectCall(const clang::CallExpr *expr, clang::CompilerInstance &instance) :
	mExpr(nullptr)
{
	if (!isQObjectConnect(expr))
	{
		return;
	}

	mSender = declForArg(expr->getArg(0));
	mReceiver = declForArg(expr->getArg(2));

	if ((mSender == nullptr) || (mReceiver == nullptr))
	{
		return;
	}

	// Looks like a connect!
	mSendMethod = metaMethodRefForArg(expr->getArg(1), instance);
	mReceiveMethod = metaMethodRefForArg(expr->getArg(3), instance);
	
	mExpr = expr;
}
