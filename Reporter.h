#ifndef _REPORTER_H
#define _REPORTER_H

#include <ostream>
#include <string>

namespace clang
{
	class SourceManager;
	class Expr;
}

class Reporter
{
public:
	Reporter(clang::SourceManager &sourceManager);

	std::ostream& report(const clang::Expr *expr);

private:
	clang::SourceManager &mSourceManager;
};

#endif
