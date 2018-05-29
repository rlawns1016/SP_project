#include "20131540.h" 

int mem_loc = 0;

void printWrong(){
	//print "Wrong Input"
	printf("Wrong Input\n");
}
void printHelp(){
	//help명령어를 입력받았을 때 사용 가능한 명령어를 출력한다.
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}
void printHistory(){
	//linked list형태로 저장된 명령어 history를 출력한다.
	his_node *temp = history_header;	//탐색을 위한 포인터
	int cnt = 0;
	while(temp != NULL){
		printf("%d\t%s\n",++cnt,temp->str);
		temp = temp->link;
	}
}
void addToHistory(char *input){
	//적절한 명령어(input)를 입력받으면 history 목록에 추가하는 함수이다.
	his_node *newNode;
	his_node *temp = history_header;	//탐색을 위한 포인터
	//새로운 node를 생성해서 초기화한다.
	newNode = (his_node*)malloc(sizeof(his_node));
	newNode->str = (char*)malloc(sizeof(char)*(strlen(input)+1));
	strcpy(newNode->str,input);
	newNode->link = NULL;
	//header가 null이면 새로운 node를 가르키게 하고 아니면 list 끝에 추가한다.
	if(history_header == NULL)
		history_header = newNode;
	else{
		while(temp->link != NULL)	
			temp = temp->link;
		temp->link = newNode;
	}
}
void readOpcode(FILE *fp){
	//fp의 파일에서 opcode정보를 읽어 linked list의 형태로 hash table를 구성한다. 
	int opcode;
	char mnemo[7];
	char format[4];
	int hash;
	op_node *temp;	//탐색을 위한 포인터
	//더이상 파일에서 읽을 수 없을 때 까지 while loop를 돌린다.
	while(fscanf(fp,"%x %s %s", &opcode, mnemo, format)==3){
		//읽은 정보에 따라 새로운 node 초기화
		op_node *newNode;
		newNode = (op_node*)malloc(sizeof(op_node));
		newNode->opcode = opcode;
		newNode->link = NULL;
		strcpy(newNode->mnemo,mnemo);
		if(strcmp(format,"1") == 0)	newNode->format = 1;
		else if(strcmp(format,"2") == 0)	newNode->format = 2;
		else	newNode->format = 3;	//format "3/4"를 받으면 일단 format3라고 저장했다.
		//명령어의 알파벳 첫글자를 20으로 나눠서(hash table의 size가 20이기 때문에) 해당하는 header가 가르키는 list에 새로운 node를 추가한다.
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
	//hash table에 저장되어 있는 opcode list를 출력한다.
	int i;
	op_node *temp;	//탐색을 위한 포인터
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
	//dump 관련 명령어를 받았을 때 메모리 정보를 출력한다.
	int start,end,i,j;
	if(_start < 0)	start = mem_loc;	//start가 -1이면 dump만 입력받은 상태이므로 start를 mem_loc으로 설정한다.
	else	start = _start;	//start가 양수면 dump start를 입력받은 것이다.
	if(_end < 0){
		//end값이 음수이면 dump 또는 dump start형태로 입력받은 것이므로 end를 start+159로 한다.(총 160개를 출력하므로)
		end = start + 159;
		mem_loc = end +1;	//mem_loc을 새롭게 설정
		if(end > 0xFFFFF){
			//end값이 메모리 끝을 넘어가면 0xFFFFF까지 출력하고 mem_loc을 0으로 설정한다.
			end = 0xFFFFF;
			mem_loc = 0;
		}
	}
	else{
		//end값이 양수이면 dump start,end형태로 입력받은 것이다.
		end = _end;
		mem_loc = end+1;	//mem_loc을 새롭게 설정
		if(mem_loc > 0xFFFFF)
			//mem_loc이 0xFFFFF보다 커지면 0으로 설정한다.
			mem_loc = 0;
	}
	//start, end가 주어진 출력 format상에 같은 줄에 있을때
	if(start/16 == end/16){	
		//메모리 주소 출력
		printf("%05X ",(start/16)*16);
		//start 앞에 공백 출력
		for(i=(start/16)*16; i<start; i++)
			printf("   ");
		//start부터 end까지 메모리에 있는 값 출력
		for(i=start; i<=end; i++)
			printf("%02X ", memory[i]);
		//end부터 해당 줄 끝까지 공백 출력
		for(i=end+1; i%16!=0; i++)
			printf("   ");
		printf("; ");
		//공백인 부분은 '.'로 출력
		for(i=(start/16)*16; i<start; i++)
			printf(".");
		//start부터 end까지 메모리에 있는 값이 20 ~ 7E이면 해당하는 character를 출력하고 아니면 '.'로 출력한다.
		for(i=start; i<=end; i++){
			if(memory[i] >= 32 && memory[i] <= 126)
				printf("%c", memory[i]);
			else
				printf(".");
		}
		//공백인 부분은 '.'로 출력
		for(i=end+1; i%16!=0; i++)
			printf(".");
		printf("\n");
		return;
	}
	//start와 end가 주어진 출력 format상 다른 줄에 있을 때
	//첫째줄 출력
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
	//마지막줄 이전까지 출력
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
	//마지막줄 출력
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
	//현재 디렉토리의 파일목록을 출력한다.
	DIR *dirp;	//파일을 가르키는 포인터
	struct dirent *direntp;	//포인터가 가르키는 파일의 정보를 담는 구조체.
	dirp = opendir(".");	//현재 디렉토리
	if(!dirp)	return;
	struct stat statbuf;
	while(1){
		direntp = readdir(dirp);
		if(!direntp)	break;
		stat(direntp->d_name, &statbuf);
		//파일이 디렉토리일 경우 /를 붙인다.
		if(S_ISDIR(statbuf.st_mode)){
			//.과 ..은 출력하지 않는다.
			if(strncmp(".",direntp->d_name,1) == 0 || strncmp("..",direntp->d_name,2) == 0)
				continue;
			printf("%s/\t", direntp->d_name);
		}
		//파일이 실행파일일 경우 *를 붙인다.
		else if(stat(direntp->d_name,&statbuf) == 0 && statbuf.st_mode & S_IXUSR)
			printf("%s*\t",direntp->d_name);
		//나머지 파일들을 출력한다.
		else{
			printf("%s\t",direntp->d_name);
		}
	}
	printf("\n");
	closedir(dirp);
}
void getInput(char *input){
	char *tok1, *tok2, *tok3;	//입력받은 string을 tokenize할 때 문자열을 저장하기 위해 사용
	char temp[100];	//입력받을 input을 여기에 복사해둔다.
	
	tok1 = NULL;	tok2 = NULL;	tok3 = NULL;
	//사용자로부터 입력을 받아서 input에 저장
	fgets(input,100,stdin);
	if(strcmp(input,"\n") == 0)	return ;	//엔터만 받으면 빠져나간다.
	input[strlen(input)-1] = '\0';	//end of string값 저장
	strcpy(temp,input);	//input를 temp에 복사
	tok1 = strtok(input, " ");	//공백을 기준으로 tokenize된 첫 문자열을 tok1에 저장
	//tok1을 기준으로 명령어를 구분한다.
	if(strcmp(tok1,"h") == 0 || strcmp(tok1,"help") == 0){
		//h[elp] 뒤에 다른 string이 존재하면 printWrong 호출하고 return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//h[elp]만 입력받으면 history에 추가하고 printHelp 호출
		addToHistory(temp);
		printHelp();
	}
	else if(strcmp(tok1,"d") == 0 || strcmp(tok1,"dir") == 0){
		//d[ir] 뒤에 다른 string이 존재하면 printWrong 호출하고 return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//d[ir]만 입력받으면 history에 추가하고 printDir 호출
		addToHistory(temp);
		printDir();
	}
	else if(strcmp(tok1,"q") == 0 || strcmp(tok1,"quit") == 0){
		////q[uit] 뒤에 다른 string이 존재하면 printWrong 호출하고 return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//q[uit]만 입력받으면 프로그램 종료
		exit(0);
	} 
	else if(strcmp(tok1,"hi") == 0 || strcmp(tok1,"history") == 0){
		//hi[story] 뒤에 다른 string이 존재하면 printWrong 호출하고 return
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//hi[story]만 입력받으면 history에 추가하고 printHistory 호출
		addToHistory(temp);
		printHistory();
	}
	else if(strcmp(tok1,"du") == 0 || strcmp(tok1,"dump") == 0){
		tok2 = strtok(NULL, " ");
		//du[mp]만 입력받은 경우 printDump의 start와 end를 음수로 설정해서 넘긴다.
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
			//sscanf의 리턴값이 1 또는 3인 경우와 t의 값이 NULL이 아니면 유효한 명령어가 아니므로 printWrong 호출하고 리턴
			if(tok_num == 1 || tok_num == 3 || t != ' '){
				printWrong();
				return;
			}
			else{
				//sscanf의 리턴값이 2이면 dump start의 경우이다.
				if(tok_num == 2){
					//start의 값이 0xFFFFF이상이면 printWrong 호출하고 리턴, 아니면 printDump에 start값을 넘겨주고 end는 -1로 넘긴다.
					if(start > 0xFFFFF){
						printWrong();
						return;
					}
					printDump(start,-1);
					addToHistory(temp);
					return;
				}
				else{
					//sscanf의 리턴값이 4이면 dump start,end의 경우이다.
					//start와 end사이의 문자가 ','가 아니거나, start가 end보다 크거나, start값이 0xFFFFF이상이면 printWrong호출하고 return
					//아니면 printDump에 start와 end값을 넘긴다.
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
		//e[dit]만 입력받으면 printWrong 호출하고 리턴
		if(tok2 == NULL){
			printWrong();
			return;
		}
		else{
			int loc, val;
			char c;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c",tok2,&loc,&c,&val,&t);
			//tok_num이 4가 아니면 edit loc,val의 형태가 아니므로 printWrong 호출하고 리턴
			if(tok_num != 4){
				printWrong();
				return;
			}
			else{
				//loc와 val 사이의 문자가 ','가 아니거나, loc가 0xFFFFF보다 크거나, val이 0xFF보다 크거나 음수이면 올바른 입력이 아니므로 printWrong 호출하고 리턴
				if(c != ',' ||  loc > 0xFFFFF || val > 0xFF || val < 0){
						printWrong();
						return;
				}
				//올바른 입력이면 memory의 주소가 loc인 부분을 val로 설정한다.
				else{
					memory[loc] = val;
					addToHistory(temp);
					return;
				}
			}
		}
	}
	else if(strcmp(tok1,"f") == 0 || strcmp(tok1,"fill") == 0){
		//f[ill]만 입력받은경우 printWrong 호출하고 리턴
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
			//tok_num이 6이 아니면 fill start,end,value의 형태가 아니므로 printWrong 호출하고 리턴
			if(tok_num != 6){
				printWrong();
				return;
			}
			else{
				//start와 end와 value 사이의 문자가 ','가 아니거나, start가 0xFFFFF가 아니거나, start가 end보다 크거나, value값이 0xFF보다 크거나 음수이면
				//올바른 입력이 아니므로 printfWrong 호출하고 리턴
				if(c1 != ',' || c2 != ',' || start > 0xFFFFF || start > end || value > 0xFF || value < 0){
					printWrong();
					return;
				}
				else{
					//올바른 입력이면 메모리의 start부터 end까지를 value로 설정한다.
					int i;
					for(i=start; i<=end; i++)
						memory[i] = value;
					addToHistory(temp);
				}
			}
		}
	}
	else if(strcmp(tok1,"reset") == 0){
		//reset뒤에 다른 string이 오면 printWrong 호출하고 리턴
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//올바른 입력이면 memory 전체를 0으로 설정
		int i;
		for(i=0;i<1048576;i++)	
			memory[i] = 0;
		addToHistory(temp);
		return;
	}
	else if(strcmp(tok1,"opcode") == 0){
		tok2 = strtok(NULL, " ");
		tok3 = strtok(NULL, " ");
		//opcode mnemonic 뒤에 다른 string이 오면 printWrong 호출하고 리턴
		if(tok3 != NULL){
			printWrong();
			return;
		}
		//tok2의 첫글자를 20으로 나눈값을 key로 해서 hash table에서 탐색한다.
		int key = tok2[0] % 20;
		op_node *tmp = op_header[key];	//탐색을 위한 포인터
		//header가 null이면 printWrong 호출하고 리턴
		if(!tmp){
			printWrong();
			return;
		}
		else{
			while(tmp){
				//명령어를 찾으면 opcode를 출력하고 리턴한다. 
				if(strcmp(tmp->mnemo,tok2) == 0){
					printf("opcode is %X\n", tmp->opcode);
					addToHistory(temp);
					return;
				}
				tmp = tmp->link;
			}
			//hash table에서 명령어를 못찾으면 printWrong 호출하고 리턴
			printWrong();
			return;
		}
	}
	else if(strcmp(tok1,"opcodelist") == 0){
		//opcodelist 뒤에 다른 string이 오면 printWrong 호출하고 리턴
		tok2 = strtok(NULL, " ");
		if(tok2 != NULL){
			printWrong();
			return;
		}
		//아니면 printOplist 호출
		addToHistory(temp);
		printOplist();
	}
	//tok1자체가 유효하지 않는 경우 printWrong 호출하고 리턴
	else{
		printWrong();
		return ;
	}
	return;
}
