#ifndef _CONNECTCALLVISITOR_H
#define _CONNECTCALLVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"

#include "Reporter.h"

namespace clang
{
	class SourceManager;
}

class ConnectCall;

class ConnectCallVisitor : public clang::RecursiveASTVisitor<ConnectCallVisitor>
{
public:
	virtual bool VisitCallExpr(clang::CallExpr *expr);
	virtual bool VisitConnectCall(const ConnectCall &) = 0;
};

#endif
