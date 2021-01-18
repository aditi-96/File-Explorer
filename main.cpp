#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
using namespace std;

#define ESC 27
#define CLR printf("%c[2J",ESC);	//clears the screen
#define setpos(x,y) printf("%c[%d;%dH",ESC,y,x);	//set cursor position to line y, col x
#define UP(n) printf("\033[%dA",n);
#define DOWN(n) printf("\033[%dB",n);
#define LEFT(n) printf("\033[%dD",n);
#define RIGHT(n) printf("\033[%dC",n);

void lstDir(string path);
void dispDir();
void disRawMode();
void enRawMode();
void commandMode();

struct termios initial_settings, new_settings;
struct winsize win;
vector<string> fList;
stack<string> backStack;
stack<string> frontStack;
uint curX, curY;
uint rows,cols;
uint flistSize;
uint DispInd;
string curDir;
// string curParent;
// string root="/home/aditi/Downloads";
string root="";
// FILE *fileptr;

int main(){
	// fileptr = fopen("/home/aditi/Desktop/foobar.txt","w+");
	char cwd[FILENAME_MAX];
	root=getcwd(cwd, FILENAME_MAX);

	printf("%c[?1049h",ESC);		//switch to alternate screen

	/* Get Terminal Size*/
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&win);
	rows=win.ws_row-3;
	cols=win.ws_col;

	struct stat fst;
	curDir=root;
	// curParent=root;

	lstDir(root);
	enRawMode();		//Non-canonical Mode

	// fclose(fileptr);
	disRawMode();
	return 0;
}