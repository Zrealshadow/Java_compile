#pragma once
#include"token.h"

#ifndef LEXER_BUFFER_SIZE
#define LEXER_BUFFER_SIZE 1024
#endif
class lexer
{
public:
	lexer(string f);
	lexer();
	~lexer();

	char buffer[LEXER_BUFFER_SIZE];

	int java_scan();
	void java_lexer_reset();
	void scan();
	void set_filename(string f);

private:
	string error;
	int stash;
	int lineno;
	off_t offset;
	// �ļ�ĩβ�� \0��β �����ж� ������char *
	char* source;
	java_token_t tok;
	string filename;
	int scan_string(int q);
	int scan_identity(int c);
	int scan_number(int c);
	int hex_literal();//����16�����ַ�
	int oct_literal();//����8�����ַ�
	void java_token_to_xml(const java_token_t &tok);
	unsigned int EnumTohex(int JAVA_TOKEN);

};

