#ifndef _REPORTER_H
#define _REPORTER_H

#include <iostream>
#include <string>

namespace clang
{
	class SourceManager;
	class Expr;
}

class ConnectCall;

class ReportStream
{
public:
	~ReportStream();

	template <class T>
	ReportStream &operator<<(const T &x)
	{
		std::cerr << " " << x;
		return *this;
	}
};

class Reporter
{
public:
	Reporter(clang::SourceManager &sourceManager);
	~Reporter();

	ReportStream report(const clang::Expr *expr);
	ReportStream report(const ConnectCall &);

	unsigned int totalReports() const 
	{
		return mTotalReports;
	}

private:
	clang::SourceManager &mSourceManager;

	unsigned int mTotalReports = 0;
};

#endif
