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

	//ÿ�μ���һ�����ʺ󣬶�tok��������
	void java_lexer_reset();

	//��ʼɨ�裬�������scan_out
	void scan();

	//�趨Դ�����ļ�·��
	void set_filename(string f);
	//�����趨����ļ�·�� ��Ĭ��Ϊ��ǰĿ¼��scan_out�ļ���
	void reset_out_filename(string f);

private:
	//���������Ϣ
	string error;
	//��ǰ��⵽���ַ�
	int stash;
	//��ǰ����
	int lineno;
	//��Դ���������ַ�Ϊ��λ����ǰƫ����
	off_t offset;
	
	//Դ������char �������ʽ����
	// �ļ�ĩβ�� \0��β �����ж� ������char *
	char* source;
	
	//��¼��ǰ�������ʵ����ͼ�ȡֵ
	java_token_t tok;

	//Դ����·��
	string filename;
	//������·��
	string outfilename;


	//�������ķ� ɨ��ӿڣ�����ע�ͣ��Է������͵��ַ�ֱ�ӽ��б�ʶ
	int java_scan();
	
	//java_scan()�Ӻ�����ɨ�赽'\"' ��ʼ����Ƿ�ΪCONSTANT_STRING
	int scan_string(int q);

	//java_scan()�Ӻ�����ɨ�赽 [_A-Za-z$]��ʼ���Ϊ�ؼ��ֻ��ʶ��
	int scan_identity(int c);

	//javca_scan()�Ӻ�����ɨ�赽 [0-9.]��ʼ��⣬���еü�ⲻͬ����������
	int scan_number(int c);

	// /uxxxx
	int hex_literal();//����16�����ַ�
	// /ddd
	int oct_literal();//����8�����ַ�

	//���õ���Token ��׼����ӡ
	void java_token_to_xml(const java_token_t &tok);
	//��Token���ݱ�׼������string������д��scan_out
	string java_token_result_infile_xml(const java_token_t &tok);
	//��������������ʵ��Ҫ�󣬽�ö������java_tokenת��Ϊ��λ16���� ����ֵ
	unsigned int EnumTohex(int JAVA_TOKEN);
};

