#ifndef _CONNECTCALL_H
#define _CONNECTCALL_H

namespace clang
{
	class CallExpr;
	class CXXRecordDecl;
}

class ConnectCall
{
public:
	// Returns a null ConnectCall
	ConnectCall() : mExpr(nullptr)
	{
	}

	static ConnectCall fromCallExpr(const clang::CallExpr *);

	bool isNull() const { return mExpr != nullptr; }

	// Return the expression for this call
	const clang::CallExpr *expr() const { return mExpr; } 

	const clang::CXXRecordDecl *sender() const { return mSender; }
	const clang::CXXRecordDecl *receiver() const { return mReceiver; }

private:
	ConnectCall(const clang::CallExpr *);

	const clang::CallExpr *mExpr;

	const clang::CXXRecordDecl *mSender;
	const clang::CXXRecordDecl *mReceiver;
};

#endif
