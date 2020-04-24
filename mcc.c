#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

void error_at(char*,char* fmt,...);
typedef struct Token Token;	// main parse

struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char* str;
};

//見ているトークン
Token *token;
//プログラム
char* user_input;

void error(char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}

bool consume(char op) {
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next; //進める
	return true;
}

void expect(char op) {
	if(token->kind != TK_RESERVED || token->str[0] != op) 
		error_at(token->str,"'%c'ではありません。",op);
	token = token->next;
}

int expect_number() {
	if(token->kind != TK_NUM)
		error_at(token->str,"数が予測されましたが、数ではありませんでした。");
	int val = token->val;
	token = token->next;

	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind,Token *prev,char *str) {
	Token* tok = calloc(1,sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	prev->next = tok;
	return tok;
}

//文字列をトークンのリンクドリストにする
Token* tokenize(char *p) {
	Token head;
	head.next = NULL;
	Token *prev = &head;

	while (*p) {
		if(isspace(*p)) {
			p++;
			continue;
		}

		if(*p == '+' || *p == '-' ) {
			prev = new_token(TK_RESERVED,prev,p++);
			continue;
		}

		if(isdigit(*p)) {
			prev = new_token(TK_NUM,prev,p);
			prev->val = strtol(p,&p,10);
			continue;
		}
	
		printf("Error!!!\n");	
		error_at(prev->str,"tokenize error");
	}
	new_token(TK_EOF,prev,p);
	return head.next;
}

void print_token(Token *tok) {
	//tokの指している場所から最後まで表示
	Token* cur = tok;
	while(cur != NULL) {
		printf("%s",cur->str);
		cur = cur->next;
	}
	printf("\n");
}

void error_at(char* loc,char *fmt,...) {
	va_list ap;
	va_start(ap,fmt);

	//どこでエラーが起きているのか
	int pos = (loc - user_input) / sizeof(char);
	fprintf(stderr,"%s\n",user_input);
	fprintf(stderr,"%*s",pos,"");
	fprintf(stderr,"^ ");
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}

int main(int argc,char **argv) {
	if(argc != 2) { 
		fprintf(stderr,"Error\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	printf("  mov rax, %d\n",expect_number());
	while(!at_eof()) {
		if(consume('+')) {
			printf("  add rax, %d\n",expect_number());
		} else if(consume('-')) {
			printf("  sub rax, %d\n",expect_number());
		} else {
			error_at(token->str,"予期していない文字です");
		}
	}
	
	printf("  ret\n");
	return 0;
}
