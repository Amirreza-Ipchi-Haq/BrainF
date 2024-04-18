#include<stdio.h>
#include"cstrcal.h"
#ifdef _WIN32
#include<io.h>
#define isatty _isatty
#define STDIN "CON:"
#else
#include<unistd.h>
#define STDIN "/dev/tty"
#endif
char *code,*pointer,exitCode=0;
size_t len=1,pointerLocation=0;
FILE *input;
void read0(FILE *file){
	if(code[0])
		free(code),code=strtmp("",0);
	for(char command=getc(file);file==stdin&&isatty(0)?command!='\n':!feof(file);command=getc(file))//Read each character of the file
		switch(command){//Only store the commands acceptable for the language
			case '>':
			case '<':
			case '+':
			case '-':
			case '.':
			case ',':
			case '[':
			case ']':
				code=strappend(code,CHR2STR(command),2);
		}
	return;
}
void append(char front){
	pointer=realloc(pointer,len+1);
	if(front){
		for(size_t i=len++;i--;pointer[i+1]=pointer[i]);
		pointer[0]=0;
	}else
		pointer[len++]=0;
	return;
}
void execute(){
	for(size_t codeLocation=0,lenCode=strlen(code);codeLocation<lenCode;codeLocation++)//Read a part of code each time
		switch(code[codeLocation]){//Choose a command based on the code
			case '>'://Push the pointer indicator forward
				if(++pointerLocation==len)//Add a new pointer to the back if needed
					append(0);
				break;
			case '<'://Push the pointer indicator backward
				if(!pointerLocation--)//Add a new pointer to the front if needed
					append(1),pointerLocation++;
				break;
			case '+'://Increase the current pointer value by 1
				pointer[pointerLocation]++;
				break;
			case '-'://Decrease the currnet pointer value by 1
				pointer[pointerLocation]--;
				break;
			case '.'://Output the current pointer as a character
				putchar(pointer[pointerLocation]);
				break;
			case ','://Input a character to the current pointer and skip the newline
				pointer[pointerLocation]=getc(input);
				break;
			case '['://Skip to a matching `]` if the current pointer value is 0
				if(!pointer[pointerLocation]){
					if(codeLocation+1==lenCode){
						exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing ']'!\n",codeLocation);
						return;
					}
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation++;//Increase the pointer indicator value
					for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
						if(codeLocation+1==lenCode&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
							exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing ']'!\n",debug);
							return;
						}else if(code[codeLocation]=='[')//(Nested `[`)
							i++;
						else if(code[codeLocation]==']')//(Matching `]`)
							i--;
					codeLocation--;//Decrease the pointer indicator value
				}
				break;
			case ']'://Go back to a matching `]` if the current pointer value is non-zero
				if(pointer[pointerLocation]){
					if(!codeLocation){
						exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing '['!\n",codeLocation);
						return;
					}
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation--;//Decrease the pointer indicator value
					for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
						if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
							exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing '['!\n",debug);
							return;
						}else if(code[codeLocation]=='[')//(Nested `[`)
							i--;
						else if(code[codeLocation]==']')//(Matching `]`)
							i++;
					codeLocation++;//Increase the pointer indicator value
				}
		}
	return;
}
int main(int argc,char** argv){
	setbuf(stdout,0),input=fopen(STDIN,"r"),code=strtmp("",0);
	if(argc>1){
		FILE *file=fopen(argv[1],"r");//Open the file
		if(file)
			read0(file);
		fclose(file);
	}
	pointer=malloc(sizeof(char)),pointer[0]=0;
	if(!code[0]&&isatty(0)){//(shell mode)
		printf("Entered shell-mode!");//Notify
		while(1)
			printf("\n>>> "),read0(stdin),execute();//Prompt
	}else if(!code[0])
		read0(stdin);
	execute(),free(code),free(pointer);
	return exitCode;
}
