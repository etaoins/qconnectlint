#include "ConnectCallChecker.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "ConnectCall.h"

ConnectCallChecker::ConnectCallChecker(clang::SourceManager &sourceManager) : 
	mReporter(sourceManager)
{
}

bool ConnectCallChecker::VisitConnectCall(const ConnectCall &call)
{
	mReporter.report(call) << "Found QObject::connect "
		<< "from " << call.sender()->getNameAsString() << " "
		<< "to " << call.receiver()->getNameAsString() << std::endl;

	return true;
}
