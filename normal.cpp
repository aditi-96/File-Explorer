#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <string>
using namespace std;

#define ESC 27
#define CLR printf("%c[2J",ESC);	//clears the screen
#define setpos(x,y) printf("%c[%d;%dH",ESC,y,x);	//set cursor position to line y, col x
#define UP(n) printf("\033[%dA",n);
#define DOWN(n) printf("\033[%dB",n);
#define LEFT(n) printf("\033[%dD",n);
#define RIGHT(n) printf("\033[%dC",n);

extern struct termios initial_settings, new_settings;
extern uint curX, curY;
extern uint rows,cols;
extern uint flistSize;
extern uint DispInd;
extern string curDir;
// string curParent;
extern string root;
extern vector<string> fList;
extern stack<string> backStack;
extern stack<string> frontStack;
extern bool gotoFlag;

void lstDir(string path);
void dispDir();
void disRawMode();
void enRawMode();
int commandMode();
void enterDir();
void exCommand(string cmd);
int copyFile(string src, string dest);
void deleteDir(string src);
// extern FILE *fileptr;
void enRawMode(){
	tcgetattr(STDIN_FILENO, &initial_settings);
	new_settings = initial_settings;
	new_settings.c_lflag &= ~(ICANON|ECHO);
	// new_settings.c_iflag &= ~(ICRNL);
	new_settings.c_cc[VMIN] = 1;
	new_settings.c_cc[VTIME] = 0;
	// new_settings.c_lflag &= ~ISIG;	//CTRL+C/CTRL+Z won't be used to terminate the program
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings) != 0) {
		fprintf(stderr,"could not set attributes\n");
	}
	else{
		char c;
		while(1){
			if(gotoFlag) c=':';
			else c=cin.get();
			switch(c){
				case 10:						//Enter is detected
				   // fprintf(fileptr,"Enter: %s %d\n",curDir.c_str(), DispInd);
					enterDir();
					break;

				case ESC:						//escape sequence(^[) is detected
					c=cin.get();
					if(c==91){
						c=cin.get();
						switch(c){
							case 'A':
								if(curY>0){
									UP(1);
									curY-=1;
								}
								break;
							case 'B':
								if(curY<min(flistSize,rows)-1){
									DOWN(1);
									curY+=1;
								}
								break;
							case 'C':
								if(!frontStack.empty()){
									backStack.push(curDir);
									curDir=frontStack.top();
									frontStack.pop();
									lstDir(curDir);
								}
								break;
							case 'D':
								if(!backStack.empty()){
									frontStack.push(curDir);
									curDir=backStack.top();
									backStack.pop();
									lstDir(curDir);
								}
								break;
							default: break;
						}
					}
					else continue;
					break;
				case 'k':			//scroll up
					if(flistSize>rows){
						DispInd=max(0,(int)DispInd-(int)rows);
						// setpos(0,rows+2);
						// printf("%d", DispInd);
						// setpos(curX,curY);
					    // fprintf(fptr,"scroll up ---> DI: %d  rows: %d  fls: %d\n", DispInd, rows, flistSize);
						dispDir();
					}
					break;
				case 'l':			//scroll down
					if(flistSize>(DispInd+rows)){
						DispInd=min((int)flistSize-(int)rows, (int)DispInd+(int)rows);
						// setpos(0,rows+2);
						// printf("%d", DispInd);
						// setpos(curX,curY);
					    // fprintf(fptr,"scroll down ---> DI: %d  rows: %d  fls: %d\n", DispInd, rows, flistSize);
						dispDir();
					}
					break;
				case 'h':			//go to home directory
						backStack.push(curDir);
						while(!frontStack.empty()){
							frontStack.pop();
						}
						curDir=root;
						lstDir(curDir);
					break;
				case 127:				//Backspace is pressed
					if(curDir!="/"){
						backStack.push(curDir);
						while(!frontStack.empty()){
							frontStack.pop();
						}
						int n=curDir.size()-1;
						while(curDir[n]!='/'){
							n--;
						}
						curDir=curDir.substr(0,n);
						if(curDir=="") curDir="/";
						lstDir(curDir);						
					}
					else curDir="/";
					break;
				case ':':
					int ret;							//Enter command mode
					curX=0;
					curY=rows+2;
					setpos(0,rows+2);
					commandMode();
					lstDir(curDir);
					// ret = commandMode();
					// if(ret==0) {
					// 	lstDir(curDir);
					// }
					break;
				default: break;
			}
		}

	}	
} 

void enterDir(){
	struct stat fst;
	// fprintf(fileptr,"Enter: %s %d %d\n",curDir.c_str(), DispInd, curY);
	if(fList[DispInd+curY]=="."){
		DispInd=0;
		setpos(curX,0);
		return;
	}
	else if(fList[DispInd+curY]==".." && curDir!="/"){
        backStack.push(curDir);
        while (!frontStack.empty())
            frontStack.pop();
		int n=curDir.size()-1;
		while(curDir[n]!='/'){
			n--;
		}
		curDir=curDir.substr(0,n);
		if(curDir=="") curDir="/";
		// setpos(0,rows+2);
		// printf("%s",curDir.c_str());
		// setpos(curX,curY);						
		lstDir(curDir);
	}
	else if(curDir=="/"){
		if(fList[DispInd+curY]==".."){
			DispInd=0;
			// setpos(curX,0);
			lstDir(curDir);
		}
		else{
			if(stat((curDir+fList[DispInd+curY]).c_str(),&fst)==0){
				if(S_ISDIR(fst.st_mode) && (fst.st_mode & S_IRWXG)){
			        backStack.push(curDir);
			        while (!frontStack.empty())
			            frontStack.pop();
					// CLR;
					curDir=curDir+fList[curY];
					// fprintf(fileptr, "%s\n", curDir.c_str());
					// setpos(0,rows+2);
					// printf("%s",curDir.c_str());
					// setpos(curX,curY);
					lstDir(curDir);					
				}
				else if(S_ISREG(fst.st_mode) && (fst.st_mode & S_IRGRP)){
					pid_t pid;
					if(pid=fork()==0){
						close(STDERR_FILENO);
						execlp("xdg-open","xdg-open",(curDir+'/'+fList[DispInd+curY]).c_str(),NULL);
						exit(0);
					}
				}										
			}
			else return;
		} 
	}
	else{
		if(stat((curDir+'/'+fList[DispInd+curY]).c_str(),&fst)==0){
			if(S_ISDIR(fst.st_mode) && (fst.st_mode & S_IRWXG)){
		        backStack.push(curDir);
		        while (!frontStack.empty())
		            frontStack.pop();
				// CLR;
				curDir=curDir+'/'+fList[DispInd+curY];
			    // fprintf(fileptr,"enter a dir ---> curY: %d  DI: %d  curDir: %s\n", curY, DispInd, curDir.c_str());
				// setpos(0,rows+2);
				// printf("%s",curDir.c_str());
				// setpos(curX,curY);
				lstDir(curDir);					
			}
			else if(S_ISREG(fst.st_mode) && (fst.st_mode & S_IRGRP)){
				pid_t pid;
				if(pid=fork()==0){
					close(STDERR_FILENO);
					execlp("xdg-open","xdg-open",(curDir+'/'+fList[DispInd+curY]).c_str(),NULL);
					exit(0);
				}
			}																
		}
		else return;
	}	
}