#include "20131540.h"

int main(){
	char input[100];	//최대 길이가 100인 input을 받는다고 한다.
	int i;
	history_header = NULL;
	for(i=0;i<20;i++)	op_header[i] = NULL;
	//history와 hash table를 구성하기 위한 header들을 초기한다.
	for(i=0;i<1048576;i++)	memory[i] = 0;	//memory 초기화	
	FILE *fp;

	fp = fopen("opcode.txt","r");
	if(!fp){
		printf("File Open Error\n");
		return -1;
	}
	readOpcode(fp);	//opcode정보를 받아서 hashtable을 생성한다.
	fclose(fp);

	while(1){
		printf("sicsim> "); 
		getInput(input);	//quit을 입력받으면 프로그램이 종료된다.
	}
	return 0;
}
