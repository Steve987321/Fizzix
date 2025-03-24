#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include <vector>

namespace Compiler
{

    CompileResult CompileString(std::string_view str, std::vector<VM::Instruction>& res, std::function<void()> pre_parse_callback)
	{
        error_msgs.clear();
        
		std::cout << str << std::endl;
		LexerInit(str);

		std::vector<Token> tokens;
		if (!LexerScan(tokens))
			return CompileResult::ERR;

		// to vm bytecodes
		if (!Parse(tokens, res, pre_parse_callback))
            return CompileResult::ERR;
        
		return CompileResult::NONE;
	}

}
