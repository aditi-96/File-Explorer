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

extern uint curX, curY;

extern string curDir;
extern struct termios initial_settings, new_settings;
extern stack<string> backStack;
extern stack<string> frontStack;
extern string root;
// extern FILE *fileptr;
vector<string> cmdArg;
int size;
bool gotoFlag;
void exCommand(string cmd);
int copyFile(string src, string dest);
void deleteDir(string src);
extern void lstDir(const string path);

void disRawMode(){
	if(tcsetattr(fileno(stdin), TCSANOW, &initial_settings) != 0) {
		fprintf(stderr,"could not set attributes\n");
	}		
}

string absPath(string path){
	string abs_path="";
	if(path[path.size()-1]=='/') path=path.substr(0,path.size()-1);
	if(path[0]=='/'){
		abs_path = root + path;
	}
	else if(path[0]=='.' && path[1]=='/'){
		if(curDir!="/") abs_path = curDir + path.substr(1,path.size()-1);
		else abs_path = path.substr(1,path.size()-1);
	}
	else if(path[0]=='~'){
		abs_path = root + path.substr(1,path.size()-1);
	}
	else abs_path = curDir + '/' + path;
	return abs_path;
}

bool isDirectory(string path){
	struct stat dst;
	if(stat(path.c_str(),&dst)!=0){
		perror(path.c_str());
		return false;
	}
	if(S_ISDIR(dst.st_mode)){
		return true;
	}
	return false;
}

int copyDir(string src, string dest){
	// cout<<"1: "<<dest<<endl;
	struct dirent *fptr;
	struct stat fst;
	DIR *dcur = opendir(src.c_str());
	// cout<<"-1"<<endl;
	if(dcur==NULL){
		// cout<<"0"<<endl;
		perror("");
		return -1;
	}

	string srcFile;
	while((fptr=readdir(dcur))!=NULL){
		// cout<<"fname: "<<fptr->d_name<<endl;
		if(strcmp(fptr->d_name,".")==0 || strcmp(fptr->d_name,"..")==0) continue;
		else {
			srcFile=src+'/'+fptr->d_name;
			// cout<<"src,srcfile "<<src<<" "<<srcFile<<endl;
			if(stat(srcFile.c_str(),&fst)!=0){
				// cout<<"1"<<endl;
				perror("");
				return -1;
			}
			else{
				// cout<<"2"<<endl;
				string destDir;
				destDir=dest+'/'+fptr->d_name;
				// cout<<"dd: "<<destDir<<" "<<dest<<endl;
				if(isDirectory(srcFile)){
					// cout<<"3"<<endl;
					if(mkdir(destDir.c_str(),fst.st_mode)!=0){
						// cout<<"4"<<endl;
						perror(destDir.c_str());
						return -1;
					}
					chown(destDir.c_str(), fst.st_uid, fst.st_gid);
					copyDir(srcFile,destDir);
				}
				else copyFile(srcFile,destDir);
			}			
		}
	}
	closedir(dcur);
	return 0;	
}

int copyFile(string src, string dest){
	struct stat fst;
	FILE *sf, *df;
	char c;

	sf = fopen(src.c_str(),"r");
	if(sf==NULL){
		// cout<<"cf:1"<<endl;
		// perror("");
		return -1;
	}

	df = fopen(dest.c_str(),"w+");
	if(df==NULL){
		// perror("");
		// cout<<"cf:2"<<endl;
		return -1;
	}

	while(!feof(sf)){
		c=getc(sf);
		putc(c,df);
	}

	// cout<<"cf:3"<<endl;
	// if(stat(src.c_str(),&fst)!=0) perror("");
	if(stat(src.c_str(),&fst)==0){
		chmod(dest.c_str(), fst.st_mode);
		chown(dest.c_str(), fst.st_uid, fst.st_gid);		
	}
	// }
	fclose(sf);
	fclose(df);
	return 0;
}

bool recSearch(string name, string listItem){
	queue<string> searchList;
	searchList.push(listItem);
    struct dirent* dirItem;
 	DIR *searchDir;
 	string path;
	while(!searchList.empty()){
		listItem=searchList.front();
		searchList.pop();
		searchDir=opendir(listItem.c_str());
		if(searchDir==NULL) continue;
		while((dirItem=readdir(searchDir))!=NULL){
			string dname=dirItem->d_name;
			if(dname==name) return true;
			if(dname!="." && dname!=".."){
				if(listItem=="/") path=listItem+dname;
				else path=listItem+'/'+dname;
				// cout<<path<<endl;
				searchList.push(path);						
			}
		}
		closedir(searchDir);
	}
	return false;
}

void commandMode(){
	string cmd="";
	char c;
	gotoFlag=false;
	// stringstream ss;
	while(1){
		cmd="";
		// ss.str(cmd);
		fflush(0);
        printf("\x1b[0K");
		printf(":\n");
		curX=0;
		while(1){
			c=cin.get();			
			switch(c){
				case 27:						//inputs starting with ESC
					c=cin.get();
					// printf("%c\n", c);
					if(c!=91) return;
					// else{
					// 	c=cin.get();
					// 	if(c==0) return 0;			//ESC is pressed
					// }
					fflush(0);
					break;
				case 10:
					// printf("\n");
					if(cmd==""){
						continue;
					}
					exCommand(cmd);
					if(gotoFlag) return;
					cmd="";
					break;
				case 127:
					if(curX>0) {
						curX--;
						LEFT(1);
	                    printf("\x1b[0K");
	                    cmd=cmd.substr(0,cmd.size()-1);
					}
					break;
				default:
					printf("%c",c);
					curX++;
					cmd+=c;
					break;
			}			
		}
	}
}

void copy(){
	string src;
	string dest;
	string fname="";
	string dloc;
	int ret;

	if(size<3) printf("Invalid Command\n");			//Command Invalid
	else {											//Command Valid
		struct stat fst;
		dest=absPath(cmdArg[size-1]);
		printf("%s\n", dest.c_str());
		for(int i=1; i<size-1; i++){
			src=absPath(cmdArg[i]);
			if(stat(src.c_str(),&fst)!=0) perror("");
			// printf("%s\n", src.c_str());

			int n=src.size();
			while(src[n-1]!='/'){
				n--;
			}
			fname=src.substr(n);
			// cout<<fname<<endl;
			dloc=dest+'/'+fname;

			if(isDirectory(src)){
				if(mkdir(dloc.c_str(),fst.st_mode)!=0){
					perror("");
					return;
				}
				chown(dloc.c_str(), fst.st_uid, fst.st_gid);
				// cout<<"Directory made"<<endl;
				ret=copyDir(src,dloc);
			}
			else{
				ret=copyFile(src,dloc);
			}
		}
	}
}

void move(){
	string src;
	copy();
	for(int i=1; i<size-1; i++){
		src=cmdArg[i];
		src=absPath(src);
		if(isDirectory(src)){
			deleteDir(src);
		}
		else remove(src.c_str());
	}
}

void deleteDir(string src){
	struct dirent *fptr;
	// struct stat fst;
	DIR *dcur = opendir(src.c_str());
	// cout<<"-1"<<endl;
	if(dcur==NULL){
		// cout<<"0"<<endl;
		perror("");
	}
	string srcFile;
	while((fptr=readdir(dcur))!=NULL){
		// cout<<"fname: "<<fptr->d_name<<endl;
		if(strcmp(fptr->d_name,".")==0 || strcmp(fptr->d_name,"..")==0) continue;
		else {
			srcFile=src+'/'+fptr->d_name;
			if(isDirectory(srcFile)){
				deleteDir(srcFile);
			}
			else remove(srcFile.c_str());
		}
	}
	closedir(dcur);
	remove(src.c_str());
}

void exCommand(string cmd){
	printf("\n");
	stringstream ss(cmd);
	string temp;
	cmdArg.clear();
	while(ss>>temp){
		cmdArg.push_back(temp);
	}
	size=cmdArg.size();
	// cout<<size<<endl;
	// for(int i=0; i<cmdArg.size(); i++){
	// 	cout<<cmdArg[i]<<" ";
	// }
	if(cmdArg[0] == "copy"){
		copy();
	}
	else if(cmdArg[0] == "move"){
		move();
	}

	else if(cmdArg[0] == "rename"){
		if (size!=3){
			cout<<"Invalid Command"<<endl;
		}
		else{
			string oldPath=absPath(cmdArg[1]);
			string newPath=absPath(cmdArg[2]);
			if(rename(oldPath.c_str(),newPath.c_str())!=0){
				perror("");	
			}
		}
		return;
	}

	else if(cmdArg[0] == "create_file"){
		if(size<3) return;
		FILE *fp;
		string destPath;
		string newFile;
		struct stat fst;
		destPath=absPath(cmdArg[size-1]);
		if(!isDirectory(destPath)){
			cout<<destPath<<": Not a Directory!"<<endl;
			return;
		}

		if(stat(destPath.c_str(),&fst)!=0) perror("");
		for(int i=1; i<size-1; i++){
			newFile=destPath+'/'+cmdArg[i];
			fp=fopen(newFile.c_str(),"w+");
			chmod(newFile.c_str(), 0755);
			chown(newFile.c_str(), fst.st_uid, fst.st_gid);
			if(!fp) perror("");
			else cout<<"File "<<cmdArg[i]<<" Created Successfully!"<<endl;
			fclose(fp);
		}
	}

	else if(cmdArg[0] == "create_dir"){
		if(size<3) return;
		string destPath;
		string destDir;
		struct stat fst;
		destPath=absPath(cmdArg[size-1]);
		if(!isDirectory(destPath)){
			cout<<destPath<<": Not a Directory!"<<endl;
			return;
		}

		if(stat(destPath.c_str(),&fst)!=0) perror("");
		for(int i=1; i<size-1; i++){
			destDir=destPath+'/'+cmdArg[i];
			if(mkdir(destDir.c_str(),0755)!=0){
				perror("");
			}
			else{
				chown(destDir.c_str(), fst.st_uid, fst.st_gid);				
				cout<<"Directory "<<cmdArg[i]<<" Created Successfully!"<<endl;
			}
		}
	}

	else if(cmdArg[0] == "delete_file"){
		string src;
		if(size<2) return;
		for(int i=1; i<size; i++){
			src=cmdArg[i];
			src=absPath(src);
			if(isDirectory(src)){
				cout<<src<<": is a Directory!"<<endl;
			}
			else remove(src.c_str());
		}
	}

	else if(cmdArg[0] == "delete_dir"){
		string src;
		if(size<2) return;
		for(int i=1; i<size; i++){
			src=cmdArg[i];
			src=absPath(src);
			if(!isDirectory(src)){
				cout<<src<<": is a File!"<<endl;
			}
			else deleteDir(src.c_str());
		}
	}

	else if(cmdArg[0] == "goto"){
		if(size!=2) return;
		string path;
		path=absPath(cmdArg[1]);
		if(!isDirectory(path)) return;
		else {
			backStack.push(curDir);
			curDir=path;
			gotoFlag=true;
		}
	}

	else if(cmdArg[0] == "search"){
		if(size!=2) return;
		bool ret;
		ret=recSearch(cmdArg[1], curDir);
		if(ret) printf("Yes\n");
		else printf("No\n");
	}

	else{
		fflush(0);
		return;
	}
}
