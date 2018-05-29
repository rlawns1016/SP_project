#include "20131540.h" 

int mem_loc = 0;

void printWrong(){
	//print "Wrong Input"
	printf("Wrong Input\n");
}
void printHelp(){
	//help��ɾ �Է¹޾��� �� ��� ������ ��ɾ ����Ѵ�.
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}
void printHistory(){
	//linked list���·� ����� ��ɾ� history�� ����Ѵ�.
	his_node *temp = history_header;	//Ž���� ���� ������
	int cnt = 0;
	while(temp != NULL){
		printf("%d\t%s\n",++cnt,temp->str);
		temp = temp->link;
	}
}
void addToHistory(char *input){
	//������ ��ɾ�(input)�� �Է¹����� history ��Ͽ� �߰��ϴ� �Լ��̴�.
	his_node *newNode;
	his_node *temp = history_header;	//Ž���� ���� ������
	//���ο� node�� �����ؼ� �ʱ�ȭ�Ѵ�.
	newNode = (his_node*)malloc(sizeof(his_node));
	newNode->str = (char*)malloc(sizeof(char)*(strlen(input)+1));
	strcpy(newNode->str,input);
	newNode->link = NULL;
	//header�� null�̸� ���ο� node�� ����Ű�� �ϰ� �ƴϸ� list ���� �߰��Ѵ�.
	if(history_header == NULL)
		history_header = newNode;
	else{
		while(temp->link != NULL)	
			temp = temp->link;
		temp->link = newNode;
	}
}
void readOpcode(FILE *fp){
	//fp�� ���Ͽ��� opcode������ �о� linked list�� ���·� hash table�� �����Ѵ�. 
	int opcode;
	char mnemo[7];
	char format[4];
	int hash;
	op_node *temp;	//Ž���� ���� ������
	//���̻� ���Ͽ��� ���� �� ���� �� ���� while loop�� ������.
	while(fscanf(fp,"%x %s %s", &opcode, mnemo, format)==3){
		//���� ������ ���� ���ο� node �ʱ�ȭ
		op_node *newNode;
		newNode = (op_node*)malloc(sizeof(op_node));
		newNode->opcode = opcode;
		newNode->link = NULL;
		strcpy(newNode->mnemo,mnemo);
		if(strcmp(format,"1") == 0)	newNode->format = 1;
		else if(strcmp(format,"2") == 0)	newNode->format = 2;
		else	newNode->format = 3;	//format "3/4"�� ������ �ϴ� format3��� �����ߴ�.
		//��ɾ��� ���ĺ� ù���ڸ� 20���� ������(hash table�� size�� 20�̱� ������) �ش��ϴ� header�� ����Ű�� list�� ���ο� node�� �߰��Ѵ�.
		hash = mnemo[0] % 20;	
		if(op_header[hash] == NULL)	op_header[hash] = newNode;
		else{
			temp = op_header[hash];
			while(temp->link != NULL)	
				temp = temp->link;
			temp->link = newNode;
		}
	}
}	
void printOplist(){
	//hash table�� ����Ǿ� �ִ� opcode list�� ����Ѵ�.
	int i;
	op_node *temp;	//Ž���� ���� ������
	for(i=0; i<20; i++){
		temp = op_header[i];
		printf("%d : ",i);
		while(temp!= NULL){
			printf("[%s,%X]",temp->mnemo, temp->opcode);
			if(temp->link != NULL)	printf(" -> ");
			temp = temp->link;
		}
		printf("\n");
	}
}
void printDump(int _start, int _end){
	//dump ���� ��ɾ �޾��� �� �޸� ������ ����Ѵ�.
	int start,end,i,j;
	if(_start < 0)	start = mem_loc;	//start�� -1�̸� dump�� �Է¹��� �����̹Ƿ� start�� mem_loc���� �����Ѵ�.
	else	start = _start;	//start�� ����� dump start�� �Է¹��� ���̴�.
	if(_end < 0){
		//end���� �����̸� dump �Ǵ� dump start���·� �Է¹��� ���̹Ƿ� end�� start+159�� �Ѵ�.(�� 160���� ����ϹǷ�)
		end = start + 159;
		mem_loc = end +1;	//mem_loc�� ���Ӱ� ����
		if(end > 0xFFFFF){
			//end���� �޸� ���� �Ѿ�� 0xFFFFF���� ����ϰ� mem_loc�� 0���� �����Ѵ�.
			end = 0xFFFFF;
			mem_loc = 0;
		}
	}
	else{
		//end���� ����̸� dump start,end���·� �Է¹��� ���̴�.
		end = _end;
		mem_loc = end+1;	//mem_loc�� ���Ӱ� ����
		if(mem_loc > 0xFFFFF)
			//mem_loc�� 0xFFFFF���� Ŀ���� 0���� �����Ѵ�.
			mem_loc = 0;
	}
	//start, end�� �־��� ��� format�� ���� �ٿ� ������
	if(start/16 == end/16){	
		//�޸� �ּ� ���
		printf("%05X ",(start/16)*16);
		//start �տ� ���� ���
		for(i=(start/16)*16; i<start; i++)
			printf("   ");
		//start���� end���� �޸𸮿� �ִ� �� ���
		for(i=start; i<=end; i++)
			printf("%02X ", memory[i]);
		//end���� �ش� �� ������ ���� ���
		for(i=end+1; i%16!=0; i++)
			printf("   ");
		printf("; ");
		//������ �κ��� '.'�� ���
		for(i=(start/16)*16; i<start; i++)
			printf(".");
		//start���� end���� �޸𸮿� �ִ� ���� 20 ~ 7E�̸� �ش��ϴ� character�� ����ϰ� �ƴϸ� '.'�� ����Ѵ�.
		for(i=start; i<=end; i++){
			if(memory[i] >= 32 && memory[i] <= 126)
				printf("%c", memory[i]);
			else
				printf(".");
		}
		//������ �κ��� '.'�� ���
		for(i=end+1; i%16!=0; i++)
			printf(".");
		printf("\n");
		return;
	}
	//start�� end�� �־��� ��� format�� �ٸ� �ٿ� ���� ��
	//ù°�� ���
	if(start%16 != 0){
		printf("%05X ",(start/16)*16);
		for(i=(start/16)*16; i<start; i++)	
			printf("   ");
		for(i=start; i%16 != 0; i++)
			printf("%02X ", memory[i]);
		printf("; ");
		for(i=(start/16)*16; i<start; i++) 
			printf(".");
		for(i=start; i%16 != 0; i++){
			if(memory[i] >= 32 && memory[i] <= 126)
				printf("%c", memory[i]);
			else
				printf(".");
		}
		printf("\n");
		start = i;
	}
	//�������� �������� ���
	for(i=start; i/16 != end/16; i+=16){
		printf("%05X ",(i/16)*16);
		for(j=i; j<i+16; j++)
			printf("%02X ", memory[j]);
		printf("; ");
		for(j=i; j<i+16; j++)
			if(memory[j] >= 32 && memory[j] <= 126)
				printf("%c", memory[j]);
			else
				printf(".");
		printf("\n");
	}
	//�������� ���
	printf("%05X ",(end/16)*16);
	for(i=(end/16)*16; i<=end; i++)	
		printf("%02X ", memory[i]);
	for(i=end+1; i%16 != 0; i++)
		printf("   ");
	printf("; ");
	for(i=(end/16)*16; i<=end; i++){
		if(memory[i] >= 32 && memory[i] <= 126)
			printf("%c", memory[i]);
		else
			printf(".");
	}
	for(i=end+1; i%16 != 0; i++)
		printf(".");
	printf("\n");
}
void printDir(){
	//���� ���丮�� ���ϸ���� ����Ѵ�.
	DIR *dirp;	//������ ����Ű�� ������
	struct dirent *direntp;	//�����Ͱ� ����Ű�� ������ ������ ��� ����ü.
	dirp = opendir(".");	//���� ���丮
	if(!dirp)	return;
	struct stat statbuf;
	while(1){
		direntp = readdir(dirp);
		if(!direntp)	break;
		stat(direntp->d_name, &statbuf);
		//������ ���丮�� ��� /�� ���δ�.
		if(S_ISDIR(statbuf.st_mode)){
			//.�� ..�� ������� �ʴ´�.
			if(strncmp(".",direntp->d_name,1) == 0 || strncmp("..",direntp->d_name,2) == 0)
				continue;
			printf("%s/\t", direntp->d_name);
		}
		//������ ���������� ��� *�� ���δ�.
		else if(stat(direntp->d_name,&statbuf) == 0 && statbuf.st_mode & S_IXUSR)
			printf("%s*\t",direntp->d_name);
		//������ ���ϵ��� ����Ѵ�.
		else{
			printf("%s\t",direntp->d_name);
		}
	}
	printf("\n");
	closedir(dirp);
}
void getInput(char *input){
	char *tok1, *tok2, *tok3;	//�Է¹��� string�� tokenize�� �� ���ڿ��� �����ϱ� ���� ���
	char temp[100];	//�Է¹��� input�� ���⿡ �����صд�.
	
	tok1 = NULL;	tok2 = NULL;	tok3 = NULL;
	//����ڷκ��� �Է��� �޾Ƽ� input�� ����
	fgets(input,100,stdin);
	if(strcmp(input,"\n") == 0)	return ;	//���͸� ������ ����������.
	input[strlen(input)-1] = '\0';	//end of string�� ����
	strcpy(temp,input);	//input�� temp�� ����
	tok1 = strtok(input, " ");	//������ �������� tokenize�� ù ���ڿ��� tok1�� ����
	//tok1�� �������� ��ɾ �����Ѵ�.
	if(strcmp(tok1,"h") == 0 || strcmp(tok1,"help") == 0){
		//h[elp] �ڿ� �ٸ� string�� �����ϸ� printWrong ȣ���ϰ� return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//h[elp]�� �Է¹����� history�� �߰��ϰ� printHelp ȣ��
		addToHistory(temp);
		printHelp();
	}
	else if(strcmp(tok1,"d") == 0 || strcmp(tok1,"dir") == 0){
		//d[ir] �ڿ� �ٸ� string�� �����ϸ� printWrong ȣ���ϰ� return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//d[ir]�� �Է¹����� history�� �߰��ϰ� printDir ȣ��
		addToHistory(temp);
		printDir();
	}
	else if(strcmp(tok1,"q") == 0 || strcmp(tok1,"quit") == 0){
		////q[uit] �ڿ� �ٸ� string�� �����ϸ� printWrong ȣ���ϰ� return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//q[uit]�� �Է¹����� ���α׷� ����
		exit(0);
	} 
	else if(strcmp(tok1,"hi") == 0 || strcmp(tok1,"history") == 0){
		//hi[story] �ڿ� �ٸ� string�� �����ϸ� printWrong ȣ���ϰ� return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//hi[story]�� �Է¹����� history�� �߰��ϰ� printHistory ȣ��
		addToHistory(temp);
		printHistory();
	}
	else if(strcmp(tok1,"du") == 0 || strcmp(tok1,"dump") == 0){
		tok2 = strtok(NULL, " ");
		//du[mp]�� �Է¹��� ��� printDump�� start�� end�� ������ �����ؼ� �ѱ��.
		if(tok2 == NULL){
			printDump(-1,-1);
			addToHistory(temp);
			return;
		}
		else{
			int start, end;
			char c;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c",tok2,&start,&c,&end,&t);
			//sscanf�� ���ϰ��� 1 �Ǵ� 3�� ���� t�� ���� NULL�� �ƴϸ� ��ȿ�� ��ɾ �ƴϹǷ� printWrong ȣ���ϰ� ����
			if(tok_num == 1 || tok_num == 3 || t != ' '){
				printWrong();
				return;
			}
			else{
				//sscanf�� ���ϰ��� 2�̸� dump start�� ����̴�.
				if(tok_num == 2){
					//start�� ���� 0xFFFFF�̻��̸� printWrong ȣ���ϰ� ����, �ƴϸ� printDump�� start���� �Ѱ��ְ� end�� -1�� �ѱ��.
					if(start > 0xFFFFF){
						printWrong();
						return;
					}
					printDump(start,-1);
					addToHistory(temp);
					return;
				}
				else{
					//sscanf�� ���ϰ��� 4�̸� dump start,end�� ����̴�.
					//start�� end������ ���ڰ� ','�� �ƴϰų�, start�� end���� ũ�ų�, start���� 0xFFFFF�̻��̸� printWrongȣ���ϰ� return
					//�ƴϸ� printDump�� start�� end���� �ѱ��.
					if(c != ',' || start > end || start > 0xFFFFF){
						printWrong();
						return;
					}
					else{
						printDump(start,end);
						addToHistory(temp);
						return;
					}
				}
			}
		}
	}
	else if(strcmp(tok1,"e") == 0 || strcmp(tok1,"edit") == 0){
		tok2 = strtok(NULL, " ");
		//e[dit]�� �Է¹����� printWrong ȣ���ϰ� ����
		if(tok2 == NULL){
			printWrong();
			return;
		}
		else{
			int loc, val;
			char c;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c",tok2,&loc,&c,&val,&t);
			//tok_num�� 4�� �ƴϸ� edit loc,val�� ���°� �ƴϹǷ� printWrong ȣ���ϰ� ����
			if(tok_num != 4){
				printWrong();
				return;
			}
			else{
				//loc�� val ������ ���ڰ� ','�� �ƴϰų�, loc�� 0xFFFFF���� ũ�ų�, val�� 0xFF���� ũ�ų� �����̸� �ùٸ� �Է��� �ƴϹǷ� printWrong ȣ���ϰ� ����
				if(c != ',' ||  loc > 0xFFFFF || val > 0xFF || val < 0){
						printWrong();
						return;
				}
				//�ùٸ� �Է��̸� memory�� �ּҰ� loc�� �κ��� val�� �����Ѵ�.
				else{
					memory[loc] = val;
					addToHistory(temp);
					return;
				}
			}
		}
	}
	else if(strcmp(tok1,"f") == 0 || strcmp(tok1,"fill") == 0){
		//f[ill]�� �Է¹������ printWrong ȣ���ϰ� ����
		tok2 = strtok(NULL, " ");
		if(tok2 == NULL){
			printWrong();
			return;
		}
		else{
			int start, end, value;
			char c1, c2;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c %x %c",tok2,&start,&c1,&end, &c2, &value, &t);
			//tok_num�� 6�� �ƴϸ� fill start,end,value�� ���°� �ƴϹǷ� printWrong ȣ���ϰ� ����
			if(tok_num != 6){
				printWrong();
				return;
			}
			else{
				//start�� end�� value ������ ���ڰ� ','�� �ƴϰų�, start�� 0xFFFFF�� �ƴϰų�, start�� end���� ũ�ų�, value���� 0xFF���� ũ�ų� �����̸�
				//�ùٸ� �Է��� �ƴϹǷ� printfWrong ȣ���ϰ� ����
				if(c1 != ',' || c2 != ',' || start > 0xFFFFF || start > end || value > 0xFF || value < 0){
					printWrong();
					return;
				}
				else{
					//�ùٸ� �Է��̸� �޸��� start���� end������ value�� �����Ѵ�.
					int i;
					for(i=start; i<=end; i++)
						memory[i] = value;
					addToHistory(temp);
				}
			}
		}
	}
	else if(strcmp(tok1,"reset") == 0){
		//reset�ڿ� �ٸ� string�� ���� printWrong ȣ���ϰ� ����
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//�ùٸ� �Է��̸� memory ��ü�� 0���� ����
		int i;
		for(i=0;i<1048576;i++)	
			memory[i] = 0;
		addToHistory(temp);
		return;
	}
	else if(strcmp(tok1,"opcode") == 0){
		tok2 = strtok(NULL, " ");
		tok3 = strtok(NULL, " ");
		//opcode mnemonic �ڿ� �ٸ� string�� ���� printWrong ȣ���ϰ� ����
		if(tok3 != NULL){
			printWrong();
			return;
		}
		//tok2�� ù���ڸ� 20���� �������� key�� �ؼ� hash table���� Ž���Ѵ�.
		int key = tok2[0] % 20;
		op_node *tmp = op_header[key];	//Ž���� ���� ������
		//header�� null�̸� printWrong ȣ���ϰ� ����
		if(!tmp){
			printWrong();
			return;
		}
		else{
			while(tmp){
				//��ɾ ã���� opcode�� ����ϰ� �����Ѵ�. 
				if(strcmp(tmp->mnemo,tok2) == 0){
					printf("opcode is %X\n", tmp->opcode);
					addToHistory(temp);
					return;
				}
				tmp = tmp->link;
			}
			//hash table���� ��ɾ ��ã���� printWrong ȣ���ϰ� ����
			printWrong();
			return;
		}
	}
	else if(strcmp(tok1,"opcodelist") == 0){
		//opcodelist �ڿ� �ٸ� string�� ���� printWrong ȣ���ϰ� ����
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//�ƴϸ� printOplist ȣ��
		addToHistory(temp);
		printOplist();
	}
	//tok1��ü�� ��ȿ���� �ʴ� ��� printWrong ȣ���ϰ� ����
	else{
		printWrong();
		return ;
	}
	return;
}
