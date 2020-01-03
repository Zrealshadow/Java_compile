#include "pch.h"
#include "lexer.h"
#include "stdio.h"
#include <cstring>
#include <fstream>
#include <streambuf>
#define next (stash=source[offset++])

#define undo (--offset)

#define token(t) (tok.token_type=t)

#define error(message) (error=message, token(JAVA_TOKEN_ILLEGAL))

#define clear_token(tok) (buf.clear(), token(tok))


lexer::lexer(string f) :filename(f)
{
	error.clear();
	lineno = 1;
	offset = 0;
	tok.value.as_string.clear();
	tok.value.as_real = 0;
	tok.value.as_int = 0;
	tok.value.as_float = 0;
	tok.value.as_long = 0;
};
lexer::lexer() {
	error.clear();
	lineno = 1;
	offset = 0;
	filename = "";
	tok.value.as_string.clear();
	tok.value.as_real = 0;
	tok.value.as_int = 0;
	tok.value.as_float = 0;
	tok.value.as_long = 0;
}

lexer::~lexer()
{
}

void lexer::java_lexer_reset() {
	//error.clear();
	//lineno = 1;
	//offset = 0;
	tok.value.as_string.clear();
	tok.value.as_int = 0;
	tok.value.as_real = 0;
	tok.value.as_float = 0;
	tok.value.as_long = 0;
}


//程序中心法 扫描昂
int lexer::java_scan() {
	//char c;
	int c;
scan:
	switch (c = next) {
	case '\n':
		lineno += 1;
	case '\t':
	case ' ':
		goto scan;
	case '(':
		return token(JAVA_TOKEN_LPAREN);
	case ')':
		return token(JAVA_TOKEN_RPAREN);
	case '{':
		return token(JAVA_TOKEN_LBRACE);
	case '}':
		return token(JAVA_TOKEN_RBRACE);
	case '[':
		return token(JAVA_TOKEN_LBRACK);
	case ']':
		return token(JAVA_TOKEN_RBRACK);
	case ',':
		return token(JAVA_TOKEN_COMMA);
	case '.':
		return token(JAVA_TOKEN_OP_DOT);
	case '%':
		return token(JAVA_TOKEN_OP_MOD);
	case '^':
		return next == '=' ? token(JAVA_TOKEN_OP_BIT_NOT_ASSIGN) : \
			(undo, token(JAVA_TOKEN_OP_NOT));
	case '~':
		return token(JAVA_TOKEN_OP_BIT_NOT);
	case '?':
		return ':' == next ? token(JAVA_TOKEN_OP_COND) : \
			(undo, token(JAVA_TOKEN_OP_COND));
	case ':':
		return token(JAVA_TOKEN_COLON);
	case ';':
		return token(JAVA_TOKEN_SEMICOLON);
	case '+':
		switch (c = next) {
		case '+':
			return token(JAVA_TOKEN_OP_INCR);
		case '=':
			return token(JAVA_TOKEN_OP_PLUS_ASSIGN);
		default:
			return undo, token(JAVA_TOKEN_OP_PLUS);
		}
	case '-':
		switch (c = next) {
		case '-':
			return token(JAVA_TOKEN_OP_DECR);
		case '=':
			return token(JAVA_TOKEN_OP_MINUS_ASSIGN);
		default:
			return undo, token(JAVA_TOKEN_OP_MINUS);
		}
	case '*':
		switch (c = next) {
		case '=':
			return token(JAVA_TOKEN_OP_MUL_ASSIGN);
		default:
			return undo, token(JAVA_TOKEN_OP_MUL);
		}
	case '/':
		switch (c = next) {
		case '/':
			while ((c = next) != '\n' &&c);
			undo;
			goto scan;
		case '=':
			return token(JAVA_TOKEN_OP_DIV_ASSIGN);
		case '*'://注释
			while (1) {
				c = next;
				if (c == '\n') lineno++;
				if (c == '*') {
					//注释完
					if ((c = next) == '/') {
						goto scan;
					}
					else {
						undo;
					}
				}

				if (c == 0) {
					error("Unclose comment");
					return token(JAVA_TOKEN_ILLEGAL);
				}
			}
		}
	case '!':
		if (next == '=') {
			return token(JAVA_TOKEN_OP_NEQ);
		}
		else {
			return token(JAVA_TOKEN_OP_NOT);
		}
	case '>':
		switch (c = next) {
		case '=':return token(JAVA_TOKEN_OP_GTE);
		case '>':switch (c = next) {
		case '=':return token(JAVA_TOKEN_OP_BIT_SHR_ASSIGN);
		case '>': return '=' == next ? token(JAVA_TOKEN_OP_BIT_SHR_ZERO_ASSIGN) : \
			(undo, token(JAVA_TOKEN_OP_BIT_SHR_ZERO));
		default: return undo, token(JAVA_TOKEN_OP_BIT_SHR);
		}
		default:
			return undo, token(JAVA_TOKEN_OP_GT);
		}
	case '<':
		switch (c = next) {
		case '=':return token(JAVA_TOKEN_OP_LTE);
		case '<':
			return '=' == next ? token(JAVA_TOKEN_OP_BIT_SHL_ASSIGN)\
				: (undo, token(JAVA_TOKEN_OP_BIT_SHL));
		default:
			return undo, token(JAVA_TOKEN_OP_LT);
		}
	case '=':
		return '=' == next ? token(JAVA_TOKEN_OP_EQ) : (undo, token(JAVA_TOKEN_OP_ASSIGN));
	case '|':
		switch (c = next) {
		case '|':return token(JAVA_TOKEN_OP_OR);
		case '=':return token(JAVA_TOKEN_OP_BIT_OR_ASSIGN);
		default:return (undo, token(JAVA_TOKEN_OP_BIT_OR));
		}
	case '&':
		switch (c = next) {
		case '&': return token(JAVA_TOKEN_OP_AND);
		case '=': return token(JAVA_TOKEN_OP_BIT_AND_ASSIGN);
		default: return undo, token(JAVA_TOKEN_OP_BIT_AND);
		}
	case '"':
		//搜索字符串
		return scan_string(c);
	case 0:
		return token(JAVA_TOKEN_EOS);
	default:
		//标识符
		if (isalpha(c) || c == '_' || c == '$') return scan_identity(c);
		if (isdigit(c) || c == '.') return scan_number(c);
		error("illegal Token");
		return 0;

	}

}

static int CharToNum(const char c) {
	if (c >= '0' and c <= '9') return c - '0';
	if (c >= 'a' and c <= 'z') return c - 'a' + 10;
	if (c >= 'A' and c <= 'Z') return c - 'A' + 10;
	return -1;
}
int lexer::hex_literal() {
	int a = CharToNum(next);
	int b = CharToNum(next);
	int c = CharToNum(next);
	int d = CharToNum(next);
	if (a > -1 and b > -1 and c > -1 and d > -1) return a << 12 | b << 8 | c << 4 | d;
	error("string hex literal \\uxxxx contains invalid digits");
	return -1;
}

int lexer::oct_literal() {
	int a = CharToNum(next);
	int b = CharToNum(next);
	int c = CharToNum(next);
	if (a > -1 and b > -1 and c > -1) return a << 6 | b << 3 | c;
	error("String oct literal \\ddd contains invalid digits");
	return -1;
}

int lexer::scan_string(int quate) {
	int c, len = 0;
	string buf;
	token(JAVA_TOKEN_CONSTANT_STRING);
	//遇见下一个引号字符串停止
	while (quate != (c = next)) {
		switch (c) {
		case '\n':
			lineno++;
			break;
			//反斜杠字符，转义,此处 C++'\\'代表 输入的'\'， 易错
		case '\\':
			switch (c = next) {
			case '\'':
				c = '\'';
				break;
			case'\\':
				c = '\\';
				break;
			case 'a':
				c = '\a';
				break;
			case 'b':
				c = '\b';
				break;
			case 'r':
				c = '\r';
				break;
			case 'n':
				c = '\n';
				break;
			case 'f':
				c = '\f';
				break;
			case 't':
				c = '\t';
				break;
			case 'u':
				//16进制表示的字符；
				c = hex_literal();
				if (-1 == c) {
					goto error;
				}
				// /ddd八进制字符 unfinished
			//case 'k'://
			//	c = oct_literal();
			//	if (-1 == c) {
			//		goto error;
			//	}
			//	//八进制表示字符
			}
			break;
		}
		buf += c;
	}
	tok.value.as_string = buf;
	buf.clear();
	//成功输出 字符串
	return 1;
error:
	buf.clear();
	return 0;
}

int lexer::scan_identity(int c) {
	//默认为标识符
	string buf;
	token(JAVA_TOKEN_ID);
	//字母或美元符号“$”或下划线开头，连接字母或美元符号“$”或下划线或数字字符的串。
	do {
		buf += c;
	} while (isalpha(c = next) || c == '_' || c == '$');
	undo;

	//检查 是否为关键字/ 若是返回关键字属性值  
	if (buf == "if") return clear_token(JAVA_TOKEN_IF);
	if (buf == "do") return clear_token(JAVA_TOKEN_DO);

	if (buf == "for")return clear_token(JAVA_TOKEN_FOR);
	if (buf == "int")return clear_token(JAVA_TOKEN_INT);
	if (buf == "new")return clear_token(JAVA_TOKEN_NEW);
	if (buf == "try")return clear_token(JAVA_TOKEN_TRY);

	if (buf == "void")return clear_token(JAVA_TOKEN_VOID);
	if (buf == "this")return clear_token(JAVA_TOKEN_THIS);
	if (buf == "char") return clear_token(JAVA_TOKEN_CHAR);
	if (buf == "byte")return clear_token(JAVA_TOKEN_BYTE);
	if (buf == "case")return clear_token(JAVA_TOKEN_CASE);
	if (buf == "else")return clear_token(JAVA_TOKEN_ELSE);
	if (buf == "long")return clear_token(JAVA_TOKEN_LONG);
	if (buf == "enum")return clear_token(JAVA_TOKEN_ENUM);
	if (buf == "goto")return clear_token(JAVA_TOKEN_GOTO);
	if (buf == "true")return clear_token(JAVA_TOKEN_CONSTANT_TRUE);
	if (buf == "null")return clear_token(JAVA_TOKEN_NULLS);
	if (buf == "false")return clear_token(JAVA_TOKEN_CONSTANT_FALSE);
	if (buf == "const")return clear_token(JAVA_TOKEN_CONST);
	if (buf == "short")return clear_token(JAVA_TOKEN_SHORT);
	if (buf == "break")return clear_token(JAVA_TOKEN_BREAK);
	if (buf == "catch")return clear_token(JAVA_TOKEN_CATCH);
	if (buf == "class")return clear_token(JAVA_TOKEN_CLASS);
	if (buf == "final")return clear_token(JAVA_TOKEN_FINAL);
	if (buf == "float")return clear_token(JAVA_TOKEN_FLOAT);
	if (buf == "super")return clear_token(JAVA_TOKEN_SUPER);
	if (buf == "throws") return clear_token(JAVA_TOKEN_THROWS);
	if (buf == "while")return clear_token(JAVA_TOKEN_WHILE);
	if (buf == "native")return clear_token(JAVA_TOKEN_NATIVE);
	if (buf == "static")return clear_token(JAVA_TOKEN_STATIC);
	if (buf == "switch")return clear_token(JAVA_TOKEN_SWITCH);
	if (buf == "import")return clear_token(JAVA_TOKEN_IMPORT);
	if (buf == "double")return clear_token(JAVA_TOKEN_DOUBLE);
	if (buf == "public")return clear_token(JAVA_TOKEN_PUBLIC);
	if (buf == "return")return clear_token(JAVA_TOKEN_RETURN);
	if (buf == "boolean")return clear_token(JAVA_TOKEN_BOOLEAN);
	if (buf == "package")return clear_token(JAVA_TOKEN_PACKAGE);
	if (buf == "private")return clear_token(JAVA_TOKEN_PRIVATE);
	if (buf == "default")return clear_token(JAVA_TOKEN_DEFAULT);
	if (buf == "extends")return clear_token(JAVA_TOKEN_EXTENDS);
	if (buf == "finally")return clear_token(JAVA_TOKEN_FINALLY);
	if (buf == "volatile")return clear_token(JAVA_TOKEN_VOLATILE);
	if (buf == "abstract")return clear_token(JAVA_TOKEN_ABSTRACT);
	if (buf == "continue")return clear_token(JAVA_TOKEN_CONTINUE);
	if (buf == "interface")return clear_token(JAVA_TOKEN_INTERFACE);
	if (buf == "protected")return clear_token(JAVA_TOKEN_PROTECTED);
	if (buf == "transient")return clear_token(JAVA_TOKEN_TRANSIENT);
	if (buf == "implements")return clear_token(JAVA_TOKEN_IMPLEMENTS);
	if (buf == "instanceof")return clear_token(JAVA_TOKEN_INSTANCEOF);
	if (buf == "synchronized")return clear_token(JAVA_TOKEN_SYNCHRONIZED);
	//否则该标识符不是关键字
	tok.value.as_string = buf;
	return 1;
}



//识别数字
int lexer::scan_number(int c) {
	int n = 0, type = 0, expo = 0, e;
	int expo_type = 1;
	token(JAVA_TOKEN_CONSTANT_INT);
	//默认为整数;

	switch (c) {
	case '0':
		switch (c = next) {
			//16进制数
		case 'X':goto scan_hex;
		case 'x': goto scan_hex;
		default:
			undo;
			c = '0';
			goto scan_int;
		}
	default:
		goto scan_int;
	}

	//检测为16进制数
scan_hex:
	{
		if (!isxdigit(c = next))
		{
			error("hex literal expects one or more digits");
			return 0;
		}
		else
		{
			do {
				n = n << 4 | CharToNum(c);
			} while (isxdigit(c = next));
			undo;
			return 1;

		}


	}
	//检测整数部分
scan_int:
	{

		do {
			//JAVA 规范，数字连接符
			if ('_' == c) continue;
			//存在小数
			else if ('.' == c) goto scan_float;
			//科学计数法
			else if ('e' == c or 'E' == c)goto scan_expo;
			n = n * 10 + c - '0';

		} while (isdigit(c = next) or '_' == c or '.' == c or 'e' == c or 'E' == c);
		//只是整数

		//结束为'L'用长整形
		if (c == 'L') {
			tok.value.as_long = n;
			token(JAVA_TOKEN_CONSTANT_LONG);
		}
		//否则用Int 存储
		else {
			undo;
			tok.value.as_int = n;
			token(JAVA_TOKEN_CONSTANT_INT);
		}
		return 1;
	}

	//检测小数部分
scan_float:
	{
		e = 1;
		type = 1;
		//小数type
		token(JAVA_TOKEN_CONSTANT_DOUBLE);
		//当前c='.'
		while (isdigit(c = next) or '_' == c or 'e' == c or 'E' == c) {
			if (c == '_')continue;
			else if ('e' == c || 'E' == c)goto scan_expo;
			n = n * 10 + c - '0';
			e *= 10;
		}
		//无科学计数法
		//指定用float进行存储   
		if (c = 'F') {
			token(JAVA_TOKEN_CONSTANT_FLOAT);
			tok.value.as_float = (float)n / e;
		}
		//否则默认用DOUBLE储存
		else {
			undo;
			tok.value.as_real = (double)n / e;
		}
		return 1;

	}

	//出现科学计数法
scan_expo:
	{
		while (isdigit(c = next) or '+' == c or '-' == c) {
			if ('-' == c) {
				if (expo_type == 1) {
					expo_type = 0;
					continue;
				}
				else {
					error("invalid digit");
					return 0;
				}
			}
			if ('+' == c) {
				if (expo_type == 1) {
					expo_type = 2;
					continue;
				}
				else {
					error("invalid digit");
					return 0;
				}
			}
			expo = expo * 10 + c - '0';
		}
		undo;
		if (expo_type == 0) expo *= -1;
		if (type == 0)
			tok.value.as_int = n * pow(10, expo);
		else
			tok.value.as_real = ((double)n / e)*pow(10, expo);
		return 1;
	}
}

/*
打印token信息
*/

void lexer::java_token_to_xml(const java_token_t &tok) {
	cerr << "<" << java_token_xml_type[tok.token_type] << ">\t";
	if (tok.token_type == JAVA_TOKEN_CONSTANT_STRING || tok.token_type == JAVA_TOKEN_ID) {
		cerr << "Value:" << tok.value.as_string<<"\t";
	}
	else if (tok.token_type == JAVA_TOKEN_CONSTANT_INT) {
		cerr << "Value:" << tok.value.as_int << "\t";
	}
	else if (tok.token_type == JAVA_TOKEN_CONSTANT_LONG) {
		cerr << "Value:" << tok.value.as_long << "\t";
	}
	else if (tok.token_type == JAVA_TOKEN_CONSTANT_FLOAT) {
		cerr << "Value:" << tok.value.as_float << "\t";
	}
	else if (tok.token_type == JAVA_TOKEN_CONSTANT_DOUBLE) {
		cerr << "Value:" << tok.value.as_real << "\t";
	}
	else if (tok.token_type == JAVA_TOKEN_ILLEGAL) {
		cerr << "Value:Null"<< error << endl << "\t";
	}
	else {
		cerr << "Value:Null\t";
	}
	//输出属性值
	cerr << "Entity: 0x" << hex << EnumTohex(tok.token_type) << "\t";
	cerr << "<" << java_token_xml_type[tok.token_type] << ">" << endl;
}

//将token的enum转化成16进制属性值，按照实验要求
unsigned int lexer::EnumTohex(int JAVA_TOKEN) {
	//错误单词 ILLEGAL
	if (JAVA_TOKEN == JAVA_TOKEN_ILLEGAL) {
		return 0x100;
	}
	//关键字
	else if (JAVA_TOKEN >= JAVA_TOKEN_DO and JAVA_TOKEN < +JAVA_TOKEN_SYNCHRONIZED) {
		return 0x103;
	}
	//标识符
	else if (JAVA_TOKEN == JAVA_TOKEN_ID) {
		return 0x104;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_CONSTANT_TRUE or JAVA_TOKEN == JAVA_TOKEN_CONSTANT_FALSE) {
		return 0x105;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_CONSTANT_CHAR) {
		return 0x106;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_CONSTANT_INT or JAVA_TOKEN == JAVA_TOKEN_LONG) {
		return 0x107;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_CONSTANT_FLOAT or JAVA_TOKEN == JAVA_TOKEN_CONSTANT_DOUBLE) {
		return 0x108;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_CONSTANT_STRING) {
		return 0x109;
	}
	else if (JAVA_TOKEN >= JAVA_TOKEN_OP_ASSIGN and \
		JAVA_TOKEN <= JAVA_TOKEN_OP_BIT_SHR_ASSIGN) {
		return 0x110;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_COND) {
		return 0x111;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_OR)
	{
		return 0x112;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_AND) {
		return 0x113;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_BIT_OR) {
		return 0x114;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_BIT_XOR) {
		return 0x115;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_BIT_AND) {
		return 0x116;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_EQ or JAVA_TOKEN == JAVA_TOKEN_OP_NEQ) {
		return 0x117;
	}
	else if (JAVA_TOKEN >= JAVA_TOKEN_OP_GT and JAVA_TOKEN <= JAVA_TOKEN_OP_LTE) {
		return 0x118;
	}
	else if (JAVA_TOKEN >= JAVA_TOKEN_OP_BIT_SHL and JAVA_TOKEN <= JAVA_TOKEN_OP_BIT_SHR_ZERO) {
		return 0x119;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_PLUS || JAVA_TOKEN == JAVA_TOKEN_OP_MINUS)
	{
		return 0x11a;
	}
	else if (JAVA_TOKEN >= JAVA_TOKEN_OP_MUL and JAVA_TOKEN <= JAVA_TOKEN_OP_MOD) {
		return 0x11b;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_OP_INCR or JAVA_TOKEN == JAVA_TOKEN_OP_DECR) {
		return 0x11c;
	}
	else if (JAVA_TOKEN >= JAVA_TOKEN_LPAREN and JAVA_TOKEN <= JAVA_TOKEN_RBRACK) {
		return 0x11b;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_COMMA) {
		return 0x120;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_LBRACE or JAVA_TOKEN <= JAVA_TOKEN_RBRACE) {
		return 0x121;
	}
	else if (JAVA_TOKEN == JAVA_TOKEN_SEMICOLON) {
		return 0x122;
	}

	return 0x123;
}


//
//size_t
//file_size(FILE *stream) {
//	size_t size = 0;
//	fseek(stream, 0, SEEK_END);
//	size = ftell(stream);
//	rewind(stream);
//	return size;
//}

char * file_read(const string filename) {

	ifstream readfile(filename);
	if (!readfile) cerr << "file is not existed" << endl;
	else {
		//int l;
		//readfile.seekg(0,ios::end);
		//l = readfile.tellg();
		//readfile.seekg(0, ios::beg);
		//char *buffer = new char[l+5];
		//readfile.read(buffer, l);
		//readfile.close();
		//return buffer;
		//不知道为什么出现乱码

		string str((istreambuf_iterator<char>(readfile)), \
			std::istreambuf_iterator<char>());
		char* ans = new char[str.length() + 1];
		/*	cerr << strlen(str.c_str()) << "  " << str.length() << endl;*/
		strcpy_s(ans, str.length() + 1, str.c_str());
		return ans;
	}
	return NULL;
}

void lexer::set_filename(string f) {
	filename = f;
}

void lexer::scan() {
	cerr << "Usage:" << endl;
	if (filename.empty()) {
		cerr << "Please set filename" << endl;
		string f;
		cin >> f;
		set_filename(f);
	}
	cerr << "<filename>" << filename << "<filename>" << endl;
	//读取文件内容
	source = file_read(filename);
	//cerr << source << endl;
	cerr << "Start Analysis" << endl;
	while (tok.token_type != JAVA_TOKEN_EOS) {
		java_lexer_reset();
		java_scan();
		java_token_to_xml(tok);
	}
	free(source);
	cerr << "Word Analysis Finished" << endl;
}

