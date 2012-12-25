#ifndef _TOKENVECTOR_H
#define _TOKENVECTOR_H

#include <vector>

namespace clang
{
	class Token;
}

typedef std::vector<clang::Token> TokenVector;

#endif
