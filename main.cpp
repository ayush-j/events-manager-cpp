/*
Name: Events & Task Manager Project
Date: Feb 2016
Author: Ayush
Description:
  A Google Calendar-like event manager written in C++.
  Code in compat.h is mostly Windows only so this does not work on Linux.
  Compile in Visual Studio / Codeblocks (g++)
*/

#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<iostream>
#include<time.h>
#include<string.h>
#include "keys.h"
using namespace std;

int width, height;
int stx;

#include "compat.h"
#include "utils.h"
#include "events.h"
#include "users.h"
Events events;
Users users;
char activeUser[20] = "-";

#include "menu.h"
#include "calendar.h"

// schtasks /Create /F /SC ONCE /SD "07/02/2016" /ST "06:55" /TN "OH YEAH!" /TR "c:\windows\System32\cmd.exe /k echo YO"

void centerText(char* text){
   gotoxy(width/2 - strlen(text)/2, wherey());
   printf(text);
}

void drawHeadFoot(){
   // top bar
   SetColorAndBackground(15, 12);
   gotoxy(0,0);
   printf("%*s", width, " ");
   gotoxy(0,1);
   printf("%*s", width, " ");

   // bottom (status) bar
   SetColorAndBackground(7, 0);
   gotoxy(0, height-2);
   printHr(height-2, '_');
   gotoxy(0, height-1);
   SetColorAndBackground(8, 8);
   printf("%*s", width-1, " ");
   SetColorAndBackground(15, 0);
}

void printStatusMsg(char *msg){
   int prev_x = wherex(), prev_y = wherey();
   gotoxy(0, height-1);
   SetColorAndBackground(8, 8);
   printf("%*s", width-1, " ");

   SetColorAndBackground(15, 8);
   gotoxy(2, height-1);
   printf(msg);
   SetColorAndBackground(15, 0);
   gotoxy(prev_x, prev_y);
}

void showEvents(int, int);
void showEvents2(int yy, int mm, int x, int y);
void showCalendar(Date&, int, int, int=0, int=1);
void addEvent(int, int);
void viewEvent(long, int, int);
void editEvent(long, int, long);


void welcomescreen(){
   gotoxy(0, 0);
   printf("\n\n\n\n");
   SetColor(10);
   centerText("EVENTS AND TASK MANAGER\n");
   ClearColor();
   centerText("PROJECT WORK\n");
   centerText("COMPUTER SCIENCE with C++\n");
   centerText("(Session 2015-16)\n");
   centerText("____________________________________________________\n");
   SetColor(14);
   printf("\n");
   centerText("INTRODUCTION:\n");
   ClearColor();
   centerText("This is an Event Management software project\n");
   centerText("written in C++ which allows one to add,\n");
   centerText("modify events and view any upcoming ones.\n");
   centerText("A calendar view is available to get an overview\n");
   centerText("of upcoming or past events.\n");
   printf("\n");
   centerText("Multiple user profiles are supported so that\n");
   centerText("different users can manage their own separate\n");
   centerText("events.\n");
   centerText("____________________________________________________\n");
   centerText("\n");
   centerText("                             Created by Ayush Jindal\n");

   //gotoxy(12, height-3);
   //printf("Press any key to continue.");

   printStatusMsg("Press any key to continue.");
   int ch = getch();
   if(ch==0 || ch==224)getch();
}


int login(){
   int i=0, ex;
   User u;
   Menu m, m2;
   int changed=1;

   m2.addItem(' ', "Username: ");
   m2.addItem(' ', "Password: ");
   m2.nokey = 1;

   int choice, ch;
   char ch2[20] = "";
   int fpos = 0;
   while(1){
      if(changed){
         m = Menu();
         strcpy(m.title, "SELECT A USER");
         m.addItem('g', "Guest");
         while(users.fetch(u, fpos))
            m.addItem('*', u.name);
         m.addItem('n', "Create New User..");
         changed = 0;
      }
      printHeader(stx, "LOGIN");
      printStatusMsg("ESC Exit\tENTER Login\tUP/DOWN Navigate\tDEL Delete User");
      m.print(width/2-25, 4, 50);

      ex = 0;
      while(choice=getch()){
         if(choice==0 || choice==224){
            choice = getch();
            switch(choice){
               case KEY_UP: // up
                  m.highlight(-1); break;
               case KEY_DOWN: // down
                  m.highlight(1); break;
               case KEY_DELETE:
                  if(m.activeItem){
                     MenuItem *item;
                     if(m.getItemByNo(m.activeItem, item)){
                        choice = item->key;
                        if(choice != '*')
                           break;
                        m.select(m.activeItem);
                        printHeader(stx, "DELETE USER");
                        clearBody(width/2-25, 50, 4);

                        strcpy(m2.title, "DELETE USER");
                        m2.print(width/2-25, 4, 50);

                        gotoxy(width/2-10, 8);
                        printf(item->name);

                        pwinput2:
                        gotoxy(width/2-10, 10);
                        printf("%20s", "");
                        gotoxy(width/2-10, 10);

                        if(takeInput(ch2, 1)<=0){ ex=1; break; }


                        if(users.get(item->name, u) && strcmp(ch2, u.passwd) == 0){

                           gotoxy(width/2-23, 13);
                           printf("Are you sure you want to delete '%s'", u.name);
                           gotoxy(width/2-23, 14);
                           printf("and all the events associated with the user?");

                           SetColor(12);
                           gotoxy(width/2-20, 16);
                           centerText("Press DELETE to confirm.");
                           SetColor(15);

                           ch = getch();
                           if((ch==0 || ch==224) && getch()==KEY_DELETE){
                              strcpy(ch2, "events"); strcat(ch2, ".");
                              strcat(ch2, u.name); strcat(ch2, ".dat");
                              remove(ch2);

                              changed = 1;
                              users.del(u.name);

                              gotoxy(width/2-15, 18);
                              centerText("User Deleted.");
                              Sleep(2000);

                           }

                           ex=1;
                           break;
                        }

                        printStatusMsg("Incorrect password");
                        goto pwinput2;
                     }
                  }

            }
         }else
            break;

         if(ex==1) break;
      }

      bswitch:
      switch(choice){
         case 'g':
            m.select('g');
            strcpy(activeUser, "Guest");
            events = Events(activeUser);
            events.clear();
            return 1;
         case 'a':
            m.select('a');

            clearBody(width/2-25, 50, 4);

            break;


         case 'n':
            m.select('n');
            printHeader(stx, "NEW USER");
            clearBody(width/2-25, 50, 4);


            strcpy(m2.title, "ADD USER");
            m2.print(width/2-25, 4, 50);
            uinput:
            gotoxy(width/2-10, 8);
            printf("%12s", "");
            gotoxy(width/2-10, 8);
            i=takeInput(ch2, 0, 1);

            if(i<=0)break;

            if(users.get(ch2, u)){
               printStatusMsg("User already exists.");
               goto uinput;
            }
            strcpy(u.name, ch2);

            gotoxy(width/2-10, 8);
            printf("%-20s", u.name);
            pinput:
            gotoxy(width/2-10, 10);
            printf("%20s", "");
            gotoxy(width/2-10, 10);

            i = takeInput(ch2, 1);
            if(i<=0) break;
            if(i<6){
               printStatusMsg("Password must be at least 6 characters long.");
               goto pinput;
            }
            strcpy(u.passwd, ch2);
            strcpy(ch2, "");

            gotoxy(stx, 13);
            printf("Add this user? (y/n)");
            ch=getch();
            if(ch==0 || ch==224)ch=getch();
            if(ch == 'y'){
               users.add(u);
               u = User();
               gotoxy(stx-5, 16);
               printf("User added. You may now login.");
               printStatusMsg("Press any key to continue.");
               getch();
               if(ch==0 || ch==224)ch=getch();
               changed = 1;
            }
            break;
         case KEY_ENTER:
            if(m.activeItem){
               MenuItem *item;
               if(m.getItemByNo(m.activeItem, item)){
                  m.select(m.activeItem);
                  choice = item->key;
                  if(choice == '*'){
                     clearBody(width/2-25, 50, 4);

                     strcpy(m2.title, "LOGIN");
                     m2.print(width/2-25, 4, 50);

                     gotoxy(width/2-10, 8);
                     printf(item->name);

                     pwinput:
                     gotoxy(width/2-10, 10);
                     printf("%20s", "");
                     gotoxy(width/2-10, 10);

                     if(takeInput(ch2, 1)<=0) break;

                     if(users.get(item->name, u) && strcmp(ch2, u.passwd) == 0){
                        strcpy(activeUser, u.name);

                        events = Events(activeUser);
                        return 1;
                     }

                     printStatusMsg("Incorrect password");
                     goto pwinput;

                  }else
                     goto bswitch;
               }
            }
            break;

         case KEY_ESC:
            return 0;
      }
      clearBody(width/2-25, 50, 4, 15);
   }
}


int main(){
   int width_, height_;
   int choice;
   int noclear = 1, nowrite = 0;
   char ch;
   int ex;
   int prev_x, prev_y;
   int evCount;

   ClearConsoleToColors(15, 0);
   SetConsoleTitle("Events and Task Manager");

   SetWH(80, 25);

   // get windows's dimensions
   GetWH(width, height);
   stx = width/2-12;

   drawHeadFoot();

   welcomescreen();
   //clearBody(stx-20, 60);
   system("cls");
   CreateDirectory(".\\data", NULL);

   //SetWH(110, 50);
   SetWH(100, 46);

   // get windows's dimensions
   GetWH(width, height);
   stx = width/2-12;
   drawHeadFoot();


   SetColorAndBackground(15, 12);
   printHr(2, '_');
   SetColorAndBackground(15, 0);

   if(!login()) return 0;
   clearBody(stx-20, 60);
   strcpy(activeUser, "Guest");
   events = Events(activeUser);
   events.clear();

   SetColorAndBackground(8, 12);
   gotoxy(width-strlen(activeUser)-2, 1);
   printf(activeUser);
   SetColorAndBackground(15, 0);

   Date date, date_;
   int inc;

   Menu menu("MAIN MENU");
   menu.addItem('1', "Add New Event");
   menu.addItem('2', "View Events");
   menu.addItem('3', "View Overview");
   menu.addItem('c', "View Calendar");
   menu.addItem('4', "Reset (Delete All)");
   menu.addItem('7', "Import Events");
   menu.addItem('8', "Export Events");



   while(1){

      if(GetWH(width_, height_)){
         if(width_!=width || height_!=height){
            width = width_;
            height = height_;

   char title[20];
   sprintf(title, "WH: %d/%d", width, height);
   SetConsoleTitle(title);

            noclear = nowrite = 0;

            ClearConsoleToColors(15, 0);
            stx = width/2-12;

            drawHeadFoot();
            SetColorAndBackground(15, 12);
            printHr(2, '_');
            gotoxy(width-strlen(activeUser)-2, 1);
            printf(activeUser);
            SetColorAndBackground(15, 0);
         }
      }

/*
cout << endl << nowrite << "-" << noclear;
cout << "\n1:"<<getch();
cout << "\n2:"<<getch();
cout << "\n3:"<<getch();
cout << "\n4:"<<getch();
*/

      if(!nowrite){
         printHeader(stx, "EVENT MANAGER");
         if(!noclear)
            clearBody(3, width-6);

         menu.print(stx-10, 4, 22);
         prev_x = wherex(); prev_y = wherey();
         showCalendar(date, stx+20, 4, 1, 0);
         gotoxy(prev_x, prev_y);
      }else{
         //gotoxy(stx-10+8, 14); // position cursor after "Select: "
      }

      evCount = events.getCount();
      ex = noclear = nowrite = 0;

      printStatusMsg("ESC Exit\tUP/DOWN Navigate\t(n)ext month\t(p)revious month");

      while(choice=getch()){
         if(choice==0 || choice==224){
            choice = getch();

            switch(choice){
               case KEY_LEFT: // left
                  choice = 'p';
                  ex = 1;
                  break;
               case KEY_RIGHT: // right
                  choice = 'n';
                  ex = 1;
                  break;
               case KEY_UP: // up
                  menu.highlight(-1);
                  break;
               case KEY_DOWN: // down
                  menu.highlight(1);
                  break;
               default:
                  ex=1;
            }

            if(ex) break;
         }else
            break;
      }

      bswitch:
      switch(choice){
         case 'c':
            menu.select('c');
            printHeader(stx, "CALENDAR");
            clearBody(stx-10, 55);
            printStatusMsg("ESC Exit\t(n)ext\t(p)revious");

            do{

               if(height>=45){
                  date_ = Date(date.yy, 2, 1);
                  inc = 12;

                  date_.prevMonth(); Calendar(date_).print(stx-24, 4, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 4, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 4, 1);

                  date_.nextMonth(); Calendar(date_).print(stx-24, 14, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 14, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 14, 1);

                  date_.nextMonth(); Calendar(date_).print(stx-24, 24, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 24, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 24, 1);

                  date_.nextMonth(); Calendar(date_).print(stx-24, 34, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 34, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 34, 1);
               }else{
                  date_ = date;
                  inc = 6;
                  date_.prevMonth(); Calendar(date_).print(stx-24, 4, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 4, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 4, 1);

                  date_.nextMonth(); Calendar(date_).print(stx-24, 14, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+2, 14, 1);
                  date_.nextMonth(); Calendar(date_).print(stx+28, 14, 1);
               }

               choice = getch();
               if(choice==0 || choice==224) choice = getch();

               clearBody(stx-24, 76);

               if(choice == 'n' || choice==KEY_RIGHT){
                  date.nextMonth(inc);
               }else if(choice == 'p' || choice==KEY_LEFT){
                  date.prevMonth(inc);
               }else
                  break;
            }while(1);

            noclear = 1;
            break;
         case '3':
            menu.select('3');
            printHeader(stx, "CALENDAR");
            clearBody(stx-10, 55);
            printStatusMsg("ESC Exit\t(n)ext month\t(p)revious month");

            do{
               date_ = date;

               if(height>=45){
                  inc = 6;

                  date_.prevMonth(); showCalendar(date_, stx-24, 4, 1, 0);
                  date_.nextMonth(); showCalendar(date_, stx+2, 4, 1, 0);
                  date_.nextMonth(); showCalendar(date_, stx+28, 4, 1, 0);
                  date_.nextMonth(); showCalendar(date_, stx-24, 23, 1, 0);
                  date_.nextMonth(); showCalendar(date_, stx+2, 23, 1);
                  date_.nextMonth(); showCalendar(date_, stx+28, 23, 1, 0);
               }else{
                  inc = 3;
                  date_.prevMonth(); showCalendar(date_, stx-24, 4, 1, 0);
                  date_.nextMonth(); showCalendar(date_, stx, 4, 0, 0);
                  date_.nextMonth(); showCalendar(date_, stx+30, 4, 1, 0);
               }

               choice = getch();
               if(choice==0 || choice==224) choice = getch();

               clearBody(stx-24, 76);

               if(choice == 'n' || choice==KEY_RIGHT){
                  date.nextMonth(inc);
               }else if(choice == 'p' || choice==KEY_LEFT){
                  date.prevMonth(inc);
               }else
                  break;
            }while(1);

            noclear = 1;
            break;

         case '2':
            menu.select('2');
            printHeader(stx, "VIEW EVENTS");
            if(width<90){
               clearBody(stx-10, 55);
               showEvents(stx-20, 5);
            }else{
               clearBody(stx-10, 55);
               showCalendar(date, stx+30, 5, 1, 0);
               showEvents(stx-30, 5);
            }
            break;
         case '7':
            menu.select('7');
            printHeader(stx, "Import Events");
            clearBody(stx-10, 55);
            gotoxy(stx, 5);
            printf("Are you sure? (y/n)");
            ch=getch();
            if(ch==0 || ch==224)ch=getch();
            if(ch == 'y'){
               events.importFromFile("myfile.ics");
               gotoxy(stx, 7);
               printf("Events imported successfully.");
               gotoxy(stx, 12);
               SetColorAndBackground(8, 0);
               printf("Press any key to continue..");
               getch();
            }
            break;
         case '8':
            if(!evCount){
               menu.showError("No events to export.");
            }
            menu.select('8');

            if(!evCount){
               Sleep(600);
               break;
            }

            printHeader(stx, "Export Events");
            clearBody(stx-10, 55);
            gotoxy(stx, 5);
            printf("Are you sure? (y/n)");
            ch=getch();
            if(ch==0 || ch==224)ch=getch();
            if(ch == 'y'){
               events.exportToFile();
               gotoxy(stx, 7);
               printf("Events exported successfully.");
               gotoxy(stx, 12);
               SetColorAndBackground(8, 0);
               printf("Press any key to continue..");
               getch();
            }
            break;
         case '1':
            menu.select('1');
            printHeader(stx, "ADD EVENT");

            if(width<90){
               clearBody(stx-10, 55);
               addEvent(stx-15, 5);
            }else{
               clearBody(stx-10, 55);
               showCalendar(date, stx+30, 5, 1);
               addEvent(stx-30, 5);
            }
            break;
         case '4':
            if(!evCount){
               menu.showError("No events to delete.");
            }
            menu.select('4');

            if(!evCount){
               Sleep(600);
               break;
            }

            printHeader(stx, "Delete Events");
            clearBody(stx-10, 55);
            gotoxy(stx, 5);
            printf("Are you sure? (y/n)");
            ch=getch();
            if(ch==0 || ch==224)ch=getch();
            if(ch == 'y'){
               events.clear();
               gotoxy(stx, 7);
               printf("Events deleted.");
               gotoxy(stx, 12);
               SetColorAndBackground(8, 0);
               printf("Press any key to continue..");
               getch();
            }
            break;
         case 'n':
            prev_x = wherex(); prev_y = wherey();
            clearBody(stx+20, 25, 3, 18);

            date.nextMonth();

            showCalendar(date, stx+20, 4, 1, 0);
            gotoxy(prev_x, prev_y);
            noclear = 1;
            nowrite = 1;
            break;
         case 'p':
            prev_x = wherex(); prev_y = wherey();
            clearBody(stx+20, 25, 3, 18);

            date.prevMonth();

            showCalendar(date, stx+20, 4, 1, 0);
            gotoxy(prev_x, prev_y);
            noclear = 1;
            nowrite = 1;
            break;
         default:
            noclear=1; nowrite=1;

            if(choice==KEY_ESC){   // esc
               if(menu.activeItem){
                  menu.highlight(0);
                  break;
               }else
                  return 0;
            }
            if(choice==KEY_ENTER){   // enter
               if(menu.activeItem){
                  MenuItem *item_;
                  if(menu.getItemByNo(menu.activeItem, item_)){
                     choice = item_->key;
                     noclear=0; nowrite=0;
                     goto bswitch;
                  }
               }
               break;
            }
            if(choice==KEY_BACKSPACE){
               break;
            }

            printStatusMsg("Invalid input");
            Sleep(1200);

            break;
      } // switch end
   }
   return 0;
}

void printEventHeader(int x, int y){
   gotoxy(x, y);
   SetColorAndBackground(15, 9); printf("%20s", " ");
   SetColorAndBackground(15, 0); printf(" ");
   SetColorAndBackground(15, 9); printf("%35s", " ");
   SetColorAndBackground(15, 0);

   y++;

   gotoxy(x,y);
   SetColorAndBackground(15, 9); printf("%20s", "Name             ");
   SetColorAndBackground(15, 0); printf(" ");
   SetColorAndBackground(15, 9); printf("%35s", "Value           ");
   SetColorAndBackground(15, 0);

   y+=2;

   gotoxy(x, y); printf(" Event Name"); hr(x, y+1, 56);
   gotoxy(x,y+2); printf(" Event Type"); hr(x, y+3, 56);
   gotoxy(x,y+4); printf(" Event Date"); hr(x, y+5, 56);
   gotoxy(x,y+6); printf(" Event Time"); hr(x, y+7, 56);
   gotoxy(x,y+8); printf(" Priority"); hr(x, y+9, 56);
   gotoxy(x,y+10); printf(" Location"); hr(x, y+11, 56);
   gotoxy(x, y+12); printf(" Calendar");

}

void printEventValues(int x, int y, Event &evItem){
   char buff[30];
   tm *ltm;
   gotoxy(x+22, y); printf(evItem.note);

   gotoxy(x+22, y+2);
   printf(evItem.type);

   gotoxy(x+22, y+4);

   getltm(ltm, evItem.time.yy, evItem.time.mm, evItem.time.dd);
   strftime(buff, 30, "%d %b %Y", ltm);
   printf(buff);

   if(evItem.allday){
      gotoxy(x+22, y+6); printf("All Day");
   }else{
      getltm(ltm, evItem.time.yy, evItem.time.mm, evItem.time.dd, evItem.time.hh, evItem.time.min);
      strftime(buff, 30, "%I:%M %p", ltm);
      gotoxy(x+22, y+6); printf(buff);
   }

   gotoxy(x+22, y+8); printf(Priorities[evItem.priority]);
   gotoxy(x+22, y+10);
   if(strlen(evItem.location))
      printf(evItem.location);
   else{
      SetColor(8); printf("None"); SetColor(15);
   }
   gotoxy(x+22, y+12); printf("%s's Calendar", activeUser);
}

void addEvent(int x, int y){
   int i = 0;

   Event evItem;
   evItem.time.mm = evItem.time.yy = evItem.time.dd = 0;
   evItem.time.hh = evItem.time.min = 0;


   printEventHeader(x, y);
   y+= 3;

   SetColor(8);

   gotoxy(x+22, y+2); printf("Holiday/Appointment etc.");
   gotoxy(x+22, y+4); printf("dd mm yyyy");
   gotoxy(x+22, y+6); printf("HH:MM (optional)");
   gotoxy(x+22, y+10); printf("(optional)");
   gotoxy(x+22, y+12); printf("%s's Calendar", activeUser);

   SetColor(15);

   char de;
   int ch;
   printStatusMsg("ESC Go Back");

   gotoxy(x+22, y);
   char ch2[30]; i=0;
   int ret = takeInput(ch2);
   if(ret>0){
      ch2[29] = '\0';
      strcpy(evItem.note, ch2);
   }else
      return;




   gotoxy(x+22, y+2); printf("%24s", "");
   gotoxy(x+22, y+2);
   ret = takeInput(ch2, 0, 1);
   if(ret>0)
      strcpy(evItem.type, ch2);
   else if(ret==-1)
      return;
   else{
      strcpy(evItem.type, "General");
      printf("General");
   }

   printStatusMsg("");

   char buff[30];

   tm *ltm;
   int dd, mm, yy;

   dateinput:
   gotoxy(x+22, y+4);


   ret = takeInput(ch2);
   if(ret>0){
      dd = atoi(strtok(ch2, ":- /"));
      mm = atoi(strtok(NULL, ":- /"));
      yy = atoi(strtok(NULL, ":- /"));
      if(yy<99) yy += 2000;

      if(!(dd*mm*yy) || dd>31 || mm>12 || yy>3000){
         printStatusMsg("Invalid date");
         gotoxy(x+22, y+4); printf("%24s", "");
         goto dateinput;
      }

      evItem.time.dd = dd;
      evItem.time.mm = mm;
      evItem.time.yy = yy;

      getltm(ltm, yy, mm, dd);
      strftime(buff, 30, "%d %b %Y", ltm);
      gotoxy(x+22, y+4); printf(buff);
   }else if(ret==0){
      goto dateinput;
   }else
      return;


   printStatusMsg("");

   timeinput:
   gotoxy(x+22, y+6); printf("%17s", "");
   gotoxy(x+22, y+6);
   ret = takeInput(ch2);
   if(ret>0){
      evItem.allday = 0;
      evItem.time.hh = atoi(strtok(ch2, ":- /"));
      evItem.time.min = atoi(strtok(NULL, ":- /"));

      if(evItem.time.hh>=24 || evItem.time.min>=60){
         printStatusMsg("Invalid time");
         goto timeinput;
      }
      getltm(ltm, evItem.time.yy, evItem.time.mm, evItem.time.dd, evItem.time.hh, evItem.time.min);
      strftime(buff, 30, "%I:%M %p          ", ltm);
      gotoxy(x+22, y+6); printf(buff);
   }else if(ret==0){
      evItem.allday = 1;
      gotoxy(x+22, y+6);
      printf("All Day         ");
   }else
      return;

   evItem.epoch = mktime(ltm);
   gotoxy(x+22, y+8);

   int opti = 1;
   printf("%c %s %c", char(174), Priorities[opti], char(175));
   int ex=0;
   fflush(stdin);

   while(ch=getch()){
      if(ch==224 || ch==0){
         ch = getch();
         switch(ch){
            case KEY_LEFT:
               if(++opti > 2) opti = 0;break;
            case KEY_RIGHT:
               if(--opti < 0) opti = 2;break;
            case KEY_UP: case KEY_DOWN:
            default:
               ex=1; break;
         }
         if(ex) break;
         gotoxy(x+22, y+8);
         printf("%15s", " ");
         gotoxy(x+22, y+8);
         printf("%c %s %c", char(174), Priorities[opti], char(175));
      }else break;
   }

   evItem.priority = opti;

   gotoxy(x+22, y+8); printf("%15s", " ");
   gotoxy(x+22, y+8); printf(Priorities[evItem.priority]);

   gotoxy(x+22, y+10); printf("             ");
   gotoxy(x+22, y+10);
   if(takeInput(ch2)>0)
      strcpy(evItem.location, ch2);
   else
      strcpy(evItem.location, "");

   y+=14;

   gotoxy(x, y);
   printf("Add this event? (y/n) ");

   ch = getch();
   if(ch=='y' || ch==KEY_ENTER){
      if(events.add(evItem)){
         gotoxy(x,y);
         printf("Event Saved.%12s", "");
      }else{
         gotoxy(x, y);
         SetColor(12);
         printf("Error%12s", "");
         ClearColor();
      }
   }else{
      gotoxy(x, y);
      printf("\aEvent Discarded.%12s", "");
      if(ch==0 || ch==224) getch();
   }


   printStatusMsg("Press any key to continue.");
   ch = getch();
   if(ch==0 || ch==224) getch();
}



void editEvent(int x, int y, long evId){
   int i = 0;

   Event evItem;
   printHeader(stx, "Edit Event");
   if(!events.getById(evId, evItem)) return;

   printEventHeader(x, y);
   y+= 3;

   SetColor(8);

   printEventValues(x, y, evItem);

   SetColor(15);

   char de;
   int ch;
   printStatusMsg("Leave the field blank to keep the value unchanged.");

   gotoxy(x+22, y);printf("%24s", "");
   gotoxy(x+22, y);
   char ch2[30]; i=0;
   int ret = takeInput(ch2);
   if(ret>0){
      ch2[29] = '\0';
      strcpy(evItem.note, ch2);
   }else if(ret==-1)
      return;
   else{
      gotoxy(x+22, y);
      SetColor(7);
      printf(evItem.note);
      SetColor(15);
   }




   gotoxy(x+22, y+2); printf("%24s", "");
   gotoxy(x+22, y+2);
   ret = takeInput(ch2, 0, 1);
   if(ret>0)
      strcpy(evItem.type, ch2);
   else if(ret==-1)
      return;
   else{
      gotoxy(x+22, y+2);
      SetColor(7);
      printf(evItem.type);
      SetColor(15);
   }

   char buff[30];

   tm *ltm;
   int dd, mm, yy;

   dateinput:
   gotoxy(x+22, y+4);
   printf("%15s", "");
   gotoxy(x+22, y+4);
   ret = takeInput(ch2);
   if(ret>0){
      dd = atoi(strtok(ch2, ":- /"));
      mm = atoi(strtok(NULL, ":- /"));
      yy = atoi(strtok(NULL, ":- /"));
      if(yy<99) yy += 2000;

      if(!(dd*mm*yy) || dd>31 || mm>12 || yy>3000){
         printStatusMsg("Invalid date");
         gotoxy(x+22, y+4); printf("%24s", "");
         goto dateinput;
      }

      evItem.time.dd = dd;
      evItem.time.mm = mm;
      evItem.time.yy = yy;
   }else if(ret==0){

   }else
      return;

   getltm(ltm, evItem.time.yy, evItem.time.mm, evItem.time.dd);
   strftime(buff, 30, "%d %b %Y", ltm);
   gotoxy(x+22, y+4);

   if(ret==0)SetColor(7);
   printf(buff);
   SetColor(15);

   timeinput:
   gotoxy(x+22, y+6); printf("%17s", "");
   gotoxy(x+22, y+6);
   ret = takeInput(ch2);
   if(ret>0 || ret==0){
      if(ret>0){
         evItem.allday = 0;
         evItem.time.hh = atoi(strtok(ch2, ":- /"));
         evItem.time.min = atoi(strtok(NULL, ":- /"));

         if(evItem.time.hh>=24 || evItem.time.min>=60){
            printStatusMsg("Invalid time");
            goto timeinput;
         }
      }
      if(evItem.allday){SetColor(7);
         printf("All Day");SetColor(15);
      }else{
         getltm(ltm, evItem.time.yy, evItem.time.mm, evItem.time.dd, evItem.time.hh, evItem.time.min);
         strftime(buff, 30, "%I:%M %p          ", ltm);
         gotoxy(x+22, y+6);
         if(ret==0)SetColor(7);
         printf(buff);
         SetColor(15);
      }
   }else{
      evItem.allday = 1;
      gotoxy(x+22, y+6);
      printf("All Day");
   }

   evItem.epoch = mktime(ltm);
   gotoxy(x+22, y+8);

   int opti = evItem.priority;
   int orig_opti = opti;
   printf("%c %s %c", char(174), Priorities[opti], char(175));
   int ex=0;
   fflush(stdin);

   while(ch=getch()){
      if(ch==224 || ch==0){
         ch = getch();
         switch(ch){
            case KEY_LEFT:
               if(++opti > 2) opti = 0;break;
            case KEY_RIGHT:
               if(--opti < 0) opti = 2;break;
            case KEY_UP: case KEY_DOWN:
            default:
               ex=1; break;
         }
         if(ex) break;
         gotoxy(x+22, y+8);
         printf("%15s", " ");
         gotoxy(x+22, y+8);
         printf("%c %s %c", char(174), Priorities[opti], char(175));
      }else break;
   }

   evItem.priority = opti;

   gotoxy(x+22, y+8); printf("%15s", " ");
   gotoxy(x+22, y+8);
   if(opti==orig_opti)
      SetColor(7);
   printf(Priorities[evItem.priority]);
   SetColor(15);

   gotoxy(x+22, y+10); printf("             ");
   gotoxy(x+22, y+10);
   ret = takeInput(ch2);
   if(ret>0)
      strcpy(evItem.location, ch2);
   else if(ret==-1)
      strcpy(evItem.location, "");
   else{
      gotoxy(x+22, y+10);
      SetColor(7);
      printf(evItem.location);
      SetColor(15);
   }

   y+=14;

   gotoxy(x, y);
   printf("Confirm edit? (y/n) ");

   ch = getch();
   if(ch=='y' || ch==KEY_ENTER){
      events.del(evId);
      if(events.add(evItem)){
         gotoxy(x,y);
         printf("Event Updated.%12s", "");
      }else{
         gotoxy(x, y);
         SetColor(12);
         printf("Error%12s", "");
         ClearColor();
      }
   }else{
      gotoxy(x, y);
      printf("\aEvent Discarded.%12s", "");
      if(ch==0 || ch==224) getch();
   }


   printStatusMsg("Press any key to continue.");
   ch = getch();
   if(ch==0 || ch==224) getch();
}

void printHeadSE(int x, int y){
   gotoxy(x, y);
   SetColorAndBackground(15, 9);
   printf("                       ");
   SetColorAndBackground(15, 0);
   printf(" ");
   SetColorAndBackground(15, 9);
   printf("            ");
   SetColorAndBackground(15, 0);
   printf(" ");
   SetColorAndBackground(15, 9);
   printf("                   ");
   SetColorAndBackground(15, 0);
   gotoxy(x,y+1);
   SetColorAndBackground(15, 9);
   printf("          Title        ");
   SetColorAndBackground(15, 0);
   printf(" ");
   SetColorAndBackground(15, 9);
   printf("    Type    ");
   SetColorAndBackground(15, 0);
   printf(" ");
   SetColorAndBackground(15, 9);
   printf("       Date        ");
   SetColorAndBackground(15, 0);
}

void showEvents(int x, int y){
   int i = 0, found = 0;
   int orig_x = x, orig_y = y;
   Event ev;
   char buff[30];
   printHeadSE(x, y);
   y = orig_y+3;
   gotoxy(x, y);

   Menu m;
   m.nokey = 1;

   char buff2[80];
   i = 0;
   int fpos=0;
   int ipp = (height-12)/2;
   if(ipp>20) ipp = 20;
   long evids[20]={0};
   int j=0;
   int last_i=0, last_fpos=0;
   int hlItem=0;
   int evc = 0;
   while(1){
      m.clear();

      found = 0; j =0;
      last_i = i; last_fpos = fpos;
      evc = events.getCount();

      while(events.fetch(ev, fpos)){
         i++; j++;
         tm *ltm;
         getltm(ltm, ev.time.yy, ev.time.mm, ev.time.dd, ev.time.hh, ev.time.min);
         if(ev.allday)
            strftime(buff, 30, "%d %b %y", ltm);
         else
            strftime(buff, 30, "%d %b %y %H:%M", ltm);

         if(strlen(ev.note) > 21)
            ev.note[19] = ev.note[20] = '.';

         sprintf(buff2, " %-21.21s  %-11.11s  %-18s", ev.note, ev.type, buff);
         m.addItem('-', buff2);
         evids[j-1] = ev.id;
         found = 1;
         if(j==ipp)
            break;
      }

      // skip empty page (when ipp is perfect multiple of evc)
      if(j==0 && evc)continue;

      // loop exited before all items were printed / end of list
      if(j!=ipp || !found){
         clearBody(x, 56, y);
      }

      if(found == 0){
         //hr(x, y, 56);
         gotoxy(x, y+1);
         printf(" No events found.");
         //hr(x, y+2, 56);
      }else{
         //hr(x, y, 56);
         m.print(x, y, 56, 1);
         if(hlItem){

            m.activeItem = hlItem-1;
            m.highlight(1);

            hlItem=0;
         }
         hr(x, height-5, 56, 7);
         gotoxy(x, height-4);
         printf("Showing events %d-%d of %d    ", i-j+1, i, evc);
         gotoxy(x+56-10, height-4);
         printf("Page %02d/%02d", (i-1)/ipp+1, (evc-1)/ipp+1);

         //hr(x, y+i, 56);
      }

      if(j!=ipp || !found)
         i=0;

      int choice, ch;
      int ex = 0;
      printStatusMsg("ESC Back\tUP/DOWN Navigate\tLEFT/RIGHT Previous/Next Page");
      while(choice=getch()){
         if(choice==0 || choice==224){
            choice = getch();

            switch(choice){
               case KEY_LEFT:
                  fpos = last_fpos - sizeof(ev)*ipp;
                  i = last_i-ipp;
                  if(fpos<0){ fpos = 0; i=0;}
                  if(fpos != last_fpos)
                     ex=1;
                  break;
               case KEY_RIGHT:
                  ex = 1;
                  break;
               case KEY_UP: // up
                  m.highlight(-1);
                  break;
               case KEY_DOWN: // down
                  m.highlight(1);
                  break;
               default:
                  break;
            }
         }else if(choice==KEY_SPACE)
            ex=1;
         else if(choice==KEY_ESC)
            return;
         else if(choice==KEY_ENTER){
            if(m.activeItem){
               MenuItem *item_;
               if(m.getItemByNo(m.activeItem, item_)){
                  int mItemNo = m.activeItem; // convert char '1' to int 1
                  m.select(mItemNo);
                  if(width<90){
                     clearBody(stx-20, 56);
                     viewEvent(evids[mItemNo-1], stx-20, 5);
                     clearBody(stx-20, 56);
                  }else{
                     clearBody(stx-30, 56);
                     viewEvent(evids[mItemNo-1], stx-30, 5);
                     clearBody(stx-30, 56);
                  }
//                  clearBody(x, 56, y);

                  printHeadSE(orig_x, orig_y);
                  //m.print(x-3, y, 56+3, 1);

                  fpos = last_fpos;
                  i = last_i;
                  hlItem = mItemNo;
                  ex=1;
               }
            }
         }

         if(ex) break;
      }

   }
}

void viewEvent(long evId, int x, int y){
   printHeader(stx, "View Event Details");
   int orig_x=x, orig_y=y;
   Event evItem;
   if(!events.getById(evId, evItem)) return;

   int i = 0, found = 0;


   printEventHeader(x, y);
   y += 3;
   y += 12;

   printEventValues(x, orig_y+3, evItem);
   cont:

   SetColor(15);
   gotoxy(x+10, y+2);
   printf("(e) Edit Event   (d) Delete Event");
   SetColor(14);
   gotoxy(x+11, y+2); printf("e");
   gotoxy(x+28, y+2); printf("d");
   gotoxy(x+56, y+2);
   SetColor(15);

   char de;
   int ch;
   printStatusMsg("ESC Go Back\te Edit Event\td Delete Event");

   while(ch = getch()){
      if(ch==0 || ch==224){
         ch = getch();
         if(ch==KEY_DELETE)
            ch = 'd';
         else
            continue;

      }

      if(ch==KEY_ESC)
         return;
      else if(ch=='e' || ch=='E'){
         clearBody(orig_x, 56);//
         editEvent(orig_x, orig_y, evId);
         events.getById(evId, evItem);
         clearBody(orig_x, 56);//
         printHeader(stx, "View Event Details");
         printEventHeader(x, orig_y);
         printEventValues(x, orig_y+3, evItem);
         goto cont;

      }else if(ch=='d' || ch=='D'){
         gotoxy(x, y+2);
         printf("Are you sure you want to delete this event? (y/n)");
         ch = getch();
         if(ch==0 || ch==224) ch=getch();
         gotoxy(x, y+2);
         printf("%56s", "");
         if(ch=='y'){
            events.del(evId);
            gotoxy(x, y+2);
            printf("\aEvent Removed.");
            Sleep(1800);
            break;
         }else{
            goto cont;
         }

      }

   }
}

void showEvents2(int yy, int mm, int x, int y){
   int i = 0, found = 0, mc = 0;
   Event ev;
   int fpos = 0;
   while(events.fetch(ev, fpos, yy, mm)){
      if(i<=3){
         gotoxy(x, i+y);
         SetColor(7);
         printf("%.2d/%.2d", ev.time.dd, ev.time.mm);
         SetColor(15);
         if(strlen(ev.note)>13)
            printf(" %.13s..", ev.note);
         else
            printf(" %s", ev.note);
         found = 1;
         i++;
      }else
         mc++;
   }

   if(found == 0){
      gotoxy(x, y);
      printf("No events.");
   }else if(mc){
      gotoxy(x, i+y);
      SetColor(8);
      printf("  ..+%d more event%c", mc, mc>1 ? 's': ' ');
      SetColor(15);
   }

}

void showCalendar(Date &d, int x, int y, int compact, int nextprev){
   Calendar(d).print(x, y, compact);
   y+=11;
   gotoxy(x, y++);
   SetColor(14);
   printf("Events This Month:");
   y++;
   if(compact)
      hr(x, y-1, 21, 14);
   else
      hr(x, y-1, 27, 14);
   SetColor(15);
   showEvents2(d.yy, d.mm, x, y);

   if(nextprev){
      gotoxy(x, y+6);
      SetColor(15); printf("n");
      SetColor(8); printf("ext month  ");
      SetColor(15); printf("p");
      SetColor(8); printf("rev month");

      SetColor(15);
   }

}
