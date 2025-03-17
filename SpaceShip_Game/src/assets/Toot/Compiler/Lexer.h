#pragma once

#include <string>
#include <vector>

namespace Compiler
{

	enum class TOKEN_TYPE
	{
		ERROR,
		STRING_LITERAL,
		NUMBER,
		COMMA,
		INT,
		FLOAT,
		IF,
		ELSE,
		FOR,
		TRUE,
		FALSE,
		SEMICOLON,
		BRACKET_LEFT,
		BRACKET_RIGHT,
		MINUS,
		MINUS_AND_ASSIGN,
		PLUS,
		PLUS_AND_ASSIGN,
		NOT,
		NOT_EQUAL,
		PARENTHESES_LEFT,
		PARENTHESES_RIGHT,
		MULTIPLY,
		MULTIPLY_AND_ASSIGN, 
		DIVIDE,
		DIVIDE_AND_ASSIGN,
		ASSIGNMENT,
		COMPARISON,
		IDENTIFIER,
		FORMAT_STRING,
	};

	struct Token 
	{
		TOKEN_TYPE type;
		std::string str;
		uint32_t line;
	};

	bool TokenIsDigit(const Token& t);

	void LexerInit(std::string_view src);

	bool LexerScan(std::vector<Token>& tokens);
}