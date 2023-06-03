#include<cstdio>
#include<deque>
#include<string>
#ifdef _WIN32
#include<io.h>
#define isatty _isatty
#else
#include<unistd.h>
#endif
int main(int argc,char** argv){
	std::string code;//Initialize the main code
	std::deque<char>pointer={0};//Initialize pointers
	FILE *input=0;//Initialize input for pipes
	if(argc>1){//Read from a file if argument is given
		FILE *file=fopen(argv[1],"r");//Open the file
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
					code+=command;
			}
		fclose(file);//Close the file
	}else if(isatty(0)){//(Interactive mode)
		printf("Entered interactive-mode!\n");//Notify
		for(size_t pointerLocation=0;1;code=""){//Read a line each time
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
						code+=command;
				}
			for(size_t codeLocation=0;codeLocation<code.size();codeLocation++)//Read a part of code each time
				switch(code[codeLocation]){//Choose a command based on the code
					case '>'://Push the pointer indicator forward
						if(++pointerLocation==pointer.size())//Add a new pointer to the back if needed
							pointer.push_back(0);
						break;
					case '<'://Push the pointer indicator backward
						if(!pointerLocation--)//Add a new pointer to the front if needed
							pointer.push_front(0),pointerLocation++;
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
						pointer[pointerLocation]=getchar();
						getchar();//Skip the newline
						break;
					case '['://Skip to a matching `]` if the current pointer value is 0
						if(!pointer[pointerLocation]){
							auto debug=codeLocation;//Save current pointer location for debug purposes
							codeLocation++;//Increase the pointer indicator value
							for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
								if(codeLocation+1==code.size()&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
									fprintf(stderr,"Error at locaton %u: Missing ']'!\n",debug);
									return 2;
								}else if(code[codeLocation]=='[')//(Nested `[`)
									i++;
								else if(code[codeLocation]==']')//(Matching `]`)
									i--;
							codeLocation--;//Decrease the pointer indicator value
						}
						break;
					case ']'://Go back to a matching `]` if the current pointer value is non-zero
						if(pointer[pointerLocation]){
							auto debug=codeLocation;//Save current pointer location for debug purposes
							codeLocation--;//Decrease the pointer indicator value
							for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
								if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
									fprintf(stderr,"Error at locaton %u: Missing '['!\n",debug);
									return 2;
								}else if(code[codeLocation]=='[')//(Nested `[`)
									i--;
								else if(code[codeLocation]==']')//(Matching `]`)
									i++;
							codeLocation++;//Increase the pointer indicator value
						}
				}
		}
	}else{//Read from a pipe
		input=fopen("/dev/tty","r");//Open input
		if(!input){//Halt if the current operating system is Windows
			fprintf(stderr,"Error!\n");
			return 1;
		}
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
					code+=command;
			}
	}
	for(size_t codeLocation=0,pointerLocation=0;codeLocation<code.size();codeLocation++)//Read a part of code each time
		switch(code[codeLocation]){//Choose a command based on the code
			case '>'://Push the pointer indicator forward
				if(++pointerLocation==pointer.size())//Add a new pointer to the back if needed
					pointer.push_back(0);
				break;
			case '<'://Push the pointer indicator backward
				if(!pointerLocation--)//Add a new pointer to the front if needed
					pointer.push_front(0),pointerLocation++;
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
				if(isatty(0))//(Not piped)
					pointer[pointerLocation]=getchar();
				else//(Piped)
					pointer[pointerLocation]=getc(input);
				break;
			case '['://Skip to a matching `]` if the current pointer value is 0
				if(!pointer[pointerLocation]){
					auto debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation++;//Increase the pointer indicator value
					for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
						if(codeLocation+1==code.size()&&(code[codeLocation]!=']'||i>1)){//Halt if there's no matching `]`
							fprintf(stderr,"Error at locaton %u: Missing ']'!\n",debug);
							return 2;
						}else if(code[codeLocation]=='[')//(Nested `[`)
							i++;
						else if(code[codeLocation]==']')//(Matching `]`)
							i--;
					codeLocation--;//Decrease the pointer indicator value
				}
				break;
			case ']'://Go back to a matching `[` if the current pointer value is non-zero
				if(pointer[pointerLocation]){
					auto debug=codeLocation;//Save current pointer location for debug purposes
					codeLocation--;//Decrease the pointer indicator value
					for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
						if(!codeLocation&&(code[codeLocation]!='['||i>1)){//Halt if there's no matching `[`
							fprintf(stderr,"Error at locaton %u: Missing '['!\n",debug);
							return 2;
						}else if(code[codeLocation]=='[')//(Nested `[`)
							i--;
						else if(code[codeLocation]==']')//(Matching `]`)
							i++;
					codeLocation++;//Increase the pointer indicator value
				}
		}
	return 0;
}
