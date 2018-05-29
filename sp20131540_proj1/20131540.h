#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include<sys/stat.h>

typedef struct _his_node{
	char *str;	//입력받은 명령어
	struct _his_node *link;
}his_node;	//명령어 history를 구성하는 node
typedef struct _op_node{
	int opcode;	//opcode
	char mnemo[7];	//명령어
	short int format;	//format
	struct _op_node *link;
}op_node;	//hash table를 구성하는 node
static his_node *history_header;	//명령어 history를 구성하는 header
static op_node *op_header[20];	//hash table을 구성하는 header
static unsigned char memory[1048576];	//총 16*65536 
void printWrong();	//잘못된 입력이 들어왔을 때 호출하는 함수
void printHelp();	//help명령어가 들어왔을 때 호출하는 함수
void printHistory();	//history명령어가 들어왔을 때 호출하는 함수
void printDump(int _start, int _end);	//dump명령어가 들어왔을 때 호출하는 함수
void printDir();	//dir명령어가 들어왔을 때호출하는 함수
void addToHistory(char *input);	//명령어 history를 추가하는 함수
void readOpcode(FILE *fp);	//opcode.txt에서 opcode정보를 읽는 함수
void printOplist();	//opcodelist명령어가 들어왔을 때 호출하는 함수
void getInput(char *input); //사용자에게 받은 input을 판단하는 함수

