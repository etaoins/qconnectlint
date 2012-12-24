#ifndef _REPORTER_H
#define _REPORTER_H

#include <ostream>
#include <string>

namespace clang
{
	class SourceManager;
	class Expr;
}

class ConnectCall;

class Reporter
{
public:
	Reporter(clang::SourceManager &sourceManager);

	std::ostream& report(const clang::Expr *expr);
	std::ostream& report(const ConnectCall &);

private:
	clang::SourceManager &mSourceManager;
};

#endif
