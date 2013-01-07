#include "ConnectCallChecker.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

#include "clang/Frontend/CompilerInstance.h"

#include "ConnectCall.h"

namespace 
{
	TypeDeclaration connectNormalizedType(const TypeDeclaration &decl)
	{
		// This converts "const Foo &" in to just "Foo"
		if (decl.isConst() && (decl.indirections().size() == 1) &&
		    decl.indirections().front() == Indirection::Reference)
		{
			return TypeDeclaration(false, decl.typeName(), std::vector<Indirection>()); 
		}

		return decl;
	}

	bool argumentsMatch(const MetaMethodSignature &connectRef, const MetaMethodSignature &methodDecl)
	{
		auto methodDeclArgIt = methodDecl.arguments().begin();

		for(const MetaMethodArgument &connectRefArg : connectRef.arguments())
		{
			if (methodDeclArgIt == methodDecl.arguments().end())
			{
				// We have more args than the method decl
				return false;
			}

			if (connectNormalizedType(connectRefArg.type()) != connectNormalizedType(methodDeclArgIt->type()))
			{
				// Incompatible arguments
				return false;
			}

			methodDeclArgIt++;
		}

		// Any remaining args on the method decl must have defaults
		for(; methodDeclArgIt != methodDecl.arguments().end(); methodDeclArgIt++)
		{
			if (!methodDeclArgIt->hasDefault())
			{
				return false;
			}
		}

		return true;
	}

	bool signaturesConnectable(const MetaMethodSignature &sender, const MetaMethodSignature &receiver)
	{
		auto senderArgIt = sender.arguments().begin();

		for(const MetaMethodArgument &receiverArg : receiver.arguments())
		{
			if (senderArgIt == sender.arguments().end())
			{
				// We're trying to receive more args than are being sent
				return false;
			}

			if (connectNormalizedType(senderArgIt->type()) != connectNormalizedType(receiverArg.type()))
			{
				return false;
			}

			senderArgIt++;
		}

		// Looks good
		return true;
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

	if (sendMethod.type() != MetaMethodType::Signal)
	{
		mReporter.report(call) << "Sender" << sendMethod.spelling() << "is not a signal";
	}

	if (!signaturesConnectable(sendMethod.signature(), receiveMethod.signature()))
	{
		mReporter.report(call) << "Sender" << sendMethod.spelling() << "not compatible with receiver" << receiveMethod.spelling();
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
			return argumentsMatch(ref.signature(), parsedDecl);
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
