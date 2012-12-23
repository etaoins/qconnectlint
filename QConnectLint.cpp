#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/FrontendActions.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include "llvm/Support/CommandLine.h"
#include "ConnectCallVisitor.h"

using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class QConnectLintConsumer : public clang::ASTConsumer
{
public:
	virtual void HandleTranslationUnit(clang::ASTContext &context) override
	{
		mVisitor.TraverseDecl(context.getTranslationUnitDecl());
	}

private:
	ConnectCallVisitor mVisitor;
};

class QConnectLintAction : public clang::ASTFrontendAction
{
	virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &, clang::StringRef inFile) override
	{
		return new QConnectLintConsumer;
	}
};

int main(int argc, const char **argv)
{
	// Parse our arguments
	clang::tooling::CommonOptionsParser optionsParser(argc, argv);

	// Create our tool
	clang::tooling::ClangTool tool(optionsParser.GetCompilations(), optionsParser.GetSourcePathList());

	return tool.run(clang::tooling::newFrontendActionFactory<QConnectLintAction>());
}
