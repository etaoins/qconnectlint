#include "ConnectCallChecker.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "clang/Frontend/CompilerInstance.h"

#include "ConnectCall.h"

namespace
{
	bool referencedMethodExists(const clang::CXXRecordDecl *recordDecl, const MetaMethodRef &ref)
	{
		// Try all our methods
		for(auto it = recordDecl->method_begin(); it != recordDecl->method_end(); it++)
		{
			if (it->getNameAsString() == ref.signature().methodName())
			{
				return true;
			}
		}

		// Try our superclasses recursively
		for(auto it = recordDecl->bases_begin(); it != recordDecl->bases_end(); it++)
		{
			clang::CXXRecordDecl *baseDecl = it->getType()->getAsCXXRecordDecl();

			if (baseDecl && referencedMethodExists(baseDecl, ref))
			{
				return true;
			}
		}

		return false;
	}
}

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
		mReporter.report(call) << "Can't parse raw slot reference \"" << sendMethod.rawString() << "\"" << std::endl;
	}
	
	if (!receiveMethod.isValid())
	{
		mReporter.report(call) << "Can't parse raw signal or slot reference \"" << receiveMethod.rawString() << "\"" << std::endl;
	}

	if (!sendMethod.isValid() || !receiveMethod.isValid())
	{
		// Doesn't make sense to continue
		return true;
	}

	if (!referencedMethodExists(call.sender(), sendMethod))
	{
		mReporter.report(call) << "Can't find sender " << sendMethod.spelling() << " on " << call.sender()->getQualifiedNameAsString() << std::endl;
	}

	if (!referencedMethodExists(call.receiver(), receiveMethod))
	{
		mReporter.report(call) << "Can't find receiver " << receiveMethod.spelling() << " on " << call.receiver()->getQualifiedNameAsString() << std::endl;
	}

	return true;
}
