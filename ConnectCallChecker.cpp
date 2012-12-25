#include "ConnectCallChecker.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "clang/Frontend/CompilerInstance.h"

#include "ConnectCall.h"

ConnectCallChecker::ConnectCallChecker(clang::CompilerInstance &instance) : 
	ConnectCallVisitor(instance),
	mReporter(instance.getSourceManager())
{
}

bool ConnectCallChecker::VisitConnectCall(const ConnectCall &call)
{
	const MetaMethodRef sendMethod(call.sendMethod()); 
	const MetaMethodRef receiveMethod(call.receiveMethod()); 

	if (!sendMethod.isValid())
	{
		mReporter.report(call) << "Can't parse slot reference \"" << sendMethod.rawString() << "\"" << std::endl;
	}
	
	if (!receiveMethod.isValid())
	{
		mReporter.report(call) << "Can't parse signal or slot reference \"" << receiveMethod.rawString() << "\"" << std::endl;
	}

	if (!sendMethod.isValid() || !receiveMethod.isValid())
	{
		// Doesn't make sense to continue
		return true;
	}

	return true;
}
