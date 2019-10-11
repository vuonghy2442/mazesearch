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

#define RB "\xE2\x94\x98" // 188 Right Bottom corner
#define RT "\xE2\x94\x90" // 187 Right Top corner
#define LT "\xE2\x94\x8C" // 201 Left Top cornet
#define LB "\xE2\x94\x94" // 200 Left Bottom corner
#define MC "\xE2\x94\xBC" // 206 Midle Cross
#define HL "\xE2\x94\x80" // 205 Horizontal Line
#define H1 "\xE2\x95\xB4" // 205 Horizontal Half Line Left
#define H2 "\xE2\x95\xB6" // 205 Horizontal Half Line Right
#define LC "\xE2\x94\x9C" // 204 Left Cross
#define RC "\xE2\x94\xA4" // 185 Right Cross
#define BC "\xE2\x94\xB4" // 202 Bottom Cross
#define TC "\xE2\x94\xAC" // 203 Top Cross
#define VL "\xE2\x94\x82" // 186 Vertical Line
#define V1 "\xE2\x95\xB5" // 186 Vertical Half Line Top
#define V2 "\xE2\x95\xB7" // 186 Vertical Half Line Bottom

#define FB  "\xE2\x96\x88" //full block
#define SQ  "\xE2\x96\xA0" //square
#define LHB "\xE2\x96\x8C" //Left half block
#define RHB "\xE2\x96\x90" //right half block
#define THB "\xE2\x96\x80" //Upper half block
#define BHB "\xE2\x96\x84" //Lower half block

#define EDGE(i, j, d) ((bool)(maze(i,j) & (1 << d)))

#define SP " " 		  // space string
#define ST "*"

#define N_INFO 16

static const std::string box_char[] = {SP, H2, V2, LT, H1, HL, RT, TC, V1, LB, VL, LC, RB, BC, RC, MC};
static const std::string block_char[] = {RHB, BHB, LHB, THB};

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

std::string dup(const std::string &s, int times) {
    std::string res;

    for (int i = 0; i < times; ++i)
        res += s;

    return res;
}

std::string text_horizontal(int i) {
    std::string s;

    for(int j = 0; j < n; ++j) {
        s += ST;
        s += dup(EDGE(i,j, DUP)?SP:ST, CELL_W);
    }
    s += ST;
    return s;
}

std::string text_vertical(int i) {
    std::string s;
    
    s += ST;
    for(int j = 0; j < n; ++j) {

        s += dup(SP, CELL_W);

        if (EDGE(i, j, DRIGHT))
            s += SP;
        else
            s += ST;
    }
    return s;
}

void star_to_box(std::vector<std::string> &border){

    const int h = border.size();
    if (!h)
        return;

    const int w = border[0].length();

    std::vector<std::string> res(h);

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (border[i][j] == ' ') {
                res[i] += SP;
                continue;
            }

            int type = 0;
            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(h, w, u, v) && border[u][v] != ' ')
                    type |= (1 << d);
            }

            res[i] += box_char[type];
        }
    }

    border = res;

}

std::vector<std::string> get_border() {
    //star border
    std::vector<std::string> border;

    for (int i = 0; i < m; ++i) {
        border.push_back(text_horizontal(i));

        std::string s = text_vertical(i);
        for (int k = 0; k < CELL_H; ++k)
            border.push_back(s);
    }
    border.push_back(std::string((CELL_W + 1) * n + 1,'*'));

    star_to_box(border);

    return border;
}

void draw_border() {
    gotoxy(1, 1);
    std::vector<std::string> border = get_border();

    for (int i = 0; i < border.size(); ++i)
        std::cout << border[i] << std::endl;
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
            std::cout << ' ';
    }
}

void fill(const std::string c, region reg) {
    int l, r, u, d;
    std::tie(u, l, d, r) = reg;

    for (int row = u; row <= d; ++row) {
        for (int col = l; col <= r; ++col) {
            gotoxy(row, col);
            std::cout << c;
        }
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

    std::cout.flush();
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

    clear_screen();
    draw_border();
    //draw_cell();
}

void gotoend() {
    gotoxy(m * (CELL_H + 1)  + 2, 0);
    std::cout.flush();
}