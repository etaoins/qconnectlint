#include "Tokenizer.h"

#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/MemoryBuffer.h"
#include "clang/Lex/Lexer.h"

namespace Tokenizer
{
	TokenVector tokenizeString(const std::string &string, clang::CompilerInstance &instance)
	{
		// Try to lex it
		llvm::MemoryBuffer *buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(string));
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

		return tokens;
	}
}
