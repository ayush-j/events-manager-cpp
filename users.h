#ifndef USERS_H_INCLUDED
#define USERS_H_INCLUDED

#include<fstream>
using namespace std;

struct User {
   char name[20];
   char passwd[20];
};

class Users {
   User user;
   fstream f;
   char fname[40];
   public:
   Users(){
      strcpy(fname, ".\\data\\users.dat");
   }

   int  get(char[20], User&);
   void add(User&);
   void del(char[20]);
   int  fetch(User&, int&, char[30]="");
};

void Users::add(User &u){
   fstream f;
   f.open(fname, ios::out | ios::app | ios::binary);
   f.write((char*)&u, sizeof(u));
   f.close();
}

void Users::del(char name[20]){
   char fname2[sizeof(fname)+4];
   strcpy(fname2, fname);
   strcat(fname2, ".tmp");

   fstream f;
   f.open(fname, ios::in | ios::binary);
   ofstream fo(fname2, ios::out | ios::binary);

   while(f.read((char*)&user, sizeof(user)))
      if(strcmp(user.name, name) != 0)
         fo.write((char*)&user, sizeof(user));

   fo.close();
   f.close();

   remove(fname);
   rename(fname2, fname);
}

int Users::get(char name[20], User& u){
   int pos = 0;
   int found = fetch(u, pos, name);
   return found;
}

int Users::fetch(User& u, int &pos, char name[20]){
   fstream f;
   f.open(fname, ios::in | ios::binary);

   if(!f){
      pos = 0;
      return 0;
   }
   int found = 0;

   if(pos)
      f.seekg(pos, ios::beg);

   while(f.read((char*)&user, sizeof(user))){
      if(strlen(name)==0 || strcmp(name, user.name)==0){
         found = 1;
         u = user;
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

#endif // USERS_H_INCLUDED
