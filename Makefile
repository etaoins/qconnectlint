CLANG_LEVEL := ../..

TOOLNAME = qconnectlint

# No plugins, optimize startup time.
TOOL_NO_EXPORTS = 1

# Run our parser test if requested
ifeq ($(PARSER_TEST), 1)
override CXXFLAGS += -DPARSER_TEST
endif

include $(CLANG_LEVEL)/../../Makefile.config
LINK_COMPONENTS := $(TARGETS_TO_BUILD) asmparser support mc
USEDLIBS = clangTooling.a clangFrontend.a clangSerialization.a clangDriver.a \
					 clangRewriteFrontend.a clangRewriteCore.a \
					 clangParse.a clangSema.a clangAnalysis.a \
					 clangAST.a clangASTMatchers.a clangEdit.a clangLex.a clangBasic.a

include $(CLANG_LEVEL)/Makefile
