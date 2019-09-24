#ifndef _VIZ_
#define _VIZ_

void init(int m, int n, int (*maze)(int, int), int (*info)(int, int));
void clear_screen();
void gotoxy(int x, int y);
void draw();
void wait(int milisec);
void set_info_str(int info, const wchar_t *str);

void set_cursor(bool on);

#endif