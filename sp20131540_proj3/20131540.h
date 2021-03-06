#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include<sys/stat.h>

typedef struct _his_node{
	char *str;	
	struct _his_node *link;
}his_node;	
typedef struct _op_node{
	int opcode;
	char mnemo[7];
	short int format;
	struct _op_node *link;
}op_node;
typedef struct _sym_node{
	int locctr;
	char *label;
	struct _sym_node *link;
}sym_node;
enum _tag{csa, esym}tag;
union _node{
	struct _csa_node{
		char label[10];
		int addr;
		int length;
	}csa;
	struct _esym_node{
		char label[10];
		int addr;
	}esym;
}node;
typedef struct _estab_node{
	enum _tag tag;
	union _node node;
	struct _estab_node *link;
}estab_node;
static estab_node *estab_header;
static his_node *history_header;
static op_node *op_header[20];	
static sym_node *sym_header;
static unsigned char memory[1048576]; 
static int bp[100];
void printWrong();
void printHelp();
void printHistory();
void printDump(int _start, int _end);
void printDir();
void addToHistory(char *input);
void readOpcode(FILE *fp);
void printOplist();
void getInput();
void printFile(FILE *fp);
void printSymbol();
int getLoc(char *label);
int getOpcode(char *mnemo);
int getFormat(char *mnemo);
short int isSingleMnemo(char *mnemo);
int getRegNum(char *mnemo);
void setStr(char *str);
int pass2(FILE *fp, char *fileName, int start_loc, int end_loc, int _base_loc);
int pass1(FILE *fp, char *fileName);
int loadPass1(FILE *fp1, FILE *fp2, FILE *fp3);
int loadPass2(FILE *fp1, FILE *fp2, FILE *fp3);
void printEstab();
void freeEstab();
int isInEstab(char *label);
void run_();
void setReg(int pc, int *r1, int *r2);
int getAddr(int *reg, int *addr, int s_addr);
