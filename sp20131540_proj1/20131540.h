#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include<sys/stat.h>

typedef struct _his_node{
	char *str;	//�Է¹��� ��ɾ�
	struct _his_node *link;
}his_node;	//��ɾ� history�� �����ϴ� node
typedef struct _op_node{
	int opcode;	//opcode
	char mnemo[7];	//��ɾ�
	short int format;	//format
	struct _op_node *link;
}op_node;	//hash table�� �����ϴ� node
static his_node *history_header;	//��ɾ� history�� �����ϴ� header
static op_node *op_header[20];	//hash table�� �����ϴ� header
static unsigned char memory[1048576];	//�� 16*65536 
void printWrong();	//�߸��� �Է��� ������ �� ȣ���ϴ� �Լ�
void printHelp();	//help��ɾ ������ �� ȣ���ϴ� �Լ�
void printHistory();	//history��ɾ ������ �� ȣ���ϴ� �Լ�
void printDump(int _start, int _end);	//dump��ɾ ������ �� ȣ���ϴ� �Լ�
void printDir();	//dir��ɾ ������ ��ȣ���ϴ� �Լ�
void addToHistory(char *input);	//��ɾ� history�� �߰��ϴ� �Լ�
void readOpcode(FILE *fp);	//opcode.txt���� opcode������ �д� �Լ�
void printOplist();	//opcodelist��ɾ ������ �� ȣ���ϴ� �Լ�
void getInput(char *input); //����ڿ��� ���� input�� �Ǵ��ϴ� �Լ�

