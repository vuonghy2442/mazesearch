#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <iomanip>
#include "viz.h"
#include "common.h"
#include "ui.h"

#define CELL_W 3
#define CELL_H 1

#define RB L'\u2518' // 188 Right Bottom corner
#define RT L'\u2510' // 187 Right Top corner
#define LT L'\u250C' // 201 Left Top cornet
#define LB L'\u2514' // 200 Left Bottom corner
#define MC L'\u253C' // 206 Midle Cross
#define HL L'\u2500' // 205 Horizontal Line
#define H1 L'\u2574' // 205 Horizontal Half Line Left
#define H2 L'\u2576' // 205 Horizontal Half Line Right
#define LC L'\u251C' // 204 Left Cross
#define RC L'\u2524' // 185 Right Cross
#define BC L'\u2534' // 202 Bottom Cross
#define TC L'\u252C' // 203 Top Cross
#define VL L'\u2502' // 186 Vertical Line
#define V1 L'\u2575' // 186 Vertical Half Line Top
#define V2 L'\u2577' // 186 Vertical Half Line Bottom

#define FB L'\u2588' //full block
#define SQ L'\u25A0' //square
#define LHB L'\u258C' //Left half block
#define RHB L'\u2590' //right half block
#define THB L'\u2580' //Upper half block
#define BHB L'\u2584' //Lower half block

#define LBB L'\u2596' //Quadrant lower left
#define RBB L'\u2597' //Quadrant lower right
#define LTB L'\u2598' //Quadrant upper left 
#define RTB L'\u259D' //

#define EDGE(i, j, d) ((bool)(maze(i,j) & (1 << d)))

#define SP L' ' 		  // space string
#define ST L'*'

#define N_INFO 16

static const wchar_t box_char[] = {SP, H2, V2, LT, H1, HL, RT, TC, V1, LB, VL, LC, RB, BC, RC, MC};
static const wchar_t block_char[] = {RHB, BHB, LHB, THB};
static const wchar_t qblock_char[] = {LTB, LBB, RBB, RTB};

static int info_color[N_INFO];

static int m, n;
static int (*maze)(int, int);
static int info[MAX_M][MAX_N];
static int value[MAX_M][MAX_N];
static bool is_show[MAX_M][MAX_N];


typedef std::tuple<int, int, int, int> region;

bool check_bidir() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(m, n, u, v)) {
                    int rd = rev_dir(d);
                    bool w1 = EDGE(i, j, d);
                    bool w2 = EDGE(u, v, rd);

                    if (w1 != w2)
                        return false;
                }
                
            }
        }
    }
    return true;
}

std::wstring text_horizontal(int i) {
    std::wstring s;

    for(int j = 0; j < n; ++j) {
        s += ST;
        
        wchar_t c = EDGE(i,j, DUP)?SP:ST;
        s += std::wstring(CELL_W, c);
    }
    s += ST;
    return s;
}

std::wstring text_vertical(int i) {
    std::wstring s;
    
    s += ST;
    for(int j = 0; j < n; ++j) {

        s += std::wstring(CELL_W, SP);

        if (EDGE(i, j, DRIGHT))
            s += SP;
        else
            s += ST;
    }
    return s;
}

void star_to_box(std::vector<std::wstring> &border){

    const int h = border.size();
    if (!h)
        return;

    const int w = border[0].length();


    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (border[i][j] == ' ')
                continue;

            int type = 0;
            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(h, w, u, v) && border[u][v] != ' ')
                    type |= (1 << d);
            }

            border[i][j] = box_char[type];
        }
    }

}

std::vector<std::wstring> get_border() {
    //star border
    std::vector<std::wstring> border;

    for (int i = 0; i < m; ++i) {
        border.push_back(text_horizontal(i));

        std::wstring s = text_vertical(i);
        for (int k = 0; k < CELL_H; ++k)
            border.push_back(s);
    }
    border.push_back(std::wstring((CELL_W + 1) * n + 1,'*'));

    star_to_box(border);

    return border;
}

void draw_border() {
    gotoxy(1, 1);
    std::vector<std::wstring> border = get_border();

    for (int i = 0; i < border.size(); ++i)
        std::wcout << border[i] << std::endl;
}

region get_cell_reg(int i, int j) {
    return {i     * (CELL_H + 1) + 2, j     * (CELL_W + 1) + 2, 
            (i+1) * (CELL_H + 1), (j+1) * (CELL_W + 1)};
}

void clear_cell(int i, int j){
    int l, r, u, d;
    std::tie(u, l, d, r) = get_cell_reg(i, j);

    for (int row = u; row <= d; ++row) {
        gotoxy(row, l);
        for (int col = l; col <= r; ++col)
            std::wcout << L' ';
    }
}

void fill(wchar_t c, region reg) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    for (int row = u; row <= d; ++row) {
        gotoxy(row, l);
        for (int col = l; col <= r; ++col)
           std::wcout << c;
    }
}


void draw_value(int v, region reg) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    int mid = (u + d) / 2;

    gotoxy(mid, l);
    std::wcout <<  std::setw(CELL_W) << v;
}

region get_edge(region reg, int dir) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    switch(dir) {
        case DLEFT:
            return {u, l, d, l};
        case DRIGHT:
            return {u, r, d, r};
        case DUP:
            return {u, l, u, r};
        case DDOWN:
            return {d, l, d, r};
        default:
            throw std::runtime_error("bug");
    }
}

region reduce_reg(region reg, int rl, int rr, int ru, int rd) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    return {u + ru, l + rl, d - rd, r - rr};
}

region move_reg(region reg, int row, int col) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    return {u + row, l + col, d + row, r + col};
}

region move_reg(region reg, int dir) {
    return move_reg(reg, dr[dir], dc[dir]);
}

void draw_edge(int i, int j, int d) {
    if (!EDGE(i, j ,d))
        return;

    int u = i + dr[d];
    int v = j + dc[d];

    int col1 = info_color[info[i][j]];
    int col2 = info_color[info[u][v]];
    int colmid = info_color[std::min(info[i][j], info[u][v])];

    bool ver = dir_ver(d);

    region reg = get_edge(get_cell_reg(i, j), d);


    reg = move_reg(reg, d);

    if (col1 >= 0 && col2 >= 0) {
        set_bg(colmid);
        fill(SP, reg);
        reset_bg();
    } else {
        fill(SP, reg);
    }
}

void draw_edge(int i, int j){
    for (int d = 0; d < 4; ++d)
        draw_edge(i, j, d);
}

void draw_cell(int i, int j) {
    int col = info_color[info[i][j]];
    if (col < 0)
        clear_cell(i, j);

    region reg = get_cell_reg(i, j);

    set_bg(col);
    if (is_show[i][j])
        draw_value(value[i][j],  reg);
    else
        fill(SP, reg);
    reset_bg();

    std::wcout.flush();
}

void draw_cell() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            draw_cell(i, j);
        }
    }
}


void set_info_color(int info, int col) {
    info_color[info] = col;
}

void init(int m_, int n_, int (*maze_)(int, int)) {
    m = m_;
    n = n_;
    maze = maze_;

    for (int i = 0; i < N_INFO; ++i)
        info_color[i] = -1;

    if (!check_bidir())
        throw std::runtime_error("Wrong input");
}

int get_info(int i, int j) {
    return info[i][j];
}

void set_info(int i, int j, int v) {
    info[i][j] = v;
    draw_cell(i,j);
    draw_edge(i,j);
}

int get_value(int i, int j) {
    return value[i][j];
}

void set_value(int i, int j, int v) {
    value[i][j] = v;
    if (is_show[i][j])
        draw_cell(i,j);
}

void show_value(int i, int j) {
    if (!is_show[i][j]) {
        is_show[i][j] = true;
        draw_cell(i,j);
    }
}

void hide_value(int i, int j) {
    if (is_show[i][j]) {
        is_show[i][j] = false;
        draw_cell(i,j);
    }
}

void clear_info() {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            info[i][j] = 0;

    draw_cell();
}

void gotoend() {
    gotoxy(m * (CELL_H + 1)  + 2, 0);
    std::wcout.flush();
}