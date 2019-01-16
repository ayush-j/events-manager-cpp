#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include<fstream>
using namespace std;

struct EventTime {
   int dd, mm, yy;
   int hh, min;
};

struct Event{
   long id;
   EventTime time;
   long epoch;
   int priority;
   char note[30];
   char type[20];
   char location[80];
   int allday;
};

char* Priorities[3] ={"High", "Normal", "Low"};

class Events {
   char  fname[40];
   int   count;
   Event evItem;
   long makeId(Event&);

   public:
   Events(char user[30]=""){
      evItem.id = 0;

      strcpy(fname, ".\\data\\events");
      if(strlen(user)){
         strcat(fname, "-");
         strcat(fname, user);
      }
      strcat(fname, ".dat");

      count = 0;
   }

   int  add(Event&);
   void del(long);
   void  clear();
   int   getCount();
   int   check(int, int, int);
   int   getById(long, Event&);
   int   fetch(Event&, int&, int=0, int mm=0, int dd=0);
   void  importFromFile(char[20]);
   void  exportToFile(char[20]="events_export.csv");
};

long Events::makeId(Event &ev){
   time_t now = time(0);
   tm *ltm = localtime(&now);
   ltm->tm_year = ev.time.yy-1900;
   ltm->tm_mon  = ev.time.mm-1;
   ltm->tm_mday = ev.time.dd;
   ltm->tm_hour = ev.time.hh;
   ltm->tm_min  = ev.time.min;
   ltm->tm_sec  = 0;

   return mktime(ltm);
}

int Events::getById(long id, Event &ev){
   fstream f;
   f.open(fname, ios::in | ios::binary);
   int found = 0;

   while(f.read((char*)&ev, sizeof(ev))){
     if(ev.id == id){
         found = 1;
         break;
     }
   }

   f.close();
   return found;
}

void Events::del(long evid){
   char fname2[sizeof(fname)+4];
   strcpy(fname2, fname);
   strcat(fname2, ".tmp");
   fstream fp(fname, ios::in | ios::binary);
   ofstream fo(fname2, ios::out | ios::binary);

   while(fp.read((char*)&evItem, sizeof(evItem)))
      if(evItem.id != evid)
         fo.write((char*)&evItem, sizeof(evItem));

   fo.close();
   fp.close();

   remove(fname);
   rename(fname2, fname);
}

int Events::getCount(){
   fstream f;
   f.open(fname, ios::in | ios::binary);
   count = 0;
   if(f){
      f.seekg(0, ios::end);
      count = f.tellg() / sizeof(Event);
      f.close();
   }
   return count;
}


int Events::check(int yy, int mm, int dd){
   fstream f;
   f.open(fname, ios::in | ios::binary);
   if(!f)
      return 0;

   int found = 0;

   while(f.read((char*)&evItem, sizeof(evItem)))
     if(evItem.time.dd==dd && evItem.time.mm==mm && evItem.time.yy==yy)
         found = 1;

   f.close();
   return found;
}

void Events::clear(){
   fstream f;
   f.open(fname, ios::out | ios::trunc);
   f.close();
}

int Events::fetch(Event &ev, int &pos, int yy, int mm, int dd){
   fstream f;
   f.open(fname, ios::in | ios::binary);

   if(!f){
      pos = 0;
      return 0;
   }
   int found = 0;

   if(pos)
      f.seekg(pos, ios::beg);

   while(f.read((char*)&evItem, sizeof(evItem))){
     if((!yy || evItem.time.yy==yy)
        && (!mm || evItem.time.mm==mm)
        && (!dd || evItem.time.dd==dd)){
         found = 1;
         ev = evItem;
         break;
        }
   }


   if(!found)
      pos = 0;
   else
      pos = f.tellg();

   f.close();
   return found;
}

int Events::add(Event &ev){
   fstream f;
   f.open(fname, ios::in | ios::out | ios::binary);
   if(!f)
      f.open(fname, ios::in | ios::out | ios::binary | ios::trunc);

   if(!ev.epoch)
      ev.epoch = makeId(ev);

   if(!ev.id)
      ev.id = ev.epoch;

   Event tmp;
   while(f.read((char*)&evItem, sizeof(evItem))){
      if(evItem.epoch < ev.epoch)
         continue;


      f.seekp((int)-sizeof(evItem), ios::cur);
      break;
   }


   int success = 0;
   int saved = 1;

   while(saved){
      if(f.read((char*)&tmp, sizeof(tmp)))
         f.seekp((int)-sizeof(tmp), ios::cur);
      else{
         saved = 0;
         f.clear();
      }

      if(f.write((char*)&ev, sizeof(ev)))
         success = 1;

      // repositioning is needed for .read to work after .write
      f.seekp(f.tellp());

      ev = tmp;
   }

   f.close();
   return success;
}

void Events::importFromFile(char fn[20]){
   FILE *fp = fopen(fn, "rb");
//tofix
   char line [200];
   int c = 0;int i=0;
   Event evItem;

   while(fgets(line, 200, fp)){
      if(strncmp(line, "BEGIN:VEVENT", 12)==0){
         evItem = Event();
         evItem.id = 0;
      }else if(strncmp(line, "SUMMARY:", 8)==0){
         strtok(line, "\r\n:");
         char *en = strtok(NULL, "\r\n:");
         i=0;
         while(i < strlen(en)){
            if(en[i]=='\\')en[i] = ' ';
            i++;}
         en[29] = '\0';
         strcpy(evItem.note, en);
      }else if(strncmp(line, "DTSTART;VALUE=DATE:", 19)==0){
         strtok(line, ":\r\n");
         char *ymd = strtok(NULL, ":\r\n");
         char y[5], m[3], d[3];
         memcpy(y, &ymd[0], 4);
         memcpy(m, &ymd[4], 2);
         memcpy(d, &ymd[6], 2);
         y[4] = m[2] = d[2] = '\0';
         evItem.time.yy = atoi(y);
         evItem.time.mm = atoi(m);
         evItem.time.dd = atoi(d);

      }else if(strncmp(line, "END:VEVENT", 10)==0){
         c++;
         evItem.allday = 1;
         strcpy(evItem.type,"Holiday");
         evItem.time.min = evItem.time.hh = 0;
         evItem.priority = 1;
         //evItem.id = ++ids;
         add(evItem);
      }
   }
printf("%d events imported", c);
   fclose(fp);
}

void Events::exportToFile(char fn[20]){
   fstream f;
   f.open(fname, ios::in | ios::binary);
   FILE *exp = fopen(fn, "w");

   fputs("Subject, Start Date, Start Time, End Date, End Time, All Day Event, Description, Location, Private\n", exp);

   while(f.read((char*)&evItem, sizeof(evItem))){
      fprintf(exp
         , "\"%s\", %02d/%02d/%d, %02d:%02d, %02d/%02d/%d, %02d:%02d, %s, %s Event, %s%s%s, %s\n"
         , evItem.note
         , evItem.time.mm, evItem.time.dd, evItem.time.yy
         , evItem.time.hh, evItem.time.min
         , evItem.time.mm, evItem.time.dd, evItem.time.yy
         , evItem.time.hh, evItem.time.min
         , evItem.allday ? "True" : "False"
         , evItem.type
         , strlen(evItem.location) ? "\"" : ""
         , evItem.location
         , strlen(evItem.location) ? "\"" : ""
         , "False"
      );
   }

   fclose(exp);
   f.close();
}

#endif // EVENTS_H_INCLUDED
