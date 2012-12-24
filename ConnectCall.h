#ifndef _CONNECTCALL_H
#define _CONNECTCALL_H

namespace clang
{
	class CallExpr;
}

class ConnectCall
{
public:
	// Returns a null ConnectCall
	ConnectCall() : mExpr(nullptr)
	{
	}

	static ConnectCall fromCallExpr(clang::CallExpr *);

	bool isNull() const { return mExpr != nullptr; }

	// Return the expression for this call
	const clang::CallExpr *expr() const { return mExpr; } 

private:
	const clang::CallExpr *mExpr;
};

#endif
