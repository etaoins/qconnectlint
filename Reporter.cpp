#include "Reporter.h"

#include <iostream>

#include "clang/AST/Expr.h"
#include "clang/Basic/SourceLocation.h"

Reporter::Reporter(clang::SourceManager &sourceManager) :
	mSourceManager(sourceManager)
{
}

std::ostream& Reporter::report(const clang::Expr *expr)
{
	return std::cout << expr->getLocStart().printToString(mSourceManager) << ": ";
}
