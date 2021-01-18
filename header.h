#define mvcrs '\033'
#define esc '\x1b'
#define cls printf("%c[2J",esc);	//clears the screen
#define setpos(x,y) printf("%c[%d;%dH",mvcrs,x,y);	//set cursor position to x,y
