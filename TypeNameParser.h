#ifndef _TYPENAMEPARSER_H
#define _TYPENAMEPARSER_H

#include <string>

#include "TokenVector.h"

namespace TypeNameParser
{
	std::string consumeTypeName(TokenVector::const_iterator &token, TokenVector::const_iterator end);
}

#endif
