#include<conio.h>
#include<deque>
#include<cstdio>
#include<string>
int main(int argc,char** argv){
	std::string code;
	std::deque<char>pointer={0};
	FILE *file=fopen(argv[1],"r");
	for(auto command=getc(file);!feof(file);command=getc(file))
		switch(command){
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
	fclose(file);
	for(size_t codeLocation=0,pointerLocation=0;codeLocation<code.size();codeLocation++)
		switch(code[codeLocation]){
			case '>':
				if(++pointerLocation==pointer.size())
					pointer.push_back(0);
				break;
			case '<':
				if(!pointerLocation--)
					pointer.push_front(0),pointerLocation++;
				break;
			case '+':
				pointer[pointerLocation]++;
				break;
			case '-':
				pointer[pointerLocation]--;
				break;
			case '.':
				putchar(pointer[pointerLocation]);
				break;
			case ',':
				pointer[pointerLocation]=getche();
				break;
			case '[':
				if(!pointer[pointerLocation]){
					auto debug=codeLocation;
					codeLocation++;
					for(unsigned long long i=1;i;codeLocation++)
						if(codeLocation+1==code.size()&&(code[codeLocation]!=']'||i>1)){
							fprintf(stderr,"Error at locaton %u: Missing ']'!\n",debug);
							return 1;
						}else if(code[codeLocation]=='[')
							i++;
						else if(code[codeLocation]==']')
							i--;
					codeLocation--;
				}
				break;
			case ']':
				if(pointer[pointerLocation]){
					auto debug=codeLocation;
					codeLocation--;
					for(unsigned long long i=1;i;codeLocation--)
						if(!codeLocation&&(code[codeLocation]!='['||i>1)){
							fprintf(stderr,"Error at locaton %u: Missing '['!\n",debug);
							return 1;
						}else if(code[codeLocation]=='[')
							i--;
						else if(code[codeLocation]==']')
							i++;
					codeLocation++;
				}
		}
	return 0;
}
