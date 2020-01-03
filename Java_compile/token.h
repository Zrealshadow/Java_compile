#pragma once
#include<string>
#include<vector>
#include<assert.h>
#include <iostream>
using namespace std;


// 关键字
#define JC_KEYWORD_LIST \
	t(DO,"do") \
	t(IF,"if") \
	t(FOR,"for")\
	t(INT,"int") \
	t(NEW,"new") \
	t(TRY,"try") \
	t(VOID,"void") \
	t(THIS,"this") \
	t(CHAR,"char") \
	t(BYTE,"byte") \
	t(CASE,"case") \
	t(ELSE,"else") \
	t(LONG,"long") \
	t(ENUM,"enum") \
	t(GOTO,"goto") \
	t(NULLS,"null")\
	t(CONST,"const") \
	t(SHORT,"short") \
	t(BREAK,"break") \
	t(CATCH,"catch") \
	t(CLASS,"class") \
	t(FINAL,"final") \
	t(FLOAT,"float") \
	t(SUPER,"super") \
	t(THROWS,"throws") \
    t(WHILE,"while")\
	t(NATIVE,"native") \
	t(STATIC,"static") \
	t(SWITCH,"switch") \
	t(IMPORT,"import") \
	t(DOUBLE,"double") \
	t(PUBLIC,"public") \
	t(RETURN,"return") \
	t(BOOLEAN,"boolean") \
	t(PACKAGE,"package") \
	t(PRIVATE,"private") \
	t(DEFAULT,"default") \
	t(EXTENDS,"extends") \
	t(FINALLY,"finally") \
	t(VOLATILE,"volatile") \
	t(ABSTRACT,"abstract") \
	t(CONTINUE,"continue") \
	t(INTERFACE,"interface") \
	t(PROTECTED,"protected") \
	t(TRANSIENT,"transient") \
	t(IMPLEMENTS,"implements") \
	t(INSTANCEOF,"instanceof") \
	t(SYNCHRONIZED,"synchronized")
// 符号
#define JC_OP_LIST \
  t(OP_DOT, ".") \
  t(OP_NOT, "!") \
  t(OP_PLUS, "+") \
  t(OP_INCR, "++") \
  t(OP_MINUS, "-") \
  t(OP_DECR, "--") \
  t(OP_MUL, "*") \
  t(OP_DIV, "/") \
  t(OP_MOD, "%") \
  t(OP_GT, ">") \
  t(OP_LT, "<") \
  t(OP_GTE, ">=") \
  t(OP_LTE, "<=") \
  t(OP_EQ, "==") \
  t(OP_NEQ, "!=") \
  t(OP_AND, "&&") \
  t(OP_OR, "||") \
\
  t(OP_ASSIGN, "=") \
  t(OP_PLUS_ASSIGN, "+=") \
  t(OP_MINUS_ASSIGN, "-=") \
  t(OP_MUL_ASSIGN, "*=") \
  t(OP_DIV_ASSIGN, "/=") \
  t(OP_MOD_ASSIGN,"%=") \
  t(OP_BIT_SHR_ASSIGN, ">>=") \
  t(OP_BIT_SHL_ASSIGN, "<<=") \
  t(OP_BIT_AND_ASSIGN,"&=") \
  t(OP_BIT_NOT_ASSIGN,"^=") \
  t(OP_BIT_OR_ASSIGN,"|=") \
  t(OP_BIT_SHR_ZERO_ASSIGN,">>>=") \
\
  t(OP_BIT_OR, "|") \
  t(OP_BIT_AND, "&") \
  t(OP_BIT_XOR, "^") \
  t(OP_BIT_NOT, "~") \
  t(OP_BIT_SHL, "<<") \
  t(OP_BIT_SHR, ">>") \
\
  t(OP_BIT_SHR_ZERO,">>>") \
  t(OP_COND,"?:")

//基本类型
#define JC_BASE_TOKEN_LIST \
    t(LBRACE, "{") \
    t(RBRACE, "}") \
    t(LPAREN, "(") \
    t(RPAREN, ")") \
    t(LBRACK, "[") \
    t(RBRACK, "]") \
    t(COLON, ":") \
    t(QMARK, "?") \
    t(SEMICOLON, ";") \
    t(COMMA, ",") \
    t(ID, "ID")\
    t(ILLEGAL, "illegal") \
    t(EOS, "EOS") \
    t(CONSTANT_TRUE, "true") \
    t(CONSTANT_FALSE, "false") \
    t(CONSTANT_STRING, "string") \
	t(CONSTANT_DOUBLE,"double")\
	t(CONSTANT_LONG,"long")\
    t(CONSTANT_INT, "int") \
    t(CONSTANT_CHAR, "char") \
    t(CONSTANT_FLOAT, "float")

// 重新定义枚举类型 枚举(前缀 JAVA_TOKEN_{ })
typedef enum {
#define t(token,str) JAVA_TOKEN_##token,
	JC_KEYWORD_LIST
	JC_OP_LIST
	JC_BASE_TOKEN_LIST
#undef t
} java_token;

//枚举类型对应的 string
//static const vector<string> java_token_string = {
//#define t(token,str) str,
//	JC_KEYWORD_LIST
//	JC_OP_LIST
//	JC_BASE_TOKEN_LIST
//#undef t
//};


//关键字属性表
typedef struct {
	java_token token_type;
	struct {
		string as_string;
		double as_real;
		float as_float;
		int as_int;
		long int as_long;
	}value;
}java_token_t;

static vector<string> java_token_string = {
#define t(token,str) str,
	JC_KEYWORD_LIST
	JC_OP_LIST
	JC_BASE_TOKEN_LIST
#undef t
};

#define STR_HELPER(x) #x

static vector<string> java_token_xml_type = {
#define t(tok,str) STR_HELPER(tok),
	JC_KEYWORD_LIST
	JC_OP_LIST
	JC_BASE_TOKEN_LIST
#undef t
};
class token
{
public:
	token();
	~token();
	inline string java_token_type(java_token type) {
		//枚举变量最后一个，防止越界
		assert(type < +JAVA_TOKEN_CONSTANT_FLOAT);
		return java_token_string[type];
	}
	void java_token_inspect(java_token_t *tok);

};



