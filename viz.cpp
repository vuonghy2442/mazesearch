#include <iostream>
#include <string>
#include <vector>
#include "viz.h"
#include "common.h"
#include <chrono>
#include <thread>

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

#define SP L' ' 		  // space string
#define ST L'*'

/*#define RB "\e(0\x6a\e(B" // 188 Right Bottom corner
#define RT "\e(0\x6b\e(B" // 187 Right Top corner
#define LT "\e(0\x6c\e(B" // 201 Left Top cornet
#define LB "\e(0\x6d\e(B" // 200 Left Bottom corner
#define MC "\e(0\x6e\e(B" // 206 Midle Cross
#define HL "\e(0\x71\e(B" // 205 Horizontal Line
#define LC "\e(0\x74\e(B" // 204 Left Cross
#define RC "\e(0\x75\e(B" // 185 Right Cross
#define BC "\e(0\x76\e(B" // 202 Bottom Cross
#define TC "\e(0\x77\e(B" // 203 Top Cross
#define VL "\e(0\x78\e(B" // 186 Vertical Line
#define SP " " 		  // space string*/

#define N_INFO 16

static const wchar_t box_char[] = {SP, H2, V2, LT, H1, HL, RT, TC, V1, LB, VL, LC, RB, BC, RC, MC};
static std::wstring info_str[N_INFO];

static int m, n;
static int (*maze)(int, int);
static int info[MAX_M][MAX_N];

static std::vector<std::wstring> border;

bool check_bidir() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(m, n, u, v)) {
                    int rd = rev_dir(d);
                    bool w1 = maze(i, j) & (1 << d);
                    bool w2 = maze(u, v) & (1 << rd);

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
        
        wchar_t c = (maze(i, j) & (1 << 3))?SP:ST;
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

        if (maze(i, j) & 1)
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

void gotoxy(int x, int y) {
    std::wcout << L"\x1B[" << x << L";" << y << L"f";
}

void clear_screen(){
    if(system("clear")) {
        throw std::runtime_error("Cannot clear screen");
    }
}

void set_cursor(bool on) {
    if (on)
        std::wcout << L"\e[?25h";
    else
        std::wcout << L"\e[?25l";
}

std::tuple<int, int, int, int> get_cell_reg(int i, int j) {
    return {i     * (CELL_H + 1) + 2, j     * (CELL_W + 1) + 2, 
            (i+1) * (CELL_H + 1), (j+1) * (CELL_W + 1)};
}

void draw_cell(int i, int j) {
    int l, r, u, d;
    std::tie(u, l, d, r) = get_cell_reg(i, j);

    for (int row = u; row <= d; ++row) {
        gotoxy(row, l);
        for (int col = l; col <= r; ++col)
            std::wcout << info_str[info[i][j]];
    }

    std::wcout.flush();
}

void draw_border() {
    gotoxy(1, 1);
    std::vector<std::wstring> border = get_border();

    for (int i = 0; i < border.size(); ++i)
        std::wcout << border[i] << std::endl;
}

void draw_cell() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            draw_cell(i, j);
        }
    }
}

void wait(int milisec){
    std::this_thread::sleep_for(std::chrono::milliseconds(milisec));
}

void set_info_str(int info, const wchar_t *str) {
    info_str[info] = std::wstring(str);
}

void init(int m_, int n_, int (*maze_)(int, int)) {
    m = m_;
    n = n_;
    maze = maze_;

    for (int i = 0; i < N_INFO; ++i)
        info_str[i] = L" ";

    if (!check_bidir())
        throw std::runtime_error("Wrong input");
}

int get_info(int i, int j) {
    return info[i][j];
}

void set_info(int i, int j, int v) {
    info[i][j] = v;
    draw_cell(i,j);
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