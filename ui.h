#ifndef _UI_
#define _UI_

void clear_screen();
void gotoxy(int x, int y);
void show_cursor(bool on);

void set_bg(int c);
void set_fg(int c);

void reset_fg();
void reset_bg();
void reset_all();


void wait(int milisec);


#endif