#ifndef _VIZ_
#define _VIZ_
void init(int m, int n, int (*maze)(int, int));

int get_info(int i, int j);
void set_info(int i, int j, int v);

int get_value(int i, int j);
void set_value(int i, int j, int v);
void show_value(int i, int j);
void hide_value(int i, int j);


void clear_info();

void gotoend();
void draw_border();
void draw_cell();

void set_info_color(int info, int color);

#endif