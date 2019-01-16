#ifndef COMPAT_H_INCLUDED
#define COMPAT_H_INCLUDED

#include<windows.h>

void gotoxy (int x, int y){
   COORD xy = {x, y};
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
}

int GetWH(int &W, int &H){
	W = H = -1;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return 0;

	W = csbi.srWindow.Right-csbi.srWindow.Left + 1;
	H = csbi.srWindow.Bottom-csbi.srWindow.Top + 1;
	return 1;
}

void SetWH(int W, int H){
   HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

   if (h == INVALID_HANDLE_VALUE)
      return;  // error

   COORD largestSize = GetLargestConsoleWindowSize(h);
   if (W > largestSize.X || H > largestSize.Y)
      return;

   COORD size = {W, H};
   SMALL_RECT info = {0, 0, W-1, H-1};

   if(!SetConsoleScreenBufferSize(h, size))
      return; // error

   if (!SetConsoleWindowInfo(h, TRUE, &info))
      return; // error
}

int wherex(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.X;
}

int wherey(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}

// set text color
void SetColor(int ForgC){
   WORD wColor;
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
   }
}

// reset colors to default
void ClearColor(){
   SetColor(15);
}

// clear console and set colors
void ClearConsoleToColors(int ForgC, int BackC){
   WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   COORD coord = {0, 0};
   DWORD count;

   CONSOLE_SCREEN_BUFFER_INFO csbi;

   SetConsoleTextAttribute(hStdOut, wColor);
   if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
      FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
      FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
      SetConsoleCursorPosition(hStdOut, coord);
   }
}

// set text color and background
void SetColorAndBackground(int ForgC, int BackC){
   WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);;
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
   return;
}

#endif // COMPAT_H_INCLUDED
