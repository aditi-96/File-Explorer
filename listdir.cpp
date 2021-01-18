#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <string>
#include <bits/stdc++.h>
#include <unistd.h>
#include <pwd.h>

using namespace std;

#define ESC 27	//'\033' '\x1B'
#define CLR printf("%c[2J",ESC);	//clears the screen
#define setpos(x,y) printf("%c[%d;%dH",ESC,y,x);	//set cursor position to x,y
#define UP(n) printf("\033[%dA",n);
#define DOWN(n) printf("\033[%dB",n);
#define LEFT(n) printf("\033[%dD",n);
#define RIGHT(n) printf("\033[%dC",n);

extern vector<string> fList;
extern uint rows,cols;
extern uint curX,curY;
extern uint flistSize;
extern uint DispInd;
extern stack<string> backStack;
extern stack<string> frontStack;
extern string root;
extern string curDir;
// extern string curParent;
void dispDir();

int lstDir(const string path){
	struct dirent *fptr;
	DIR *dcur = opendir(path.c_str());
	if(dcur==NULL){
		// setpos(0,rows+2);
		// printf("Cannot open the requested directory\n");
		// setpos(curX,curY);
		return -1;
	} 
	fList.clear();
	// string curpath;
	while((fptr=readdir(dcur))!=NULL){
		// curpath=path+'/'+fptr->d_name;
		fList.push_back(fptr->d_name);
	}
	sort(fList.begin(),fList.end());
	closedir(dcur);
	flistSize=fList.size();
	DispInd=0;
	dispDir();
	return 0;
}

void dispDir(){
	CLR;
	setpos(0,0);
	curX=0; curY=0;	
	struct stat fst; 
	struct passwd *pwd;
	string perm;
	string user;
	string group;
	string modTime;
	long long spaceOcc;
	int numY;
	char unit;

	// if(((int)flistSize - (int)DispInd)<rows)
	// 	DispInd=max((int)flistSize-(int)rows,0);

	numY=min(rows+DispInd,flistSize);

	string curpath;
    // fprintf(fptr,"dispDir ---> DI: %d  numY: %d\n", DispInd, numY);
	for(int i=DispInd; i<numY; i++){
		curX=0;
		perm="";
		curpath=curDir+'/'+fList[i];
		stat(curpath.c_str(),&fst);

		perm+=S_ISDIR(fst.st_mode)? 'd' : '-';
		perm+=(fst.st_mode & S_IRUSR) ? 'r' : '-';
		perm+=(fst.st_mode & S_IWUSR) ? 'w' : '-';
		perm+=(fst.st_mode & S_IXUSR) ? 'x' : '-';
		perm+=(fst.st_mode & S_IRGRP) ? 'r' : '-';
		perm+=(fst.st_mode & S_IWGRP) ? 'w' : '-';
		perm+=(fst.st_mode & S_IXGRP) ? 'x' : '-';
		perm+=(fst.st_mode & S_IROTH) ? 'r' : '-';
		perm+=(fst.st_mode & S_IWOTH) ? 'w' : '-';
		perm+=(fst.st_mode & S_IXOTH) ? 'x' : '-';

		curX += printf("%s",perm.c_str());

		pwd=getpwuid(fst.st_uid);
		if(pwd!=NULL) user=pwd->pw_name;
		curX += printf(" %10s ", user.c_str());

		pwd=getpwuid(fst.st_gid);
		if(pwd!=NULL) group=pwd->pw_name;
		curX += printf(" %10s ", group.c_str());
		
		spaceOcc = fst.st_size;
		if (spaceOcc >= 1<<30){
			spaceOcc/=(1<<30);
			unit='G';
		}
		else if (spaceOcc >= 1<<20){
			spaceOcc/=(1<<20);
			unit='M';
		}
		else if (spaceOcc >= 1<<10){
			spaceOcc/=(1<<10);
			unit='K';
		}
		else unit='B';
		curX +=printf("%4lld%c ", spaceOcc, unit);

		modTime = (string)ctime(&fst.st_mtime);
		modTime = modTime.substr(0,modTime.size()-1);
		curX += printf(" %-13s ", modTime.c_str());

	    if (perm[0]=='d')
	        printf("%c[33m", 27);
		printf("%-30s\n", fList[i].c_str());
	    if (perm[0]=='d')
	        printf("%c[0m", 27);
	}
	curX+=1;
	setpos(curX,curY);
	return;
}
