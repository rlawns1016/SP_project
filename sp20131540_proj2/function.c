#include "20131540.h" 
#define MAX_LENGTH 100
#define BUFFER_SIZE 512
#define TOKEN_SIZE 32
#define ASSEM_SIZE 100
#define _OPCODE_ 65536
#define _N_  131072
#define _I_ 65536
#define _X_ 32768
#define _B_ 16384
#define _P_	8192
#define _E_	4096
int mem_loc = 0;

/*------------------------------------------------------------------------------------
  Name: printWrong
  Object: 에러 메세지를 출력한다.
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printWrong(){
	printf("Wrong Input\n");
}
/*------------------------------------------------------------------------------------
  Name: printHelp
  Object: 사용 가능한 명령어들을 출력한다.
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printHelp(){
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
}
/*------------------------------------------------------------------------------------
  Name: printHistory
  Object: Linked list형태로 저장된 명령어 history를 출력한다
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printHistory(){
	his_node *temp = history_header;	
	int cnt = 0;
	while(temp != NULL){
		printf("%d\t%s\n",++cnt,temp->str);
		temp = temp->link;
	}
}
/*------------------------------------------------------------------------------------
  Name: addToHistory
  Object: 유효한 명령어(input)를 입력받으면 history list에 추가한다.
  Parameters: char *input : 사용자에게 입력받은 명령어
  Return: None
  ------------------------------------------------------------------------------------*/
void addToHistory(char *input){
	his_node *newNode;
	his_node *temp = history_header;
	newNode = (his_node*)malloc(sizeof(his_node));
	newNode->str = (char*)malloc(sizeof(char)*(strlen(input)+1));
	strcpy(newNode->str,input);
	newNode->link = NULL;
	if(history_header == NULL)
		history_header = newNode;
	else{
		while(temp->link != NULL)	
			temp = temp->link;
		temp->link = newNode;
	}
}
/*------------------------------------------------------------------------------------
  Name: readOpcode
  Object: fp가 가르키는 파일에서 opcode정보를 읽어 linked list 형태로 hash table을 구성한다.
  Parameters: FILE *fp : 파일 포인터로 main함수에서 지정
  Return: None
  ------------------------------------------------------------------------------------*/
void readOpcode(FILE *fp){
	int opcode;
	char mnemo[7];
	char format[4];
	int hash;
	op_node *temp;	
	while(fscanf(fp,"%x %s %s", &opcode, mnemo, format)==3){
		op_node *newNode;
		newNode = (op_node*)malloc(sizeof(op_node));
		newNode->opcode = opcode;
		newNode->link = NULL;
		strcpy(newNode->mnemo,mnemo);
		if(strcmp(format,"1") == 0)	newNode->format = 1;
		else if(strcmp(format,"2") == 0)	newNode->format = 2;
		else	newNode->format = 3;	
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
/*------------------------------------------------------------------------------------
  Name: printOplist
  Object: hash table에 있는 opcode list를 출력한다. hash table의 헤더들은 전역변수이다.
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printOplist(){
	int i;
	op_node *temp;
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
/*------------------------------------------------------------------------------------
  Name: printDump
  Object: dump 관련 명령어를 받았을 때 메모리 정보를 출력하고 전역변수인 mem_loc값을 설정한다.
  Parameters: int _start, int _end : du[mp] start,end 에서 각각 start와 end값이다. _start와 _end가 -1이면 안받은 것으로 간주한다. 
  Return: None
  ------------------------------------------------------------------------------------*/
void printDump(int _start, int _end){
	int start,end,i,j;
	if(_start < 0)	start = mem_loc;
	else	start = _start;
	if(_end < 0){
		end = start + 159;
		mem_loc = end +1;
		if(end > 0xFFFFF){
			end = 0xFFFFF;
			mem_loc = 0;
		}
	}
	else{
		end = _end;
		mem_loc = end+1;
		if(mem_loc > 0xFFFFF)
			mem_loc = 0;
	}
	if(start/16 == end/16){	
		printf("%05X ",(start/16)*16);
		for(i=(start/16)*16; i<start; i++)
			printf("   ");
		for(i=start; i<=end; i++)
			printf("%02X ", memory[i]);
		for(i=end+1; i%16!=0; i++)
			printf("   ");
		printf("; ");
		for(i=(start/16)*16; i<start; i++)
			printf(".");
		for(i=start; i<=end; i++){
			if(memory[i] >= 32 && memory[i] <= 126)
				printf("%c", memory[i]);
			else
				printf(".");
		}
		for(i=end+1; i%16!=0; i++)
			printf(".");
		printf("\n");
		return;
	}
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
/*------------------------------------------------------------------------------------
  Name: printDir
  Object: 현재 디렉토리의 파일목록을 출력한다. 디렉토리면 '/'를 붙이고 실행파일이면 '*'를 붙인다.
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printDir(){
	DIR *dirp;
	struct dirent *direntp;
	struct stat statbuf;
	dirp = opendir(".");
	if(!dirp)	return;
	while(1){
		direntp = readdir(dirp);
		if(!direntp)	break;
		stat(direntp->d_name, &statbuf);
		if(S_ISDIR(statbuf.st_mode)){
			if(strncmp(".",direntp->d_name,1) == 0 || strncmp("..",direntp->d_name,2) == 0)
				continue;
			printf("%s/\t", direntp->d_name);
		}
		else if(stat(direntp->d_name,&statbuf) == 0 && statbuf.st_mode & S_IXUSR)
			printf("%s*\t",direntp->d_name);
		else{
			printf("%s\t",direntp->d_name);
		}
	}
	printf("\n");
	closedir(dirp);
}
/*------------------------------------------------------------------------------------
  Name: printFile
  Object: 파일에 있는 내용을 출력한다
  Parameters: FILE *fp : 출력 할 파일을 가르키는 포인터
  Return: None
  ------------------------------------------------------------------------------------*/
void printFile(FILE *fp){
	char str[BUFFER_SIZE];
	while(fgets(str,BUFFER_SIZE,fp) != NULL)
		printf("%s",str);
	printf("\n");
	return;
}
/*------------------------------------------------------------------------------------
  Name: isSingleMnemo
  Object: Mnemo가 혼자서 쓰이는 명령어인지 판단하는 함수이다
  Parameters: char *mnemo : 판단할 명령어
  Return: 아닐경우 0, 맞으면 1 리턴
  ------------------------------------------------------------------------------------*/
short int isSingleMnemo(char *mnemo){
	if(strcmp(mnemo,"FIX")==0 || strcmp(mnemo,"FLOAT")==0 || strcmp(mnemo,"HIO")==0 || strcmp(mnemo,"RSUB")==0 || strcmp(mnemo,"SIO")==0 || strcmp(mnemo,"TIO")==0 ||  strcmp(mnemo,"NORM")==0)
		return 1;
	return 0;
}
/*------------------------------------------------------------------------------------
  Name: getRegNum
  Object: 레지스터의 번호를 알려주는 함수이다.
  Parameters: char *mnemo : 판단할 레지스터
  Return: 각 레지스터의 번호, 레지스터가 아니면 -1 리턴
  ------------------------------------------------------------------------------------*/
int getRegNum(char *mnemo){
	if(strlen(mnemo) == 1)
		if((*mnemo) == 'A')
			return 0;
		else if((*mnemo) == 'X')
			return 1;
		else if((*mnemo) == 'L')
			return 2;
		else if((*mnemo) == 'B')
			return 3;
		else if((*mnemo) == 'S')
			return 4;
		else if((*mnemo) == 'T')
			return 5;
		else if((*mnemo) == 'F')
			return 6;
		else return -1;
	else
		if(strcmp(mnemo,"PC") == 0)
			return 8;
		else if(strcmp(mnemo,"SW") == 0)
			return 9;
		else return -1;
	return -1;
}
/*------------------------------------------------------------------------------------
  Name: printSymbol
  Object: symbol table에 있는 symbol들과 symbol의 address를 출력한다.
  Parameters: None
  Return: None
  ------------------------------------------------------------------------------------*/
void printSymbol(){
	sym_node *i, *j;
	int t;
	char temp[TOKEN_SIZE];
	i = sym_header;
	if(i != NULL){
		if(i->link != NULL){
			for(; i->link != NULL; i = i->link){
				for(j = i->link; j != NULL; j = j->link){
					if(strcmp(j->label,i->label) > 0){
						t = i->locctr;
						strcpy(temp,i->label);
						i->locctr = j->locctr;
						strcpy(i->label,j->label);
						j->locctr = t;
						strcpy(j->label,temp);
					}
				}
			}
		}
	}
	i = sym_header;
	while(i!=NULL){
		printf("\t%s\t%04X\n",i->label,i->locctr);
		i = i->link;
	}	
}
/*------------------------------------------------------------------------------------
  Name: getLoc
  Object: symbol의 address를 알려주는 함수이다.
  Parameters: char *label : symbol에 해당한다.
  Return: symbol의 address값
  ------------------------------------------------------------------------------------*/
int getLoc(char *label){
	sym_node *temp;
	temp = sym_header;
	while(temp){
		if(strcmp(label,temp->label) == 0)
			return temp->locctr;
		temp = temp->link;
	}
	return 0;
}
/*------------------------------------------------------------------------------------
  Name: getOpcode
  Object: mnemo의 opcode를 알기위한 함수이다.
  Parameters: char *mnemo : mnemo에 해당한다.
  Return: mnemo의 opcode
  ------------------------------------------------------------------------------------*/
int getOpcode(char *mnemo){
	int key;
	op_node *temp;
	
	key = mnemo[0] % 20;
		temp = op_header[key];
		if(!temp){
			return -1;
		}
		else{
			while(temp){
				if(strcmp(temp->mnemo,mnemo) == 0){
					return temp->opcode;
				}
				temp = temp->link;
			}
		}
		return -1;
}
/*------------------------------------------------------------------------------------
  Name: getFormat
  Object: mnemo의 format을 알기위한 함수이다. format4는 format3로 저장되어있다.
  Parameters: char *mnemo : mnemo에 해당한다.
  Return: mnemo의 format
  ------------------------------------------------------------------------------------*/
int getFormat(char *mnemo){
	int key;
	op_node *temp;
	
	key = mnemo[0] % 20;
		temp = op_header[key];
		if(!temp){
			return -1;
		}
		else{
			while(temp){
				if(strcmp(temp->mnemo,mnemo) == 0){
					return temp->format;
				}
				temp = temp->link;
			}
		}
		return -1;
}
/*------------------------------------------------------------------------------------
  Name: setStr
  Object: BUFFER , X 와 같은 문자열들을 BUFFER, X꼴로 통일시킨다.
  Parameters: char *str : 변경시킬 문자열
  Return: NONE
  ------------------------------------------------------------------------------------*/
void setStr(char *str){
	unsigned int i,j;
	for(i=0; i<strlen(str); i++){
		if(str[i] == ','){
			char tok1[ASSEM_SIZE/2], tok2[ASSEM_SIZE/2];
			sscanf(str,"%[^,],%s",tok1,tok2);
			for(j=strlen(tok1)-1;j>0;j--){
				if(tok1[j] == ' ' || tok1[j] == '\t')
					tok1[j] = '\0';
				else
					break;
			}
			strcat(tok1,", ");
			strcat(tok1,tok2);
			strcpy(str,tok1);
			strcat(str,"\n");
			return;
		}
	}
	return;
}
/*------------------------------------------------------------------------------------
  Name: pass2
  Object: 강의자료의 pass2에 해당한다.
  Parameters: FILE *fp : pass1에서 생성한 파일, char *fileName : 2_5.asm에서 2_5부분에 해당, int start_loc : start location counter
				int end_loc : 맨마지막 location counter, int _base_loc : BASE 레지스터의 location counter
  Return: 에러인 Line number, 에러가 없으면 0 리턴
  ------------------------------------------------------------------------------------*/
int pass2(FILE *fp, char *fileName, int start_loc, int end_loc, int _base_loc){
	FILE *ob_fp;
	FILE *ob_fp2;
	FILE *lst_fp;
	FILE *pc_fp;
	char *fileName2;
	char *stop = NULL;
	char str[ASSEM_SIZE];
	char str2[ASSEM_SIZE];
	char tok1[TOKEN_SIZE], tok2[TOKEN_SIZE], tok3[TOKEN_SIZE], tok4[TOKEN_SIZE], tok5[TOKEN_SIZE], tok6[TOKEN_SIZE];
	int line=0, tok_num, loc_ctr, ob_code=0, opcode=0, pc_loc=0, base_loc = _base_loc, ob_cnt =0, M_cnt=0, M_i = 0;
	int M[TOKEN_SIZE],i;
	short int _n=0,_i=0,_x=0,_b=0,_p=0,_e=0,pc=0;
	short int newLine=0;

	for(i=0;i<TOKEN_SIZE;i++)	M[i] = -1;
	pc_fp = fopen(fileName,"r");
	fileName2 = (char*)malloc(sizeof(char)*(strlen(fileName)+1));
	strcpy(fileName2,fileName);
	ob_fp = fopen(strcat(fileName,".objtmp"),"wt");
	strcpy(fileName,fileName2);
	lst_fp = fopen(strcat(fileName2,".lst"),"wt");
	ob_fp2 = fopen(strcat(fileName,".obj"),"wt");
	if(!pc_fp || !ob_fp || !lst_fp || !ob_fp2)
		return -1;
	
	fgets(str,ASSEM_SIZE,fp);
	fgets(str2,ASSEM_SIZE,pc_fp);
	tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
	do{
		fgets(str2,ASSEM_SIZE,pc_fp);
		pc = sscanf(str2,"%d %*s",&pc_loc);
	}while(pc==0 || pc_loc == 0);	
	line+=5;

	if(strcmp(tok2,"START") == 0){
		fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\n",line,start_loc,tok1,tok2);
		fprintf(ob_fp,"H%-6s%06X%06X\n",tok1,start_loc,end_loc);
		fgets(str,ASSEM_SIZE,fp);
		tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
		line+=5;
	}
	else if(strcmp(tok1,"START") == 0){
		fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\n",line,start_loc," ",tok1);
		fprintf(ob_fp,"H%-6s%06X%06X\n","NONE",start_loc,end_loc);
		fgets(str,ASSEM_SIZE,fp);
		tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
		line+=5;
	}
	else{
		fprintf(ob_fp,"H%-6s%06X%06X\n","NONE",start_loc,end_loc);
	}
	fprintf(ob_fp,"T%06X",start_loc);
	
	while(strcmp(tok1,"END") != 0){
		if((*tok1) != '.'){
			tok5[0] = '\0';
			tok6[0] = '\0';
			_n=0;_i=0;_x=0;_b=0;_p=0;_e=0;
			opcode = 0; ob_code = 0;
			if(tok_num == 2){
				if(strcmp(tok1,"BASE") == 0){
					fprintf(lst_fp,"%d\t\t%4s\t%-7s\t%-5s\t%s\n",line," "," ",tok1,tok2);
					line+=5;
					fgets(str,ASSEM_SIZE,fp);
					tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
					if(tok_num == 0)
						tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
					continue;	
				}
				else{
					strcpy(tok5,tok1);
					tok1[0] = '\0';
				}
			}
			else if(tok_num == 3){
				if(isSingleMnemo(tok2) == 1){
					strcpy(tok5,tok2);	
				}
				else{
					strcpy(tok5,tok1);
					strcpy(tok6,tok2);
					tok1[0] = '\0';
				}
			}
			else if(tok_num == 4){
				if(tok2[strlen(tok2)-1] != ','){
					strcpy(tok5,tok2);
					strcpy(tok6,tok3);
				}
				else{
					strcpy(tok5,tok1);
					strcpy(tok6,tok2);
					_x = 1;
					tok1[0] = '\0';
				}
			}
			else{
				strcpy(tok5,tok2);
				strcpy(tok6,tok3);
				_x = 1;
			}
			if(tok5[0] == '+'){
				unsigned int i;
				_e = 1;
				for(i=0; i < strlen(tok5); i++)
					tok5[i] = tok5[i+1];
			}
			opcode = getOpcode(tok5);
			if(opcode >= 0){
				if(_e == 1)
					ob_code = opcode*_OPCODE_*16*16;
				else
					ob_code = opcode*_OPCODE_;
				if(isSingleMnemo(tok5) == 1){ //single mnemo
					int fmat;
					_n = 1;
					_i = 1;
					if(_e == 0){
						ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
						if(tok1[0] != '\0')
							fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%06X\n",line,loc_ctr,tok1,tok5," ",ob_code);
						else
							fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%06X\n",line,loc_ctr," ",tok5," ",ob_code);
					}
					else{
						ob_code +=16*16*(_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_);
						if(tok1[0] != '\0')
							fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%-15s\t\t%06X\n",line,loc_ctr,tok1,tok5," ",ob_code);
						else
							fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%-15s\t\t%06X\n",line,loc_ctr," ",tok5," ",ob_code);
					}
					fmat = getFormat(tok5);
					if(_e == 1)	fmat++;
					ob_cnt += fmat;
					M_cnt += ob_cnt;
					if(ob_cnt > 0x1D || newLine == 1){
						fprintf(ob_fp,"\nT%06X",loc_ctr);
						newLine = 0;
						ob_cnt = fmat;
					}
					if(fmat == 1)
						fprintf(ob_fp,"%02X",ob_code);
					else if(fmat == 2)
						fprintf(ob_fp,"%04X",ob_code);
					else if(fmat == 3)
						fprintf(ob_fp,"%06X",ob_code);
					else
						fprintf(ob_fp,"%08X",ob_code);					
					line+=5;
					fgets(str,ASSEM_SIZE,fp);
					tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
					if(tok_num == 0)
						tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
					do{
						fgets(str2,ASSEM_SIZE,pc_fp);
						pc = sscanf(str2,"%d %*s",&pc_loc);
					}while(pc==0);
					continue;
				}
				else{
					if(tok6[0] == '#'){
						int temp;
						unsigned int i;
						for(i=0; i < strlen(tok6); i++)
							tok6[i] = tok6[i+1];
						_i = 1;
						temp = (int)strtol(tok6,&stop,10);
						if(temp == 0 && (*tok6) != '0'){ //#symbol
							int sym_loc=0;
							sym_node *temp = sym_header;
							stop = NULL;
							while(temp){
								if(strcmp(tok6,temp->label) == 0){
									sym_loc = temp->locctr;
									break;
								}
								temp = temp->link;
							}
							if(!temp){
								//free memory
								sym_node *temp;
								if(sym_header == NULL){	
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								if(sym_header->link == NULL)
									free(sym_header);
								else{
									temp = sym_header->link;
									while(temp){
										free(sym_header);
										sym_header = temp;
										temp = temp->link;
									}
									free(sym_header);
									sym_header = NULL;
								}
								fclose(pc_fp);
								fclose(ob_fp);
								fclose(lst_fp);
								fclose(ob_fp2);
								remove(fileName);
								remove(fileName2);
								strcat(fileName,"tmp");
								remove(fileName);
								return line/5;
							}
							//symbol 찾음
							//다 format3,4 가정하고, MNEMO #SYM,XX는 없다고 가정함
							if(sym_loc - pc_loc > 2047 || sym_loc - pc_loc < -2048){
								//base relative
								_b = 1;
								if(sym_loc - base_loc < 0)	ob_code+=16*16*16;
								ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
								ob_code += sym_loc - base_loc;
							}
							else{
								//pc relative
								_p = 1;
								if(sym_loc - pc_loc < 0) ob_code+=16*16*16;
								ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
								ob_code += sym_loc - pc_loc;
							}
							if(tok1[0] != '\0')
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t#%-15s\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
							else
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t#%-15s\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							if(_e == 0){
								ob_cnt += 3;
								M_cnt += 3;
							}
							else{
								M[M_i++] = M_cnt+1;
								ob_cnt += 4;
								M_cnt += 4;
							}
							if(ob_cnt > 0x1D || newLine == 1){
								fprintf(ob_fp,"\nT%06X",loc_ctr);
								if(_e == 0){
									ob_cnt = 3;
								}
								else{
									ob_cnt = 4;
								}
								newLine = 0;
							}
							if(_e == 0){
								fprintf(ob_fp,"%06X",ob_code);
							}
							else{
								fprintf(ob_fp,"%08X",ob_code);
							}
							line+=5;
							fgets(str,ASSEM_SIZE,fp);
							do{
								fgets(str2,ASSEM_SIZE,pc_fp);
								pc = sscanf(str2,"%d %*s",&pc_loc);
							}while(pc==0);
							tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
							if(tok_num == 0){
								tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
							}
							continue;
						}//end of #symbol
						else{	//#십진수
							ob_code+=temp;
							if(_e == 0){
							ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
								if(tok1[0] != '\0')
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t#%-15s\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
								else
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t#%-15s\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							}
							else{
								ob_code +=16*16*(_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_);
								if(tok1[0] != '\0')
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t#%-15s\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
								else
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t#%-15s\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							}
							if(_e == 0){
								ob_cnt += 3;
								M_cnt += 3;
							}
							else{
								ob_cnt += 4;
							}
							if(ob_cnt > 0x1D || newLine == 1){
								fprintf(ob_fp,"\nT%06X",loc_ctr);
								if(_e == 0){
									ob_cnt = 3;
								}
								else{
									ob_cnt = 4;
								}
								newLine = 0;
							}
							if(_e == 0){
								fprintf(ob_fp,"%06X",ob_code);
							}
							else{
								fprintf(ob_fp,"%08X",ob_code);
							}line+=5;
							fgets(str,ASSEM_SIZE,fp);
							tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
							if(tok_num == 0)
								tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
							do{
								fgets(str2,ASSEM_SIZE,pc_fp);
								pc = sscanf(str2,"%d %*s",&pc_loc);
							}while(pc==0);
							continue;							
						}
					}
					if(tok6[0] == '@'){
						int sym_loc=0;
						unsigned int i;
						sym_node *temp = sym_header;
						for(i=0; i < strlen(tok6); i++)
							tok6[i] = tok6[i+1];
						_n = 1;
						stop = NULL;
						while(temp){
							if(strcmp(tok6,temp->label) == 0){
								sym_loc = temp->locctr;
								break;
							}
							temp = temp->link;
						}
						if(!temp){
							//free memory
							sym_node *temp;
							if(sym_header == NULL){
								fclose(pc_fp);
								fclose(ob_fp);
								fclose(lst_fp);
								fclose(ob_fp2);
								remove(fileName);
								remove(fileName2);
								strcat(fileName,"tmp");
								remove(fileName);
								return line/5;
							}
							if(sym_header->link == NULL)
								free(sym_header);
							else{
								temp = sym_header->link;
								while(temp){
									free(sym_header);
									sym_header = temp;
									temp = temp->link;
								}
								free(sym_header);
								sym_header = NULL;
							}
							fclose(pc_fp);
							fclose(ob_fp);
							fclose(lst_fp);
							fclose(ob_fp2);
							remove(fileName);
							remove(fileName2);
							strcat(fileName,"tmp");
							remove(fileName);
							return line/5;
						}
						//symbol 찾음
						//다 format3,4라 가정하고, MNEMO @SYM,XX는 없다고 가정함
						if(sym_loc - pc_loc > 2047 || sym_loc - pc_loc < -2048){
							//base relative
							_b = 1;
							if(sym_loc - base_loc < 0)	ob_code+=16*16*16;
							ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
							ob_code += sym_loc - base_loc;
						}
						else{
							//pc relative
							_p = 1;
							if(sym_loc - pc_loc < 0) ob_code+=16*16*16;
							ob_code +=_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
							ob_code += sym_loc - pc_loc;
						}
						if(_e == 0){
							if(tok1[0] != '\0')
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t@%-15s\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
							else
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t@%-15s\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
						}
						else{
							ob_code *=16*16;
							if(tok1[0] != '\0')
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t@%-15s\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
							else
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t@%-15s\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
						}
						if(_e == 0){
							ob_cnt += 3;
							M_cnt += 3;
						}
						else{
							M[M_i++] = M_cnt+1;
							ob_cnt += 4;
							M_cnt += 4;
						}
						if(ob_cnt > 0x1D || newLine == 1){
							fprintf(ob_fp,"\nT%06X",loc_ctr);
							if(_e == 0){
								ob_cnt = 3;
							}
							else{
								ob_cnt = 4;
							}
							newLine = 0;
						}
						if(_e == 0){
							fprintf(ob_fp,"%06X",ob_code);
						}
						else{
							fprintf(ob_fp,"%08X",ob_code);
						}
						line+=5;
						fgets(str,ASSEM_SIZE,fp);
						do{
							fgets(str2,ASSEM_SIZE,pc_fp);
							pc = sscanf(str2,"%d %*s",&pc_loc);
						}while(pc==0);
						tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
						if(tok_num == 0){
							tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
						}
						continue;
					}//end of if @symbol
					if(getFormat(tok5) == 2){ //format2
						ob_code /= 16*16;
						if(tok6[strlen(tok6)-1] == ','){
							int reg1,reg2;
							tok6[strlen(tok6)-1] = '\0';
							reg1 = getRegNum(tok6);
							if(tok1[0] == '\0')
								reg2 = getRegNum(tok3);
							else
								reg2 = getRegNum(tok4);
							if(reg1 < 0 || (reg2 < 0 && strcmp(tok5,"SHIFTL") != 0) || (reg2 < 0 && strcmp(tok5,"SHIFTR") != 0)){
								//free memory
								sym_node *temp;
								if(sym_header == NULL){	
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								if(sym_header->link == NULL)
									free(sym_header);
								else{
									temp = sym_header->link;
									while(temp){
										free(sym_header);
										sym_header = temp;
										temp = temp->link;
									}
									free(sym_header);
									sym_header = NULL;
									}
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
							}
							if(reg2 < 0 && (strcmp(tok5,"SHIFTL") == 0 || strcmp(tok5,"SHIFTR") == 0))
								reg2 = strtol(tok6,&stop,16);
							ob_code+=reg1*16+reg2;
							if(tok1[0] != '\0')
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%s, %-13s\t%04X\n",line,loc_ctr,tok1,tok5,tok6,tok4,ob_code);
							else
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%s, %-13s\t%04X\n",line,loc_ctr," ",tok5,tok6,tok3,ob_code);
							ob_cnt += 2;
							M_cnt += 2;
							if(ob_cnt > 0x1D || newLine == 1){
								fprintf(ob_fp,"\nT%06X",loc_ctr);
								newLine = 0;
								ob_cnt = 2;
							}
							fprintf(ob_fp,"%04X",ob_code);
							line+=5;
							fgets(str,ASSEM_SIZE,fp);
							do{
								fgets(str2,ASSEM_SIZE,pc_fp);
								pc = sscanf(str2,"%d %*s",&pc_loc);
							}while(pc==0);
								tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
							if(tok_num == 0)
								tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
							continue;
						}
						else{
							int reg;
							reg = getRegNum(tok6);
							if(reg < 0){
								if(strcmp(tok5,"SVC") != 0){
									//free memory
									sym_node *temp;
									if(sym_header == NULL){
										fclose(pc_fp);
										fclose(ob_fp);
										fclose(lst_fp);
										fclose(ob_fp2);
										remove(fileName);
										remove(fileName2);
										strcat(fileName,"tmp");
										remove(fileName);
										return line/5;
									}
									if(sym_header->link == NULL)
										free(sym_header);
									else{
										temp = sym_header->link;
										while(temp){
											free(sym_header);
											sym_header = temp;
											temp = temp->link;
										}
										free(sym_header);
										sym_header = NULL;
									}
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								else
									reg = strtol(tok6,&stop,16);
							}
							ob_code += reg*16;
							if(tok1[0] != '\0')
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%04X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
							else
								fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%04X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							ob_cnt += 2;
							M_cnt += 2;
							if(ob_cnt > 0x1D || newLine == 1){
								fprintf(ob_fp,"\nT%06X",loc_ctr);
								newLine = 0;
								ob_cnt = 2;
							}
							fprintf(ob_fp,"%04X",ob_code);line+=5;
							fgets(str,ASSEM_SIZE,fp);
							do{
								fgets(str2,ASSEM_SIZE,pc_fp);
								pc = sscanf(str2,"%d %*s",&pc_loc);
							}while(pc==0);
								tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
							if(tok_num == 0)
								tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
							continue;				
						}
					}
					else{	//format 3/4
						if(tok6[strlen(tok6)-1] != ','){
							sym_node* temp;
							int sym_loc;
							temp = sym_header;
							_i=1; _n=1;
							while(temp){
								if(strcmp(tok6,temp->label) == 0){
									sym_loc = temp->locctr;
									break;
								}
								temp = temp->link;
							}
							if(!temp){
								//free memory
								sym_node *temp;
								if(sym_header == NULL){
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								if(sym_header->link == NULL)
									free(sym_header);
								else{
									temp = sym_header->link;
									while(temp){
										free(sym_header);
										sym_header = temp;
										temp = temp->link;
									}
									free(sym_header);
									sym_header = NULL;
								}
								fclose(pc_fp);
								fclose(ob_fp);
								fclose(lst_fp);
								fclose(ob_fp2);
								remove(fileName);
								remove(fileName2);
								strcat(fileName,"tmp");
								remove(fileName);
								return line/5;
							}
							//symbol 찾음
							if(_e == 0){
								if(sym_loc - pc_loc > 2047 || sym_loc - pc_loc < -2048){
									//base relative
									_b = 1;
									ob_code += sym_loc - base_loc;
									if(sym_loc - base_loc < 0)	ob_code+=16*16*16;
								}
								else{
									//pc relative
									_p = 1;
									ob_code += sym_loc - pc_loc;
									if(sym_loc - pc_loc < 0) ob_code+=16*16*16;
								}
								ob_code += _n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
							}
							else{
								ob_code += 16*16*(_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_);
								ob_code += sym_loc;
							}
							if(_e == 0){
								if(tok1[0] != '\0')
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
								else
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							}
							else{
								if(tok1[0] != '\0')
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%-15s\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
								else
									fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%-15s\t\t%06X\n",line,loc_ctr," ",tok5,tok6,ob_code);
							}
							if(_e == 0){
								ob_cnt += 3;
								M_cnt += 3;
							}
							else{
								M[M_i++] = M_cnt+1;
								ob_cnt += 4;
								M_cnt += 4;
							}
							if(ob_cnt > 0x1D || newLine == 1){
								fprintf(ob_fp,"\nT%06X",loc_ctr);
								if(_e == 0){
									ob_cnt = 3;
								}
								else{
									ob_cnt = 4;
								}
								newLine = 0;
							}
							if(_e == 0){
								fprintf(ob_fp,"%06X",ob_code);
							}
							else{
								fprintf(ob_fp,"%08X",ob_code);
							}
							line+=5;
							fgets(str,ASSEM_SIZE,fp);
							do{
								fgets(str2,ASSEM_SIZE,pc_fp);
								pc = sscanf(str2,"%d %*s",&pc_loc);
							}while(pc==0);
							tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
							if(tok_num == 0){
								tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
							}
							continue;
						}
						else{
							if((tok1[0] != '\0' && strcmp(tok4,"X") == 0) || (tok1[0] == 0 && strcmp(tok3,"X") == 0)){
								sym_node* temp;
								int sym_loc;
								_x = 1; _i=1; _n=1;
								tok6[strlen(tok6)-1] = '\0';
								temp = sym_header;
								while(temp){
									if(strcmp(tok6,temp->label) == 0){
										sym_loc = temp->locctr;
										break;
									}
									temp = temp->link;
								}
								if(!temp){
									//free memory
									sym_node *temp;
									if(sym_header == NULL){
										fclose(pc_fp);
										fclose(ob_fp);
										fclose(lst_fp);
										fclose(ob_fp2);
										remove(fileName);
										remove(fileName2);
										strcat(fileName,"tmp");
										remove(fileName);
										return line/5;
									}
									if(sym_header->link == NULL)
										free(sym_header);
									else{
										temp = sym_header->link;
										while(temp){
											free(sym_header);
											sym_header = temp;
											temp = temp->link;
										}
										free(sym_header);
										sym_header = NULL;
									}
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								//symbol 찾음
								if(_e == 0){
									if(sym_loc - pc_loc > 2047 || sym_loc - pc_loc < -2048){
										//base relative
										_b = 1;
										ob_code += sym_loc - base_loc;
										if(sym_loc - base_loc < 0)	ob_code+=16*16*16;
									}
									else{
										//pc relative
										_p = 1;
										ob_code += sym_loc - pc_loc;
										if(sym_loc - pc_loc < 0) ob_code+=16*16*16;
									}
									ob_code += _n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_;
								}
								else{
									ob_code += 16*16*(_n*_N_ + _i*_I_ + _x*_X_ + _b*_B_ + _p*_P_ + _e*_E_);
									ob_code += sym_loc;
								}
								if(_e == 0){
									if(tok1[0] != '\0')
										fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%s, %s\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,tok4,ob_code);
									else
										fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%s, %s\t\t%06X\n",line,loc_ctr," ",tok5,tok6,tok3,ob_code);
								}
								else{
									if(tok1[0] != '\0')
										fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%s, %s\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,tok4,ob_code);
									else
										fprintf(lst_fp,"%d\t\t%04X\t%-7s\t+%-5s\t%s, %s\t\t%06X\n",line,loc_ctr," ",tok5,tok6,tok3,ob_code);
								}
								if(_e == 0){
									ob_cnt += 3;
									M_cnt += 3;
								}
								else{
									M[M_i++] = M_cnt+1;
									ob_cnt += 4;
									M_cnt += 4;
								}
								if(ob_cnt > 0x1D || newLine == 1){
									fprintf(ob_fp,"\nT%06X",loc_ctr);
									if(_e == 0){
										ob_cnt = 3;
									}
									else{
										ob_cnt = 4;
									}
									newLine = 0;
								}
								if(_e == 0){
									fprintf(ob_fp,"%06X",ob_code);
								}
								else{
									fprintf(ob_fp,"%08X",ob_code);
								}
								line+=5;
								fgets(str,ASSEM_SIZE,fp);
								do{
									fgets(str2,ASSEM_SIZE,pc_fp);
									pc = sscanf(str2,"%d %*s",&pc_loc);
								}while(pc==0);
								tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
								if(tok_num == 0){
									tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
								}
								continue;
							}
							else{
								//free memory??
								sym_node *temp;
								if(sym_header == NULL){
									fclose(pc_fp);
									fclose(ob_fp);
									fclose(lst_fp);
									fclose(ob_fp2);
									remove(fileName);
									remove(fileName2);
									strcat(fileName,"tmp");
									remove(fileName);
									return line/5;
								}
								if(sym_header->link == NULL)
									free(sym_header);
								else{
									temp = sym_header->link;
									while(temp){
										free(sym_header);
										sym_header = temp;
										temp = temp->link;
									}
									free(sym_header);
									sym_header = NULL;
								}
								fclose(pc_fp);
								fclose(ob_fp);
								fclose(lst_fp);
								fclose(ob_fp2);
								remove(fileName);
								remove(fileName2);
								strcat(fileName,"tmp");
								remove(fileName);
								return line/5;
							}
						}//end of if buffer,
					}//end of format 3/4
				}//end of if not single mnemo
			}// end of if opcode >= 0
			else if(strcmp(tok5,"BYTE") == 0){
				unsigned int i;
				if((*tok6) == 'C'){
					for(i=0; i < strlen(tok6); i++)
						tok6[i] = tok6[i+1];
					for(i=0; i < strlen(tok6); i++)
						tok6[i] = tok6[i+1];
					tok6[strlen(tok6)-1] = '\0';
					for(i=0; i < strlen(tok6); i++ ,ob_code*=16*16){
						ob_code += tok6[i];
					}
					ob_code /= 16*16;					
					fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\tC'%s'\t\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
					if(i==1){
						ob_cnt+=1;
						M_cnt+=1;
					}
					else if(i == 2){
						ob_cnt+=2;
						M_cnt+=2;
					}
					else{
						ob_cnt+=3;
						M_cnt+=3;
					}
					if(ob_cnt > 0x1D || newLine == 1){
						fprintf(ob_fp,"\nT%06X",loc_ctr);
						ob_cnt = i;
						newLine = 0;
					}
					if(i == 1)
						fprintf(ob_fp,"%02X",ob_code);
					else if(i == 2)
						fprintf(ob_fp,"%04X",ob_code);
					else
						fprintf(ob_fp,"%06X",ob_code);
				}
				else{
					for(i=0; i < strlen(tok6); i++)
						tok6[i] = tok6[i+1];
					for(i=0; i < strlen(tok6); i++)
						tok6[i] = tok6[i+1];
					tok6[strlen(tok6)-1] = '\0';
					ob_code = (int)strtol(tok6,NULL,16);
					fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\tX'%s'\t\t\t%02X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);
					ob_cnt += 1;
					M_cnt += 1;
					if(ob_cnt > 0x1D || newLine == 1){
						fprintf(ob_fp,"\nT%06X",loc_ctr);
						ob_cnt = 1;
						newLine = 0;
					}
					fprintf(ob_fp,"%02X",ob_code);
				}				
				line+=5;
				fgets(str,ASSEM_SIZE,fp);
				do{
					fgets(str2,ASSEM_SIZE,pc_fp);
					pc = sscanf(str2,"%d %*s",&pc_loc);
				}while(pc==0);
				tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
				if(tok_num == 0){
					tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
				}
				continue;
			}
			else if(strcmp(tok5,"WORD") == 0){
				ob_code += (int)strtol(tok6,NULL,10);
				fprintf(ob_fp,"%06X",ob_code);
				ob_cnt += 3;
				M_cnt += 3;
				if(ob_cnt > 0x1D || newLine == 1){
					fprintf(ob_fp,"\nT%06X",loc_ctr);
					ob_cnt = 3;
					newLine = 0;
				}
				fprintf(ob_fp,"%06X",ob_code);
				fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%-15s\t\t%06X\n",line,loc_ctr,tok1,tok5,tok6,ob_code);				
				line+=5;
				fgets(str,ASSEM_SIZE,fp);
				do{
					fgets(str2,ASSEM_SIZE,pc_fp);
					pc = sscanf(str2,"%d %*s",&pc_loc);
				}while(pc==0);
				tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
				if(tok_num == 0){
					tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
				}
				continue;
			}
			else{	//RESW REWB
				newLine = 1;
				fprintf(lst_fp,"%d\t\t%04X\t%-7s\t%-5s\t%s\n",line,loc_ctr,tok1,tok5,tok6);
				line+=5;
				fgets(str,ASSEM_SIZE,fp);
				do{
					fgets(str2,ASSEM_SIZE,pc_fp);
					pc = sscanf(str2,"%d %*s",&pc_loc);
				}while(pc==0);				
				tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
				if(tok_num == 0){
					tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
				}
				continue;
			}
		}
		//end of if not comment
		else{	//주석일때
			fprintf(lst_fp,"%d\t\t%s",line,str);
			line+=5;
			fgets(str,ASSEM_SIZE,fp);
			tok_num = sscanf(str,"%d %s %s %s %s",&loc_ctr,tok1,tok2,tok3,tok4);
			if(tok_num == 0){
				tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
			}
		}
	}//end of while
	fprintf(lst_fp,"%d\t\t%4s\t%-7s\t%-5s\t%s",line," "," ",tok1,tok2);
	if(M[0]>0)
		for(i=0; M[i] > 0; i++)
			fprintf(ob_fp,"\nM%06X05",M[i]);
	fprintf(ob_fp,"\nE%06X",start_loc);
	fclose(ob_fp);
	ob_fp = fopen(strcat(fileName,"tmp"),"r");
	do{
		fgets(str,ASSEM_SIZE,ob_fp);
		if((*str) != 'T')
			fprintf(ob_fp2,"%s",str);
		else{	
			int length=0;
			sscanf(str,"%7s%s",tok1,str2);
			length = strlen(str2)/2;
			fprintf(ob_fp2,"%7s%02X%s\n",tok1,length,str2);
		}
	}while((*str) != 'E');
	fclose(ob_fp);
	fclose(ob_fp2);
	fclose(pc_fp);
	remove(fileName);
	fclose(lst_fp);
	return 0;
}
/*------------------------------------------------------------------------------------
  Name: pass1
  Object: 강의자료에서 pass1에 해당하는 함수
  Parameters: FILE *fp : .asm파일, char *fileName : 2_5.asm에서 2_5에 해당하는부분
  Return: 에러가 발생한 Line, pass1이 성공하면 pass2의 리턴값을 리턴
  ------------------------------------------------------------------------------------*/
int pass1(FILE *fp, char *fileName){
	FILE *wfp;
	char str[ASSEM_SIZE];
	char tok1[TOKEN_SIZE], tok2[TOKEN_SIZE], tok3[TOKEN_SIZE], tok4[TOKEN_SIZE], tok5[TOKEN_SIZE], base[TOKEN_SIZE];
	int loc_ctr, tok_num, start_loc, end_loc, base_loc, sum = 0;
	short int fmat4 = 0;
	int line = 1, ps2;
	char *stop;

	wfp = fopen(fileName, "wt");
	if(!wfp)	return -1;

	fgets(str,ASSEM_SIZE,fp);
	setStr(str);
	tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
	
	if(strcmp(tok2,"START") == 0){
		loc_ctr = (int)strtol(tok3,&stop,16);
		start_loc = loc_ctr;
		fprintf(wfp, "%d\t%s", loc_ctr,str);
		fgets(str,ASSEM_SIZE,fp);
		setStr(str);
		tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
		line++;
	}
	else if(strcmp(tok1,"START") == 0){
		loc_ctr = (int)strtol(tok2,&stop,16);
		start_loc = loc_ctr;
		fprintf(wfp, "%d\t%s", loc_ctr,str);
		fgets(str,ASSEM_SIZE,fp);
		setStr(str);
		tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
		line++;
	}
	else{
		loc_ctr = 0;
		start_loc = 0;
	}
	base[0] = '\0';
	while(strcmp(tok1,"END") != 0){
		if((*tok1) != '.'){
			if((tok_num == 3 && tok2[strlen(tok2)-1] != ',') || (tok_num == 2 && isSingleMnemo(tok2) == 1) || tok_num == 4){
				sym_node *temp;
				sym_node *newNode;

				newNode = (sym_node*)malloc(sizeof(sym_node));
				newNode->label = (char*)malloc(sizeof(char)*(strlen(tok1)+1));
				strcpy(newNode->label,tok1);
				newNode->locctr = loc_ctr;
				newNode->link = NULL;
				if(sym_header == NULL)
					sym_header = newNode;
				else{
					temp = sym_header;
					while(temp != NULL){
						if(strcmp(temp->label,tok1) == 0){
							//free memory
							sym_node *temp;
							if(sym_header == NULL){
								fclose(wfp);
								remove(fileName);
								return line;
							}
							if(sym_header->link == NULL)
								free(sym_header);
							else{
								temp = sym_header->link;
								while(temp){
									free(sym_header);
									sym_header = temp;
									temp = temp->link;
								}
								free(sym_header);
								sym_header = NULL;
							}
							fclose(wfp);
							remove(fileName);
							return line;
						}
						else{
							temp = temp->link;
						}
					}
					temp = sym_header;
					while(temp->link != NULL)
						temp = temp -> link;
					temp->link = newNode;
				}
			}
			if(tok_num == 1)	strcpy(tok5,tok1);
			else if(tok_num == 2){
				if(isSingleMnemo(tok2) == 1)
					strcpy(tok5,tok2);
			
				else
					strcpy(tok5,tok1);
			}
			else if(tok_num == 3){
				if(tok2[strlen(tok2)-1] != ',')
					strcpy(tok5,tok2);
				else
					strcpy(tok5,tok1);
			}
			else
				strcpy(tok5,tok2);
			if(tok5[0] == '+'){
				unsigned int i;
				fmat4 = 1;
				for(i=0; i < strlen(tok5); i++)
					tok5[i] = tok5[i+1];
			}
			if(strcmp(tok5,"WORD") == 0){				
				sum = 3;
			}
			else if(strcmp(tok5,"RESW") == 0){
				sum += 3*(int)strtol(tok3,&stop,10);
			}
			else if(strcmp(tok5,"RESB") == 0){
				sum = (int)strtol(tok3,&stop,10);
			}
			else if(strcmp(tok5,"BYTE") == 0){
				if((*tok3) == 'X'){
					if((strlen(tok3)-3)%2 == 0)
						sum = (strlen(tok3)-3)/2; 
					else
						sum = (strlen(tok3)-3)/2+1;
				}
				else if((*tok3) == 'C'){
					sum = (strlen(tok3)-3);
				}
				else{
					sum = 1;
				}
			}
			else if(strcmp(tok5,"BASE") == 0){
				strcpy(base,tok2);
				fprintf(wfp, "%4s\t%s"," ",str);
				fgets(str,ASSEM_SIZE,fp);
				setStr(str);
				tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
				line++;
				continue;
			}
			else if(strcmp(tok5,"NOBASE") == 0){
				fprintf(wfp, "%4s\t%s"," ",str);
				fgets(str,ASSEM_SIZE,fp);
				setStr(str);
				tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
				line++;
				continue;
			}
			else{
				int key;
				op_node *temp;
				key = tok5[0]%20;
				temp = op_header[key];
				while(temp){
					if(strcmp(tok5,temp->mnemo) == 0){
						sum = temp->format;
						if(fmat4 == 1)	sum++;
						break;
					}
					else
						temp = temp->link;
				}
				if(!temp){
					//free memory
					sym_node *temp;
						if(sym_header == NULL){
							fclose(wfp);
							remove(fileName);
							return line;
						}
						if(sym_header->link == NULL)
							free(sym_header);
						else{
							temp = sym_header->link;
							while(temp){
								free(sym_header);
								sym_header = temp;
								temp = temp->link;
							}
							free(sym_header);
							sym_header = NULL;
						}
					fclose(wfp);
					remove(fileName);
					return line; 
				}
			}
			fprintf(wfp, "%d\t%s", loc_ctr,str);
			fgets(str,ASSEM_SIZE,fp);
			setStr(str);
			tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
			line++;
			loc_ctr += sum;
			sum = 0;
			fmat4 = 0;
		}
		else{
			fprintf(wfp, "%4s\t%s"," ",str);
			fgets(str,ASSEM_SIZE,fp);
			setStr(str);
			tok_num = sscanf(str,"%s %s %s %s",tok1,tok2,tok3,tok4);
			line++;			
		}
	}
	end_loc = loc_ctr;
	fprintf(wfp,"%d\t%4s\t%s",end_loc," ",str);
	base_loc = getLoc(base);
	fclose(wfp);
	wfp = fopen(fileName,"r");
	strcpy(str,fileName);
	ps2 = pass2(wfp,fileName,start_loc,end_loc,base_loc);
	fclose(wfp);
	remove(str);
	return ps2;
}
/*------------------------------------------------------------------------------------
  Name: getInput
  Object: 사용자에게 입력받은 명령어가 유효한지 검사하고 그에 해당하는 함수를 호출한다.
  Parameters: char *input : 사용자에게 입력받은 명령어
  Return: None
  ------------------------------------------------------------------------------------*/
void getInput(){
	char input[MAX_LENGTH], temp[MAX_LENGTH];
	char tok1[TOKEN_SIZE], tok2[TOKEN_SIZE], tok3[TOKEN_SIZE];
	
	fgets(input,MAX_LENGTH,stdin);
	if(strcmp(input,"\n") == 0)	return ;
	input[strlen(input)-1] = '\0';
	strcpy(temp,input);
	sscanf(input,"%s %*s",tok1);
	if(strcmp(tok1,"h") == 0 || strcmp(tok1,"help") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		addToHistory(temp);
		printHelp();
	}
	else if(strcmp(tok1,"d") == 0 || strcmp(tok1,"dir") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		addToHistory(temp);
		printDir();
	}
	else if(strcmp(tok1,"q") == 0 || strcmp(tok1,"quit") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		exit(0);
	} 
	else if(strcmp(tok1,"hi") == 0 || strcmp(tok1,"history") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		addToHistory(temp);
		printHistory();
	}
	else if(strcmp(tok1,"du") == 0 || strcmp(tok1,"dump") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c == ' '){
			printDump(-1,-1);
			addToHistory(temp);
			return;
		}
		else{
			int start, end;
			char c;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c",tok2,&start,&c,&end,&t);
			if(tok_num == 1 || tok_num == 3 || t != ' '){
				printWrong();
				return;
			}
			else{
				if(tok_num == 2){
					if(start > 0xFFFFF){
						printWrong();
						return;
					}
					printDump(start,-1);
					addToHistory(temp);
					return;
				}
				else{
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
		char c = ' ' ;
		sscanf(input,"%*s %c",&c);
		if(c == ' '){
			printWrong();
			return;
		}
		else{
			int loc, val;
			char c;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c",tok2,&loc,&c,&val,&t);
			if(tok_num != 4){
				printWrong();
				return;
			}
			else{
				if(c != ',' ||  loc > 0xFFFFF || val > 0xFF || val < 0){
						printWrong();
						return;
				}
				else{
					memory[loc] = val;
					addToHistory(temp);
					return;
				}
			}
		}
	}
	else if(strcmp(tok1,"f") == 0 || strcmp(tok1,"fill") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c == ' '){
			printWrong();
			return;
		}
		else{
			int start, end, value;
			char c1, c2;
			char t = ' ';
			int tok_num = sscanf(temp,"%s %x %c %x %c %x %c",tok2,&start,&c1,&end, &c2, &value, &t);
			if(tok_num != 6){
				printWrong();
				return;
			}
			else{
				if(c1 != ',' || c2 != ',' || start > 0xFFFFF || start > end || value > 0xFF || value < 0){
					printWrong();
					return;
				}
				else{
					int i;
					for(i=start; i<=end; i++)
						memory[i] = value;
					addToHistory(temp);
				}
			}
		}
	}
	else if(strcmp(tok1,"reset") == 0){
		int i;
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		for(i=0;i<1048576;i++)	
			memory[i] = 0;
		addToHistory(temp);
		return;
	}
	else if(strcmp(tok1,"opcode") == 0){
		int key;
		op_node *tmp;
		char c = ' ';
		int tok_num;
		tok_num = sscanf(input,"%*s %s %c",tok2, &c);
		if(tok_num != 1){
			printWrong();
			return;
		}
		key = tok2[0] % 20;
		tmp = op_header[key];
		if(!tmp){
			printWrong();
			return;
		}
		else{
			while(tmp){
				if(strcmp(tmp->mnemo,tok2) == 0){
					printf("opcode is %X\n", tmp->opcode);
					addToHistory(temp);
					return;
				}
				tmp = tmp->link;
			}
			printWrong();
			return;
		}
	}
	else if(strcmp(tok1,"opcodelist") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		addToHistory(temp);
		printOplist();
	}
	else if(strcmp(tok1,"type") == 0){
		FILE *fp;
		char c = ' ';
		int tok_num;
		tok_num = sscanf(temp,"%*s %s %c",tok2, &c);
		if(tok_num != 1){
			printWrong();
			return;
		}
		fp = fopen(tok2,"r");
		if(!fp){
			printWrong();
			return;
		}
		printFile(fp);
		addToHistory(temp);
		fclose(fp);
	}
	else if(strcmp(tok1,"assemble") == 0){
		FILE *fp;
		char c = ' ';
		int tok_num;
		int line;
		tok_num = sscanf(input,"%*s %s %c", tok2, &c);
		if(tok_num != 1){
			printWrong();
			return;
		}
		strcpy(tok1,tok2);
		tok_num = sscanf(tok1,"%[^.].%s",tok2,tok3);
		if(tok_num != 2 || strcmp(tok3,"asm") != 0){
			printWrong();
			return;
		}
		fp = fopen(tok1,"r");
		if(!fp){
			printWrong();
			return;
		}
		line = pass1(fp,tok2);
		if (line < 0){	
			printf("File open error\n");
			fclose(fp);
			return;
		}
		else if(line != 0){
			printf("Error in line %d\n", line);
			return;
		}
		addToHistory(temp);
		fclose(fp);
	}
	else if(strcmp(tok1,"symbol") == 0){
		char c = ' ';
		sscanf(input,"%*s %c",&c);
		if(c != ' '){
			printWrong();
			return;
		}
		addToHistory(temp);
		if(sym_header != NULL)
			printSymbol();
		return;
	}
	else{
		printWrong();
		return ;
	}
	return;
}
