#include "pch.h"
#include "token.h"

token::token()
{
}


token::~token()
{
}

void token::java_token_inspect(java_token_t *tok) {
	cerr << "TokenType: " << java_token_string[tok->token_type]<<" ";
	if (tok->token_type == JAVA_TOKEN_CONSTANT_STRING) {
		cerr << "Value:" << tok->value.as_string;
	}
	else if (tok->token_type == JAVA_TOKEN_CONSTANT_INT) {
		cerr << "Value: " << tok->value.as_int;
	}
	else if (tok->token_type == JAVA_TOKEN_CONSTANT_FLOAT) {
		cerr << "Value: " << tok->value.as_real;
	}
	else if (tok->token_type == JAVA_TOKEN_ID) {
		cerr << "Value: " << tok->value.as_string;
	}
	cerr << "***********************************************************";
	cerr << endl;
}