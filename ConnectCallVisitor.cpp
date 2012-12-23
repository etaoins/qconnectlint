#include "ConnectCallVisitor.h"

bool ConnectCallVisitor::VisitCallExpr(clang::CallExpr *expr)
{
	return true;
}
