#include "Reporter.h"

#include <iostream>

// Assume anything that isn't Windows is vaguely Unix-like
#ifndef _WIN32
#include <unistd.h>
#endif

#include "clang/Basic/SourceManager.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Decl.h"

#include "ConnectCall.h"

namespace
{
	bool shouldUseAnsi()
	{
#ifndef _WIN32
		return isatty(1);
#else
		return false;
#endif
	}
	
	// Build an ANSI escape sequence or an empty string if ANSI is disabled
	const std::string sgrCode(const char *sgrCode)
	{
		if (shouldUseAnsi())
		{
			return std::string("\x1b[") + sgrCode + "m";
		}

		return "";
	}

	const std::string resetCode()
	{
		return sgrCode("0");
	}
	
	const std::string boldCode()
	{
		return sgrCode("1");
	}

	const std::string connectColorCode()
	{
		return sgrCode("34");
	}
	
	const std::string defaultColorCode()
	{
		return sgrCode("39");
	}
}

ReportStream::~ReportStream()
{
	std::cerr << resetCode() << std::endl;
}

Reporter::Reporter(clang::SourceManager &sourceManager) :
	mSourceManager(sourceManager)
{
}

Reporter::~Reporter()
{
	if (mTotalReports == 1)
	{
		std::cerr << "1 connect notice generated" << std::endl;
	}
	else if (mTotalReports > 1)
	{
		std::cerr << mTotalReports << " connect notices generated" << std::endl;
	}
}
	
ReportStream Reporter::report(const clang::SourceRange &range)
{
	// Track this report
	mTotalReports++;

	// Print the expansion location
	const clang::SourceLocation expansionLoc(mSourceManager.getExpansionLoc(range.getBegin()));
	std::cerr << boldCode() << expansionLoc.printToString(mSourceManager) << ": ";

	// Print the "connect" in a different color than warnings or errors
	std::cerr << connectColorCode() << "connect:" << defaultColorCode();

	return ReportStream();
}

ReportStream Reporter::report(const clang::Stmt *stmt)
{
	return report(stmt->getSourceRange());
}

ReportStream Reporter::report(const clang::Decl *decl)
{
	return report(decl->getSourceRange());
}
	
ReportStream Reporter::report(const ConnectCall &call)
{
	return report(call.expr());
}
