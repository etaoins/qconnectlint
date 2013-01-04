#ifndef _CONNECTCALLVISITOR_H
#define _CONNECTCALLVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"

#include "Reporter.h"

namespace clang
{
	class CompilerInstance;
}

class ConnectCall;

class ConnectCallVisitor : public clang::RecursiveASTVisitor<ConnectCallVisitor>
{
public:
	ConnectCallVisitor(clang::CompilerInstance &);

	virtual bool VisitCallExpr(clang::CallExpr *expr);
	virtual bool VisitConnectCall(const ConnectCall &) = 0;

	clang::CompilerInstance &compilerInstance()
	{
		return mInstance;
	}

private:
	clang::CompilerInstance &mInstance;
};

#endif
