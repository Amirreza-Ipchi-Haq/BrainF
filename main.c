#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef _WIN32
#include<io.h>
#define isatty _isatty
#define STDIN "CON:"
#else
#include<unistd.h>
#define STDIN "/dev/tty"
#endif
char *code=NULL,*pointer=NULL,exitCode=0;
size_t len=128,lenCode=0,lenCodeFull=128,pointerLocation=63;
FILE *input=NULL;
void *safeRealloc(void *ptr,size_t size){
	void *tmpptr=realloc(ptr,size);
	if(!tmpptr){
		free(ptr);
	}
	return tmpptr;
}
void read0(FILE *file){
	lenCode=0;
	for(char command=getc(file);input&&isatty(0)?command!='\n':!feof(file);command=getc(file)){//Read each character of the file
		switch(command){//Only store the commands acceptable for the language
			case '>':
			case '<':
			case '+':
			case '-':
			case '.':
			case ',':
			case '[':
			case ']':
				if(lenCode==lenCodeFull){
					lenCodeFull*=2;
					code=safeRealloc(code,lenCodeFull);
					if(!code){
						fclose(input?input:file);
						if(pointer){
							free(pointer);
						}
						perror("Memory error!");
						exit(1);
					}
				}
				code[lenCode++]=command;
		}
	}
	if(!(input&&isatty(0))){
		char *tmpCode=realloc(code,lenCode);
		if(tmpCode){
			code=tmpCode;
		}
	}
	if(!pointer){
		pointer=malloc(128);
		if(!pointer){
			if(input){
				fclose(input);
			}
			free(code);
			perror("Memory error!");
			exit(1);
		}
		for(unsigned char i=128;i--;){
			pointer[i]=0;
		}
	}
}
void append(){	
	len*=2;
	pointer=safeRealloc(pointer,len);
	if(!pointer){
		if(input){
			fclose(input);
		}
		free(code);
		free(pointer);
		perror("Memory error!");
		exit(1);
	}
}
void execute(){
	for(size_t codeLocation=0;codeLocation<lenCode;codeLocation++)//Read a part of code each time
		switch(code[codeLocation]){//Choose a command based on the code
			case '>'://Push the pointer indicator forward
				if(++pointerLocation==len){//Add a new pointer to the back if needed
					append();
					for(size_t i=len/2;i<len;i++){
						pointer[i]=0;
					}
				}
				break;
			case '<'://Push the pointer indicator backward
				if(!pointerLocation--){//Add a new pointer to the front if needed
					pointerLocation=len-1;
					append();
					memcpy(pointer+len/2,pointer,len/2);
					for(size_t i=len/2;i--;){
						pointer[i]=0;
					}
				}
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
				pointer[pointerLocation]=getc(input?input:stdin);
				break;
			case '['://Skip to a matching `]` if the current pointer value is 0
				if(!pointer[pointerLocation]){
					if(codeLocation+1==lenCode){
						exitCode=1,fprintf(stderr,"\nError at locaton %zu: Missing ']'!\n",codeLocation);
						return;
					}
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation++;//Increase the pointer indicator value
					for(size_t i=1;i;codeLocation++){//Skip part of the code until reaching a matching `]`
						if(codeLocation+1==lenCode&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
							exitCode=1,fprintf(stderr,"\nError at locaton %zu: Missing ']'!\n",debug);
							return;
						}else if(code[codeLocation]=='['){//(Nested `[`)
							i++;
						}else if(code[codeLocation]==']'){//(Matching `]`)
							i--;
						}
					}
					codeLocation--;//Decrease the pointer indicator value
				}
				break;
			case ']'://Go back to a matching `]` if the current pointer value is non-zero
				if(pointer[pointerLocation]){
					if(!codeLocation){
						exitCode=1,fprintf(stderr,"\nError at locaton %zu: Missing '['!\n",codeLocation);
						return;
					}
					size_t debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation--;//Decrease the pointer indicator value
					for(size_t i=1;i;codeLocation--){//Go back until reaching a matching `[`
						if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
							exitCode=1,fprintf(stderr,"\nError at locaton %zu: Missing '['!\n",debug);
							return;
						}else if(code[codeLocation]=='['){//(Nested `[`)
							i--;
						}else if(code[codeLocation]==']'){//(Matching `]`)
							i++;
						}
					}
					codeLocation++;//Increase the pointer indicator value
				}
		}
}
int main(int argc,char** argv){
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
	if(argc>1){
		FILE *file=fopen(argv[1],"r");//Open the file
		if(!file){
			perror("Error trying to open file!");
			exit(1);
		}
		code=malloc(128);
		if(!code){
			fclose(input);
			fclose(file);
			perror("Memory error!");
			exit(1);
		}
		read0(file);
		fclose(file);
	}else{
		input=fopen(STDIN,"r");
		if(!input){
			perror("Memory error!");
			exit(1);
		}
	}
	if(!code){
		code=malloc(128);
		if(!code){
			fclose(input);
			perror("Memory error!");
			exit(1);
		}
	}
	if(!lenCode&&isatty(0)){//(shell mode)
		printf("Entered shell-mode!");//Notify
		while(1){
			printf("\n%%%%%% ");
			read0(stdin);
			execute();//Prompt
		}
	}else if(!lenCode){
		read0(stdin);
	}
	execute();
	if(input){
		fclose(input);
	}
	free(code);
	free(pointer);
	return exitCode;
}
