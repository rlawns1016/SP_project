#include "20131540.h"

int main(){
	char input[100];	//�ִ� ���̰� 100�� input�� �޴´ٰ� �Ѵ�.
	int i;
	history_header = NULL;
	for(i=0;i<20;i++)	op_header[i] = NULL;
	//history�� hash table�� �����ϱ� ���� header���� �ʱ��Ѵ�.
	for(i=0;i<1048576;i++)	memory[i] = 0;	//memory �ʱ�ȭ	
	FILE *fp;

	fp = fopen("opcode.txt","r");
	if(!fp){
		printf("File Open Error\n");
		return -1;
	}
	readOpcode(fp);	//opcode������ �޾Ƽ� hashtable�� �����Ѵ�.
	fclose(fp);

	while(1){
		printf("sicsim> "); 
		getInput(input);	//quit�� �Է¹����� ���α׷��� ����ȴ�.
	}
	return 0;
}
