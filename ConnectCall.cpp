#include "ConnectCall.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "MetaMethodRef.h"
	
namespace
{
	enum class ArgParseResult
	{
		// We don't have enough info to continue linting
		InsufficentInfo,
		// This argument looks suspicious
		SuspiciousArg,
		// It worked
		Success
	};

	enum class ConnectCallType
	{
		Unrecognized,
		StaticFiveArg,
		MemberFourArg
	};

	ConnectCallType exprConnectCallType(const clang::CallExpr *expr)
	{
		// These are needed a lot so keep them around
		static const clang::StringRef expectedMethodName("connect");
		static const clang::StringRef expectedClassName("QObject");
		const unsigned int argCount = expr->getNumArgs();

		const clang::Decl *calleeDecl = expr->getCalleeDecl();

		// Assume this is a method
		const auto *methodDecl = clang::dyn_cast_or_null<clang::CXXMethodDecl>(calleeDecl);

		if (!methodDecl)
		{
			// Nope
			return ConnectCallType::Unrecognized;
		}
		
		ConnectCallType callType;

		// Figure out what kind of connect call it is
		if (methodDecl->isStatic() && (argCount == 5))
		{
			callType = ConnectCallType::StaticFiveArg;
		}
		else if (!methodDecl->isStatic() && (argCount == 4))
		{
			callType = ConnectCallType::MemberFourArg;
		}
		else
		{
			return ConnectCallType::Unrecognized;
		}
		
		// Check the name of the class and methond
		if (methodDecl->getName() != expectedMethodName)
		{
			return ConnectCallType::Unrecognized;
		}
		
		if (methodDecl->getParent()->getName() != expectedClassName)
		{
			return ConnectCallType::Unrecognized;
		}
		
		return callType;
	}

	const clang::CXXRecordDecl *declForArg(const clang::Expr *rawExpr)
	{
		// connect calls generally have a lot of casting
		const clang::Expr *expr = rawExpr->IgnoreImpCasts();
		return expr->getType().getTypePtr()->getPointeeCXXRecordDecl();
	}

	MetaMethodRef metaMethodRefForArg(const clang::Expr *expr, clang::CompilerInstance &instance, ArgParseResult &parseResult)
	{
		const auto *callExpr = clang::dyn_cast<clang::CallExpr>(expr);

		if (!callExpr) 
		{
			// Not a call - it won't be easy to get a static string value for it
			parseResult = ArgParseResult::InsufficentInfo;
			return MetaMethodRef();
		}
		
		const clang::FunctionDecl *functionDecl = callExpr->getDirectCallee();
		
		if (!functionDecl || (functionDecl->getName() != clang::StringRef("qFlagLocation")))
		{
			// Not a call to qFlagLocation
			parseResult = ArgParseResult::InsufficentInfo;
			return MetaMethodRef();
		}

		if (callExpr->getNumArgs() != 1)
		{
			// I don't get it
			parseResult = ArgParseResult::SuspiciousArg;
			return MetaMethodRef();
		}

		const auto *stringLiteral = clang::dyn_cast<clang::StringLiteral>(
				callExpr->getArg(0)->IgnoreImpCasts());

		// Use Data() so we chop the location data after the \000
		parseResult = ArgParseResult::Success;
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
	ConnectCallType callType = exprConnectCallType(expr);
	unsigned int sendMethodArgIndex = 0;
	unsigned int receiveMethodArgIndex = 0;

	if (callType == ConnectCallType::Unrecognized)
	{
		return;
	}
	else if (callType == ConnectCallType::StaticFiveArg)
	{
		mSender = declForArg(expr->getArg(0));
		sendMethodArgIndex = 1;
		mReceiver = declForArg(expr->getArg(2));
		receiveMethodArgIndex = 3;

		if ((mSender == nullptr) || (mReceiver == nullptr))
		{
			return;
		}

	}
	else if (callType == ConnectCallType::MemberFourArg)
	{
		const clang::CXXMemberCallExpr *memberCallExpr = clang::dyn_cast<clang::CXXMemberCallExpr>(expr);

		if (memberCallExpr == nullptr)
		{
			return;
		}

		mReceiver = declForArg(memberCallExpr->getImplicitObjectArgument());
		mSender = declForArg(memberCallExpr->getArg(0));
		sendMethodArgIndex = 1;
		receiveMethodArgIndex = 2;
	}
		
	// Looks like a connect! Parse the meta method references
	ArgParseResult parseResult;
	mSendMethod = metaMethodRefForArg(expr->getArg(sendMethodArgIndex), instance, parseResult);

	if (parseResult == ArgParseResult::InsufficentInfo)
	{
		return;
	}

	mReceiveMethod = metaMethodRefForArg(expr->getArg(receiveMethodArgIndex), instance, parseResult);
	
	if (parseResult == ArgParseResult::InsufficentInfo)
	{
		return;
	}
	
	mExpr = expr;
}
