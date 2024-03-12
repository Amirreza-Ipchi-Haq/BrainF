#include<stdio.h>
#include"cstrcal.h"
#define NEWLINE(c) ((char)c=='\n'||(char)c=='\r')//Defines a macro to check if a character is a whitespace
char *append(char *array,size_t len,char front){
	char *newArray=malloc(sizeof(array)+sizeof(char));
	for(size_t i=len;i--;newArray[i+front]=array[i]);
	newArray[front?0:len]=0,free(array),array=newArray;
	return newArray;
}
#ifdef _WIN32
#include<io.h>
#define isatty _isatty
char win=1;
#else
#include<unistd.h>
char win=0;
#endif
int main(int argc,char** argv){
	char *code=0,*pointer,skip=1;
	size_t len=1;
	FILE *input=fopen(win?"CON:":"/dev/tty","r");
	if(argc>1){
		for(size_t i=0;++i<(size_t)argc;)
			if(!cmpstr(argv[1],"--help")){
				printf("Usage: BrainF_"),printf(win?"Windows":"Linux"),printf(" [options] [filename]\n\nOptions:\n--skip-newline         Skip the newline (and the carriage return) in the input stream\n--do-not-skip-newline  Include the newline (and the carriage return) in the input stream\n");
				if(code)
					free(code);
				return 0;
			}
			else if(!cmpstr(argv[i],"--skip-newline"))
				skip=1;
			else if(!cmpstr(argv[i],"--do-not-skip-newline"))
				skip=0;
			else{
				FILE *file=fopen(argv[i],"r");//Open the file
				if(file){
					if(code)
						free(code);
					code=strtmp("",0);
					for(char command=getc(file);!feof(file);command=getc(file))//Read each character of the file
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
					fclose(file);
				}
			}
	}
	pointer=malloc(sizeof(char)),pointer[0]=0;
	if(!code&&isatty(0)){//(Interactive mode)
		printf("Entered interactive-mode!\n"),code=strtmp("",0);//Notify
		for(size_t pointerLocation=0;1;free(code),code=strtmp("",0)){//Read a line each time
			printf(">>> ");//Prompt
			for(char command=getchar();command!='\n';command=getchar())//Read from the input
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
			for(size_t codeLocation=0;codeLocation<strlen(code);codeLocation++)//Read a part of code each time
				switch(code[codeLocation]){//Choose a command based on the code
					case '>'://Push the pointer indicator forward
						if(++pointerLocation==len)//Add a new pointer to the back if needed
							pointer=append(pointer,len,0),len++;
						break;
					case '<'://Push the pointer indicator backward
						if(!pointerLocation--)//Add a new pointer to the front if needed
							pointer=append(pointer,len,1),len++,pointerLocation++;
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
						while(skip&&NEWLINE(pointer[pointerLocation]))
							pointer[pointerLocation]=getc(input);
						break;
					case '['://Skip to a matching `]` if the current pointer value is 0
						if(!pointer[pointerLocation]){
							size_t debug=codeLocation;//Save current pointer location for debug purposes
							codeLocation++;//Increase the pointer indicator value
							for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
								if(codeLocation+1==strlen(code)&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
									fprintf(stderr,"Error at locaton %lu: Missing ']'!\n",debug);
									goto finish;
								}else if(code[codeLocation]=='[')//(Nested `[`)
									i++;
								else if(code[codeLocation]==']')//(Matching `]`)
									i--;
							codeLocation--;//Decrease the pointer indicator value
						}
						break;
					case ']'://Go back to a matching `]` if the current pointer value is non-zero
						if(pointer[pointerLocation]){
							size_t debug=codeLocation;//Save current pointer location for debug purposes
							codeLocation--;//Decrease the pointer indicator value
							for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
								if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
									fprintf(stderr,"Error at locaton %lu: Missing '['!\n",debug);
									goto finish;
								}else if(code[codeLocation]=='[')//(Nested `[`)
									i--;
								else if(code[codeLocation]==']')//(Matching `]`)
									i++;
							codeLocation++;//Increase the pointer indicator value
						}
				}
			finish:
		}
	}else if(!code){//Read from a pipe
		code=strtmp("",0);//Open input
		for(char command=getchar();!feof(stdin);command=getchar())//Read each character of the file
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
	}
	for(size_t codeLocation=0,pointerLocation=0;codeLocation<strlen(code);codeLocation++)//Read a part of code each time
		switch(code[codeLocation]){//Choose a command based on the code
			case '>'://Push the pointer indicator forward
				if(++pointerLocation==len)//Add a new pointer to the back if needed
					pointer=append(pointer,len,0),len++;
				break;
			case '<'://Push the pointer indicator backward
				if(!pointerLocation--)//Add a new pointer to the front if needed
					pointer=append(pointer,len,1),len++,pointerLocation++;
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
			case ','://Input a character to the current pointer
				pointer[pointerLocation]=isatty(0)?getchar():getc(input);
				while(skip&&NEWLINE(pointer[pointerLocation]))
					pointer[pointerLocation]=isatty(0)?getchar():getc(input);
				break;
			case '['://Skip to a matching `]` if the current pointer value is 0
				if(!pointer[pointerLocation]){
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation++;//Increase the pointer indicator value
					for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
						if(codeLocation+1==strlen(code)&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
							free(code),free(pointer),fprintf(stderr,"Error at locaton %lu: Missing ']'!\n",debug);
							return 1;
						}else if(code[codeLocation]=='[')//(Nested `[`)
							i++;
						else if(code[codeLocation]==']')//(Matching `]`)
							i--;
					codeLocation--;//Decrease the pointer indicator value
				}
				break;
			case ']'://Go back to a matching `[` if the current pointer value is non-zero
				if(pointer[pointerLocation]){
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation--;//Decrease the pointer indicator value
					for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
						if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
							free(code),free(pointer),fprintf(stderr,"Error at locaton %lu: Missing '['!\n",debug);
							return 1;
						}else if(code[codeLocation]=='[')//(Matching `[`)
							i--;
						else if(code[codeLocation]==']')//(Nested `]`)
							i++;
					codeLocation++;//Increase the pointer indicator value
				}
		}
	free(code),free(pointer);
	return 0;
}
