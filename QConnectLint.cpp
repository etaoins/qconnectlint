#include <iostream>

#include "clang/Basic/SourceManager.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include "llvm/Support/CommandLine.h"
#include "ConnectCallChecker.h"

#ifdef PARSER_TEST
#include "test/ParserTest.h"
#endif

using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class QConnectLintConsumer : public clang::ASTConsumer
{
public:
	QConnectLintConsumer(clang::CompilerInstance &instance) :
		mChecker(instance)
	{
	}

	virtual void HandleTranslationUnit(clang::ASTContext &context) override
	{
		mChecker.TraverseDecl(context.getTranslationUnitDecl());
	}

private:
	ConnectCallChecker mChecker;
};

class QConnectLintAction : public clang::ASTFrontendAction
{
	virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &instance, clang::StringRef inFile) override
	{
#ifdef PARSER_TEST
		ParserTest::run(instance);
#endif
		return new QConnectLintConsumer(instance);
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
