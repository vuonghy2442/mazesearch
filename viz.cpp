#include <iostream>
#include <string>
#include <vector>
#include "viz.h"
#include "common.h"
#include <chrono>
#include <thread>

#define CELL_W 3
#define CELL_H 1

#define RB L"\u2518" // 188 Right Bottom corner
#define RT L"\u2510" // 187 Right Top corner
#define LT L"\u250C" // 201 Left Top cornet
#define LB L"\u2514" // 200 Left Bottom corner
#define MC L"\u253C" // 206 Midle Cross
#define HL L"\u2500" // 205 Horizontal Line
#define H1 L"\u2574" // 205 Horizontal Half Line Left
#define H2 L"\u2576" // 205 Horizontal Half Line Right
#define LC L"\u251C" // 204 Left Cross
#define RC L"\u2524" // 185 Right Cross
#define BC L"\u2534" // 202 Bottom Cross
#define TC L"\u252C" // 203 Top Cross
#define VL L"\u2502" // 186 Vertical Line
#define V1 L"\u2575" // 186 Vertical Half Line Top
#define V2 L"\u2577" // 186 Vertical Half Line Bottom

#define SP L" " 		  // space string

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

static const std::wstring box_str[] = {SP, H2, V2, LT, H1, HL, RT, TC, V1, LB, VL, LC, RB, BC, RC, MC};
static std::wstring info_str[N_INFO];

static int m, n;
static int (*maze)(int, int);
static int (*info)(int, int);

static std::vector<std::string> maze_viz;

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

std::string draw_upper(int i) {

    std::string s;

    for(int j = 0; j < n; ++j) {
        s += "*";
        
        char c = '*';
        if (maze(i, j) & (1 << 3))
            c = ' ';
        
        s += std::string(CELL_W, c);
    }
    s += "*";
    return s;
}

std::string draw_border(int i) {
    std::string s;
    
    s += "*";
    for(int j = 0; j < n; ++j) {

        s += std::string(CELL_W, (char)info(i, j));

        if (maze(i, j) & 1)
            s += " ";
        else
            s += "*";
    }
    return s;
}

void viz() {
    //boundary
    for (int i = 0; i < m; ++i) {
        maze_viz.push_back(draw_upper(i));

        std::string s = draw_border(i);
        for (int k = 0; k < CELL_H; ++k)
            maze_viz.push_back(s);
    }
    maze_viz.push_back(std::string((CELL_W + 1) * n + 1,'*'));
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


void output() {

    const int h = maze_viz.size();
    if (!h)
        return;

    const int w = maze_viz[0].length();


    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (maze_viz[i][j] < N_INFO) {
                std::wcout << info_str[maze_viz[i][j]];
                continue;
            }

            if (maze_viz[i][j] == ' ') {
                int info = N_INFO;

                for (int d = 0; d < 4; ++d) {
                    int u = i + dr[d];
                    int v = j + dc[d];

                    if (inside(h, w, u, v)) {
                        if (maze_viz[u][v] < N_INFO && info > maze_viz[u][v])
                            info = maze_viz[u][v];
                    }
                }

                if (info < N_INFO)
                    std::wcout << info_str[info];
                else
                    std::wcout << L" ";

                continue;
            }

            int type = 0;
            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(h, w, u, v) && maze_viz[u][v] == '*')
                    type |= (1 << d);
            }

            std::wcout << box_str[type];
        }
        std::wcout << std::endl;
    }
}

void draw() {
    maze_viz.clear();
    viz();
    output();
}

void wait(int milisec){
    std::this_thread::sleep_for(std::chrono::milliseconds(milisec));
}

void set_info_str(int info, const wchar_t *str) {
    info_str[info] = std::wstring(str);
}

void init(int m_, int n_, int (*maze_)(int, int), int (*info_)(int, int)) {
    m = m_;
    n = n_;
    maze = maze_;
    info = info_;

    for (int i = 0; i < N_INFO; ++i)
        info_str[i] = L" ";

    if (!check_bidir())
        throw std::runtime_error("Wrong input");
}