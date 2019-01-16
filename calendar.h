#ifndef CALENDAR_H_INCLUDED
#define CALENDAR_H_INCLUDED


class Date {
   public:
   int dd, mm, yy;

   Date(){
      time_t t = time(0);  // current time
      struct tm *now = localtime(&t);

      yy = now->tm_year + 1900;
      mm = now->tm_mon + 1;
      dd = now->tm_mday;
   }

   Date(int y, int m, int d){
      yy = y;
      mm = m;
      dd = d;
   }

   int getYear(){ return yy; };
   int getMonth(){ return mm; };
   int getDate(){ return dd; };
   void nextDate(int=1);
   void prevDate(int=1);
   void nextMonth(int=1);
   void prevMonth(int=1);
} Today;


class Calendar {
   public:
   int dd, mm, yy;

   Calendar(){
      Calendar(Date());
   }

   Calendar(int dd, int month, int year){
      dd = dd;
      mm = month;
      yy = year;
   }

   Calendar(int month, int year){
      dd = 1;
      mm = month;
      yy = year;
   }

   Calendar(Date &d){
      dd = d.dd;
      mm = d.mm;
      yy = d.yy;
   }

   void  increaseMonth(int=1);
   void  decreaseMonth(int=1);
   char* getDayName(int);
   void  printMonthHeader(int, int, int);
   int   getDayNumber(int, int, int);
   char* getDay(int);
   void  print(int, int, int=0);

   static int getNoOfDays(int, int);
   static int isLeapYear(int);

};

// checks if it's a leap year or not
int Calendar::isLeapYear(int yy){
    if(yy%400==0 || (yy%100!=0 && yy%4==0))
       return 1;
    return 0;
}

// returns the no. of days in a month
int Calendar::getNoOfDays(int mm, int yy){
   switch(mm){
      case 1 :case 3 :case 5 :
      case 7 :case 8 :case 10:
      case 12:
         return 31;
      case 2 :
         if(isLeapYear(yy))
            return 29;
         else
            return 28;
      default:
         return 30;
   }
}



// increment date and adjust month/year if needed
void Date::nextDate(int s){
   int nod = Calendar::getNoOfDays(mm, yy);
	while(s-- > 0){
		++dd;
		if(dd > nod){
         nextMonth();
			dd -= nod;
		}
	}
}

// decrement date and adjust month/year if needed
void Date::prevDate(int s){
	while(s-- > 0){
		--dd;
		if(dd < 1){
         prevMonth();
         dd = Calendar::getNoOfDays(mm, yy);
		}
	}
}

// increment month and adjust year if needed
void Date::nextMonth(int s){
	while(s-- > 0){
		++mm;
		if(mm > 12){
			++yy;
			mm -= 12;
		}
	}
}

// decrement month and adjust year if needed
void Date::prevMonth(int s){
    while(s-- > 0){
		--mm;
		if(mm < 1){
			--yy;
			if(yy<1900) return;
			mm += 12;
		}
	}
}

// maps 0-6 to Day names
char *Calendar::getDayName(int day){ //returns the name of the day
   switch(day){
      case 0 :return("Sunday");
      case 1 :return("Monday");
      case 2 :return("Tuesday");
      case 3 :return("Wednesday");
      case 4 :return("Thursday");
      case 5 :return("Friday");
      case 6 :return("Saturday");
      default:return("Error0");
   }
}

// prints the month name and year
void Calendar::printMonthHeader(int x, int y, int compact=0){
   if(compact)
      hr(x, y, 21, 7); //printf("---------------------\n");
   else
      hr(x, y, 27, 7); //printf("---------------------------\n");

   gotoxy(compact ? x+3 : x+5,y+1);

   switch(mm){
      case 1: printf("January"); break;
      case 2: printf("February"); break;
      case 3: printf("March"); break;
      case 4: printf("April"); break;
      case 5: printf("May"); break;
      case 6: printf("June"); break;
      case 7: printf("July"); break;
      case 8: printf("August"); break;
      case 9: printf("September"); break;
      case 10: printf("October"); break;
      case 11: printf("November"); break;
      case 12: printf("December"); break;
   }

   printf(" , %d", yy);
   //gotoxy(x,y+2);
   if(compact)
      hr(x, y+2, 21, 7); // printf("---------------------\n");
   else
      hr(x, y+2, 27, 7); //printf("---------------------------");
}

// returns the day number
int Calendar::getDayNumber(int day, int mm, int yy){
    int res = 0, t1, t2, year = yy;
    year = year - 1600;

    while(year >= 100){
        res = res + 5;
        year = year - 100;
    }

    res = (res % 7);
    t1 = ((year - 1) / 4);
    t2 = (year-1)-t1;
    t1 = (t1*2)+t2;
    t1 = (t1%7);
    res = res + t1;
    res = res%7;
    t2 = 0;

    for(t1 = 1;t1 < mm; t1++)
        t2 += getNoOfDays(t1, yy);

    t2 = t2 + day;
    t2 = t2 % 7;

    res = res + t2;
    res = res % 7;

    if(yy > 2000)
        res = res + 1;
    res = res % 7;

    return res;
}

// get day name for a date
char* Calendar::getDay(int dd){
    int day;

    if(!(mm>=1 && mm<=12))
        return("Invalid month value");

    if(!(dd>=1 && dd<=getNoOfDays(mm,yy)))
        return("Invalid date");

    if(yy>=1600){
        day = getDayNumber(dd, mm, yy);
        day = day%7;
        return getDayName(day);
    }else
        return("Please give year more than 1600");
}

// prints the calendar
void Calendar::print(int x,int y, int compact){
   int nod  = getNoOfDays(mm, yy);
   int d    = 1;
   int day = getDayNumber(d, mm, yy);
   int day_pos = 0;
   int x_org = x;
   int y_org = y;
   int colw = compact ? 3 : 4;


   gotoxy(x, y);
   printMonthHeader(x, y, compact);

   y += 3;
   gotoxy(x, y);

   if(compact)
      printf(" S  M  T  W  T  F  S ");
   else
      printf(" S   M   T   W   T   F   S  ");

   y++;

   switch(day){
      case 0:
         x = x;
         day_pos = 1;
         break;
      case 1 :
         x = x+colw;
         day_pos = 2;
         break;
      case 2 :
         x=x+colw*2;
         day_pos = 3;
         break;
      case 3 :
         x = x+colw*3;
         day_pos = 4;
         break;
      case 4 :
         x=x+colw*4;
         day_pos = 5;
         break;
      case 5 :
         x=x+colw*5;
         day_pos = 6;
         break;
      case 6 :
         x=x+colw*6;
         day_pos = 7;
         break;
      default :
         printf("Error1"); // tofix
         return;
   }

   gotoxy(x, y);


   SetColorAndBackground(15, 0);

   // build a list of dates having an event
   int dayEvents[31] = {0}, fpos = 0;
   Event ev;
   while(events.fetch(ev, fpos, yy, mm))
      dayEvents[ev.time.dd-1] = 1;

   for(d=1; d<=nod; d++){
      if(dayEvents[d-1])
         SetColor(10);
      else if(d==Today.dd && mm==Today.mm && yy==Today.yy)
         SetColor(14);
      else if(day_pos==1)
         SetColor(12);
      else
         ClearColor();


      printf("%2d%*s", d, colw-2, "");

      if(day_pos%7 == 0){
         y++;
         day_pos = 1;
         gotoxy(x_org, y);
      }else
         day_pos++;
   }

   ClearColor();
}


#endif // CALENDAR_H_INCLUDED
