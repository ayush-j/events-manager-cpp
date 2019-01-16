#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

struct MenuItem {
   int   itemNo;
   char  name[90];
   int   state;
   struct COORDS{ int x,y; } coords;
   char  key;
   MenuItem  *next;
};

struct MenuColors {
   int key_fg, key_bg;
   int txt_fg, txt_bg;
};

class Menu {
   private:
   MenuItem *start, *end, *item;
   int menuX, menuY, menuW;
   void printItem(MenuItem*&);
   void printItem(MenuItem*&, MenuColors&);

   public:
   MenuColors colors;
   int activeItem;
   int count;
   char title[20];
   int nokey;

   Menu(char n[20]="MENU"){
      start = end = NULL;
      menuX = menuY = menuW = 0;
      activeItem = 0;
      count = 0;
      nokey = 0;
      strcpy(title, n);
      colors.key_fg = colors.txt_bg = 0;
      colors.key_bg = colors.txt_fg = 15;
   }

   // manage menu list
   int   addItem(char, char[20], int=0);
   void  clear();

   // highlight item
   void  select(char);
   void  select(int);
   void  select(MenuItem*&);
   void  highlight(int);

   // modify an item
   void  change(char, char[20]);
   void  change(char, int);
   void  change(char, char[20], int);

   // show menu
   void print(int, int, int, int=0);
   void showError(char msg[20]);

   // find item
   int getItemByNo(int, MenuItem*&);
   int getItemByKey(char, MenuItem*&);
};

int Menu::getItemByNo(int itemNo, MenuItem *&ret){
   if(itemNo<1 || itemNo>count) return 0;

   ret = start;
   while(ret){
      if(ret->itemNo == itemNo)
         return 1;
      ret = ret->next;
   }
   return 0;
}

int Menu::getItemByKey(char key, MenuItem *&ret){
   ret = start;
   while(ret){
      if(ret->key == key)
         return 1;
      ret = ret->next;
   }
   return 0;
}

int Menu::addItem(char key, char text[20], int state){
   count++;

   item = new MenuItem;
   item->itemNo = count;
   strcpy(item->name, text);
   item->state = state;
   //item->coords.x = -1;
   //item->coords.y = -1;
   item->key = key; // item_c+1;
   item->next = NULL;

   if(end){
      end->next = item;
      end = item;
   }else
      start = end = item;

   return item->itemNo;
}


void Menu::clear(){
   item = start;

   while(item){
      start = start->next;
      delete item;
      item = start;
   }

   start = end = item = NULL;
   count = 0;
   activeItem = 0;
}

void Menu::printItem(MenuItem *&item){
   printItem(item, colors);
}

void Menu::printItem(MenuItem *&item, MenuColors &colors){
   //gotoxy(menuX, menuY + 2*(item->itemNo+1));
   gotoxy(item->coords.x, item->coords.y);
   if(!nokey){
      SetColorAndBackground(colors.key_fg, colors.key_bg);
      printf(" %c ", item->key);
      SetColorAndBackground(colors.txt_fg, colors.txt_bg);
      printf(" %-*s", menuW-4, item->name);
   }else{
      SetColorAndBackground(colors.txt_fg, colors.txt_bg);
      printf(" %-*s", menuW-1, item->name);
   }
}

void Menu::print(int x, int y, int w, int nohead){
   menuX = x;
   menuY = y;
   menuW = w;

   item = start;

   int first = 1;
   int i = 0;

   if(!nohead){
      gotoxy(x,y+1);
      SetColorAndBackground(15, 0);
      printf("%*s", (w+strlen(title))/2, title);
      y++;

      //gotoxy(x,y+1);
      hr(x, y+1, w);
      y+=3;
   }

   while(item){
      if(!first){
         gotoxy(x, y);

         if(!nokey){
            //SetColorAndBackground(colors.key_fg, colors.key_bg);
            SetColorAndBackground(colors.key_bg, colors.key_bg);
            printf(" - ");//tofix
            SetColor(colors.key_fg);
            hr(x+3, y, w-3);
         }else
            hr(x, y, w);
         y++;

      }else
         first = 0;

      item->coords.x = x;
      item->coords.y = y;
      printItem(item);
      y++;
      item = item->next;
   }

   y += 2;

   gotoxy(x, y);
}

void Menu::showError(char msg[20]){
   gotoxy(menuX, menuY + count*2+4);
   printf("%*s", -menuW, msg);
}

void Menu::change(char key, char name[20]){
   if(!getItemByKey(key, item)){
      printf("error");
      return;
   }

   strcpy(item->name, name);
   printItem(item);
}

void Menu::change(char key, int state){
   if(!getItemByKey(key, item)){
      printf("error");
      return;
   }

   item->state = state;
   printItem(item);
}

void Menu::change(char key, char name[20], int state){
   if(!getItemByKey(key, item)){
      printf("error");
      return;
   }

   strcpy(item->name, name);
   item->state = state;
   printItem(item);
}

void Menu::select(int itemNo){
   if(getItemByNo(itemNo, item))
      select(item);
}

void Menu::select(char key){
   if(getItemByKey(key, item))
      select(item);
}

void Menu::select(MenuItem *&item){
   int prev_x = wherex(), prev_y = wherey();

   activeItem = 0;

   MenuColors c = {colors.key_fg, colors.key_bg, colors.txt_bg, colors.txt_fg};
   printItem(item, c);
   gotoxy(prev_x, prev_y);
   SetColorAndBackground(15, 0);

	Sleep(250);

	return;
}

void Menu::highlight(int pos){
   int prev_x = wherex(), prev_y = wherey();

   if(getItemByNo(activeItem, item))
      printItem(item);

   if(pos == 0){
      activeItem = 0;
      gotoxy(prev_x, prev_y);
      return;
   }

   // special case when no item is selected
   // and user scrolls up
   if(activeItem == 0 && pos<0)
      activeItem = pos+1;
   else
      activeItem += pos;

   if(activeItem <= 0)
      activeItem = count+activeItem;
   else if(activeItem > count)
      activeItem = activeItem-count;

   if(getItemByNo(activeItem, item)){
      MenuColors c = {colors.key_bg, colors.key_fg, colors.txt_bg, colors.txt_fg};
      printItem(item, c);
   }

   SetColorAndBackground(15, 0);

   gotoxy(prev_x, prev_y);
}

#endif // MENU_H_INCLUDED
