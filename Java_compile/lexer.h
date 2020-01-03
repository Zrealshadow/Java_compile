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

	//每次检测出一个单词后，对tok进行重置
	void java_lexer_reset();

	//开始扫描，并输出到scan_out
	void scan();

	//设定源代码文件路径
	void set_filename(string f);
	//重新设定输出文件路径 （默认为当前目录下scan_out文件）
	void reset_out_filename(string f);

private:
	//保存错误信息
	string error;
	//当前检测到的字符
	int stash;
	//当前行数
	int lineno;
	//在源代码上以字符为单位，当前偏移量
	off_t offset;
	
	//源代码以char 数组的形式储存
	// 文件末尾以 \0结尾 用于判断 所以用char *
	char* source;
	
	//记录当前检测出单词的类型及取值
	java_token_t tok;

	//源代码路径
	string filename;
	//输出结果路径
	string outfilename;


	//程序中心法 扫描接口，消除注释，对符号类型的字符直接进行标识
	int java_scan();
	
	//java_scan()子函数，扫描到'\"' 开始检测是否为CONSTANT_STRING
	int scan_string(int q);

	//java_scan()子函数，扫描到 [_A-Za-z$]开始检测为关键字或标识符
	int scan_identity(int c);

	//javca_scan()子函数，扫描到 [0-9.]开始检测，其中得检测不同的数据类型
	int scan_number(int c);

	// /uxxxx
	int hex_literal();//处理16进制字符
	// /ddd
	int oct_literal();//处理8进制字符

	//将得到的Token 标准化打印
	void java_token_to_xml(const java_token_t &tok);
	//将Token内容标准化返回string，便于写入scan_out
	string java_token_result_infile_xml(const java_token_t &tok);
	//辅助函数，根据实验要求，将枚举类型java_token转变为三位16进制 属性值
	unsigned int EnumTohex(int JAVA_TOKEN);
};

