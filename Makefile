CLANG_LEVEL := ../..

TOOLNAME = qconnectlint

# No plugins, optimize startup time.
TOOL_NO_EXPORTS = 1

include $(CLANG_LEVEL)/../../Makefile.config
LINK_COMPONENTS := $(TARGETS_TO_BUILD) asmparser support mc
USEDLIBS = clangTooling.a clangFrontend.a clangSerialization.a clangDriver.a \
					 clangRewriteFrontend.a clangRewriteCore.a \
					 clangParse.a clangSema.a clangAnalysis.a \
					 clangAST.a clangASTMatchers.a clangEdit.a clangLex.a clangBasic.a

include $(CLANG_LEVEL)/Makefile
