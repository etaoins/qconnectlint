#include "MetaMethodRef.h"
#include "MetaMethodSignature.h"

#include <vector>

#include "llvm/Support/MemoryBuffer.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"

MetaMethodRef::MetaMethodRef(std::string rawString, clang::CompilerInstance &instance) :
	mRawString(rawString),
	mType(MetaMethodType::Invalid)
{
	if (rawString.length() < 1)
	{
		// XXX: How?
		return;
	}

	// Skip the type part for now
	std::string declaration(rawString.substr(1));

	// Try to lex it
	llvm::MemoryBuffer *buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(declaration));
	clang::FileID fileId = instance.getSourceManager().createFileIDForMemBuffer(buffer);

	clang::Lexer lex(fileId, buffer, instance.getSourceManager(), clang::LangOptions());
	lex.SetKeepWhitespaceMode(false);

	// Fetch all of the tokens
	TokenVector tokens;
	while(true)
	{
		clang::Token token;
		lex.LexFromRawLexer(token);

		if (token.getKind() == clang::tok::eof)
		{
			// We're done
			break;
		}

		tokens.push_back(token);
	}

	mSignature = MetaMethodSignature(tokens, ArgumentParseMode::ConnectCall);

	if (!mSignature.isValid())
	{
		// Not a valid signature
		return;
	}

	// Looks valid!
	switch(rawString[0])
	{
	case '1':
		mType = MetaMethodType::Slot;
		break;
	case '2':
		mType = MetaMethodType::Signal;
		break;
	}
}

std::string MetaMethodRef::spelling() const
{
	if (!isValid())
	{
		return "(invalid)";
	}

	return typeSpelling() + " " + signature().spelling();
}
	
std::string MetaMethodRef::typeSpelling() const
{
	switch(type())
	{
	case MetaMethodType::Invalid:
		return "(invalid)";
	case MetaMethodType::Signal:
		return "signal";
	case MetaMethodType::Slot:
		return "slot";
	}
}
