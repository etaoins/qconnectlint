#ifndef _CONNECTCALLVISITOR_H
#define _CONNECTCALLVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"

#include "Reporter.h"

namespace clang
{
	class SourceManager;
}

class ConnectCallVisitor : public clang::RecursiveASTVisitor<ConnectCallVisitor>
{
public:
	ConnectCallVisitor(clang::SourceManager &sourceManager);
	virtual bool VisitCallExpr(clang::CallExpr *expr);

private:
	Reporter mReporter;
};

#endif
