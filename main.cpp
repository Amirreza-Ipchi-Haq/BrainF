#include<conio.h>
#include<deque>
#include<fstream>
#include<iostream>
int main(int argc,char** argv){
	std::string code;
	std::deque<char>pointer={0};
	std::ifstream file(argv[1]);
	for(char command;file.get(command);)
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
	file.close();
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
				std::cout<<pointer[pointerLocation];
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
							std::cerr<<"Error at character "<<debug<<": Missing ']'!";
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
							std::cerr<<"Error at character "<<debug<<": Missing '['!";
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
