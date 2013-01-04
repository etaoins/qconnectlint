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
		mReporter.report(call) << "Can't parse raw slot reference" << ("\"" + sendMethod.rawString() + "\"");
	}
	
	if (!receiveMethod.isValid())
	{
		mReporter.report(call) << "Can't parse raw signal or slot reference" << ("\"" + receiveMethod.rawString() + "\"");
	}

	if (!sendMethod.isValid() || !receiveMethod.isValid())
	{
		// Doesn't make sense to continue
		return true;
	}

	if (!referencedMethodExists(call.sender(), sendMethod))
	{
		mReporter.report(call) << "Can't find sender" << sendMethod.spelling() << "on" << call.sender()->getQualifiedNameAsString();
	}

	if (!referencedMethodExists(call.receiver(), receiveMethod))
	{
		mReporter.report(call) << "Can't find receiver" << receiveMethod.spelling() << "on" << call.receiver()->getQualifiedNameAsString();
	}

	return true;
}
	
bool ConnectCallChecker::referencedMethodExists(const clang::CXXRecordDecl *recordDecl, const MetaMethodRef &ref)
{
	// Try all our methods
	for(auto it = recordDecl->method_begin(); it != recordDecl->method_end(); it++)
	{
		if (it->getNameAsString() == ref.signature().methodName())
		{
			const clang::CXXMethodDecl *decl = *it;

			// Parse the declaration
			MetaMethodSignature parsedDecl(decl, compilerInstance());

			if (!parsedDecl.isValid())
			{
				// Dump the source text
				const clang::SourceManager &sm = compilerInstance().getSourceManager();
				const clang::SourceRange range = decl->getSourceRange();

				const std::string sourceText(
					sm.getCharacterData(range.getBegin()),
					sm.getCharacterData(range.getEnd()) + 1);

				mReporter.report(decl) << "Can't parse signal or slot declaration" << ("\"" + sourceText + "\"");
			}

			// Close enough
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
