#ifndef _CONNECTCALL_H
#define _CONNECTCALL_H

#include "MetaMethodRef.h"

namespace clang
{
	class CallExpr;
	class CXXRecordDecl;
	class CompilerInstance;
}

class ConnectCall
{
public:
	// Returns a null ConnectCall
	ConnectCall() : mExpr(nullptr)
	{
	}

	static ConnectCall fromCallExpr(const clang::CallExpr *, clang::CompilerInstance &);

	bool isNull() const { return mExpr != nullptr; }

	// Return the expression for this call
	const clang::CallExpr *expr() const { return mExpr; } 

	const clang::CXXRecordDecl *sender() const { return mSender; }
	MetaMethodRef sendMethod() const { return mSendMethod; }

	const clang::CXXRecordDecl *receiver() const { return mReceiver; }
	MetaMethodRef receiveMethod() const { return mReceiveMethod; }

private:
	ConnectCall(const clang::CallExpr *, clang::CompilerInstance &);

	const clang::CallExpr *mExpr;

	const clang::CXXRecordDecl *mSender;
	MetaMethodRef mSendMethod;

	const clang::CXXRecordDecl *mReceiver;
	MetaMethodRef mReceiveMethod;
};

#endif
