#include "20131540.h"

int main(){
	int i;
	FILE *fp;
	history_header = NULL;
	sym_header = NULL;
	for(i=0;i<20;i++)	op_header[i] = NULL;
	for(i=0;i<1048576;i++)	memory[i] = 0;		
	
	fp = fopen("opcode.txt","r");
	if(!fp){
		printf("File Open Error\n");
		return -1;
	}
	readOpcode(fp);
	fclose(fp);

	while(1){
		printf("sicsim> "); 
		getInput();
	}
	return 0;
}
