#ifndef _REPORTER_H
#define _REPORTER_H

#include <iostream>
#include <string>

#include "clang/Basic/SourceLocation.h"

namespace clang
{
	class SourceManager;
	class Stmt;
	class Decl;
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

	ReportStream report(const clang::Stmt *stmt);
	ReportStream report(const clang::Decl *decl);
	ReportStream report(const ConnectCall &);
	
	ReportStream report(const clang::SourceRange &);

	unsigned int totalReports() const 
	{
		return mTotalReports;
	}

private:
	clang::SourceManager &mSourceManager;

	unsigned int mTotalReports = 0;
};

#endif
