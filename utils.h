#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include<windows.h>
#include "compat.h"
#include "keys.h"

void printBorder(){
	int width, height;

	char tb = '*', lr = tb;
	char a = tb, b = a, c = a, d = a;
	// tb = char(196); lr = char(179);
	// a = char(218); b = char(191); c = char(217); d = char(192);

	if(GetWH(width, height)){
		int x=0,y=0; int goup = 0, goleft = 0;

		while(x++<width-2){
			gotoxy(x, 0); printf("%c", tb); // top
			gotoxy(x, height-1); printf("%c", tb); // bottom
		}
		while(y++<height-2){
			gotoxy(0, y); printf("%c", lr); // left
			gotoxy(width-1, y); printf("%c", lr); // right
		}
	}
	gotoxy(0,0); putchar(a);
	gotoxy(width-1, 0); putchar(b);
	//gotoxy(width, height); putchar(c);
	gotoxy(0,height-1); putchar(d);
}


void printHr(int iy, char xx=char(196)){
	int width, height;
	if(GetWH(width, height)){
		int x=0, y=iy;
		for(int x=0; x<width; x++){
			gotoxy(x, y);
			printf("%c", xx);
		}
	}
}

void hr(int x, int y, int c, int color=8){
   gotoxy(x, y);
   SetColorAndBackground(color, 0);
   int i=0;
   while(i++<c){
      printf("%c", char(196));
   }
   SetColorAndBackground(15, 0);
}

void printHeader(int stx, char x[50]){
   SetColorAndBackground(15, 12);
   int hw = 40;            // header width - this area gets cleared/rewritten
	int len = strlen(x);
   int pad = (hw-len)/2;   // padding for each side
	stx = stx+12;
	gotoxy(stx-(hw/2), 1);
	printf("%*s", pad, " ");
	printf("%s", x);
	printf("%*s", pad, " ");
	//printHr(2, '_');
   SetColorAndBackground(15, 0);

   char title[80];
   strcpy(title, "Events Manager - ");
   strcat(title, x);
   SetConsoleTitle(title);
}

void clearBody(int x, int w, int y=3, int lines=-1, char c=' '){
   int width, height;
   GetWH(width, height);

   if(lines==-1)
      lines = height-y-2;

	int count = y+lines;
	while(y < count){
		gotoxy(x, y);

		// printing one char first then the rest of the characters
		// fixes an issue with clearing background color in windows cmd
		//printf("%c", c);
		printf("%*c", w, c);
		y++;
	}
}

void getltm(tm *&ltm, int yy, int mm, int dd, int hh=0, int min=0, int sec=0){
   time_t now = time(0);
   ltm = localtime(&now);
   ltm->tm_year = yy-1900;
   ltm->tm_mon  = mm-1;
   ltm->tm_mday = dd;
   ltm->tm_hour = hh;
   ltm->tm_min  = min;
   ltm->tm_sec  = 0;
}

int takeInput(char* out, int hide=0, int alphaNum=0){
   int i=0, ch;

   while(ch=getch()){
      if(ch==KEY_ESC)
         return -1;
      else if(ch==KEY_ENTER)
         break;
      else if(ch==KEY_BACKSPACE){
         if(i>0){
            i--;
            printf("%c %c", ch, ch);
         }
      }else if(ch==0 || ch==224){
         getch();
         continue;
      }else if(!alphaNum || isalnum(ch)){
         out[i] = ch;
         printf("%c", hide?'*':ch);
         i++;
      }
   }
   out[i] = '\0';
   return i;
}
#endif // UTILS_H_INCLUDED
