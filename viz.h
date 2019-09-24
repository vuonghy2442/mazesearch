#ifndef _VIZ_
#define _VIZ_

void init(int m, int n, int (*maze)(int, int));

int get_info(int i, int j);
void set_info(int i, int j, int v);
void clear_info();

void clear_screen();
void gotoxy(int x, int y);
void gotoend();
void draw_border();
void draw_cell();

void wait(int milisec);
void set_info_str(int info, const wchar_t *str);

void set_cursor(bool on);

#endif