#ifndef _CONNECTCALLVISITOR_H
#define _CONNECTCALLVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"

class ConnectCallVisitor : public clang::RecursiveASTVisitor<ConnectCallVisitor>
{
public:
	virtual bool VisitCallExpr(clang::CallExpr *expr);
};

#endif
