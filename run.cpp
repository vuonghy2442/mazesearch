#include <iostream>
#include <signal.h>
#include <fstream>
#include <queue>
#include <tuple>
#include "viz.h"
#include "common.h"
#include "ui.h"

static volatile bool run = false;

void intHandler(int dummy) {
    if (run) {
        run = false;
        return;
    }

    show_cursor(true);
    exit(dummy);
}

#define CURRENT_FLAG 8
#define GOAL_FLAG 16

#define DFS_VISITED_FLAG 1
#define DFS_PATH_FLAG 2
#define UCS_QUEUE_FLAG 1
#define UCS_VISITED_FLAG 2
#define UCS_PATH_FLAG 4

int m, n;
int sr, sc;
int er, ec;
int maze[MAX_M][MAX_N];
int cost[MAX_M][MAX_N];


static int wait_time = 300;

void read(const char *file, bool read_cost) {
    std::ifstream fin;
    fin.open(file);

    fin >> m >> n;
    fin >> sr >> sc >> er >> ec;

    for (int i = 0; i < m; ++i)
        for(int j = 0; j < n; ++j)
            fin >> maze[i][j];

    if (read_cost) {
        for (int i = 0; i < m; ++i)
            for(int j = 0; j < n; ++j)
                fin >> cost[i][j];
    }
}

void wait() {
    gotoend();

    if (run)
        wait(wait_time);
    else {
        char s;
        do {
            std::cin.get(s);
        } while (s != 'r' && s != 's');

        if (s == 'r') {
            std::cin >> wait_time;
            run = true;
        }
        
    }
}

void toggle_flag(int i, int j, int flag) {
    int info = get_info(i,j) ^ flag;
    set_info(i, j, info);
}

void set_flag(int i, int j, int flag) {
    int info = get_info(i,j) | flag;
    set_info(i, j, info);
}


void reset_flag(int i, int j, int flag) {
    int info = get_info(i,j) & ~flag;
    set_info(i, j, info);
}

void stay(int i, int j) {
    set_flag(i, j, CURRENT_FLAG);
    wait();
    reset_flag(i, j, CURRENT_FLAG);
}

static int dfs_depth = INT32_MAX;

bool dfs(int i, int j, int depth) {
    set_flag(i, j, DFS_VISITED_FLAG);
    set_flag(i, j, DFS_PATH_FLAG); //flag for current path

    set_value(i, j, depth);
    show_value(i,j);

    if (get_info(i,j) & GOAL_FLAG) {
        //goal here
        stay(i, j);
        return true;
    }

    if (depth < dfs_depth) {    

        for (int d = 0; d < 4; ++d) {
            if(!(maze[i][j] & (1 << d)))
                continue;

            int u = i + dr[d];
            int v = j + dc[d];

            if (!(get_info(u,v) & DFS_VISITED_FLAG)) {
                stay(i, j);
                draw_edge(i, j, d);
                if (dfs(u, v, depth + 1))
                    return true;

                draw_edge(u, v, rev_dir(d));
            }
        }

    }

    stay(i, j);
    reset_flag(i, j, DFS_PATH_FLAG);

    return false;
}

void normal_dfs(int i, int j) {
    dfs_depth = INT32_MAX;
    dfs(i, j, 0);
}

void incremental_dfs(int i, int j) {

    dfs_depth = 0;

    while (!dfs(i, j, 0)) {
        ++dfs_depth;
        clear_info();
        set_flag(er, ec, GOAL_FLAG);
    }
}

void ucs(int sr, int sc) {
    std::priority_queue<std::tuple<int, int, int, int>, 
                        std::vector<std::tuple<int, int, int, int>>,
                        std::greater<std::tuple<int, int, int, int>> > q;

    int i = sr;
    int j = sc;

    q.push({0, i, j, -1});
    set_flag(i, j, UCS_QUEUE_FLAG);


    while (!q.empty()) {
        int depth;
        int d;

        std::tie(depth, i, j, d) = q.top();
        q.pop();

        if (get_info(i, j) & UCS_VISITED_FLAG)
            continue;

        cost[i][j] = rev_dir(d); //reuse this to save direction
        if (d >= 0)
            draw_edge(i - dr[d], j - dc[d], d);

        set_flag(i, j, UCS_VISITED_FLAG);
        reset_flag(i, j, UCS_QUEUE_FLAG);

        set_value(i, j, depth);
        show_value(i,j);


        stay(i, j);


        if (get_info(i,j) & GOAL_FLAG) {
            //goal here
            break;
        }
        
        bool expand = false;

        for (int d = 0; d < 4; ++d) {
            if(!(maze[i][j] & (1 << d)))
                continue;
    
            int u = i + dr[d];
            int v = j + dc[d];

            if (!(get_info(u,v) & UCS_VISITED_FLAG)) {
                int c = depth + cost[u][v] + 1;

                if (get_info(u,v) & UCS_QUEUE_FLAG) {
                    if (c < get_value(u,v)) {
                        set_value(u, v, c);
                        q.push({c, u, v, d});
                        expand = true;
                    } 
                } else {
                    set_flag(u, v, UCS_QUEUE_FLAG);

                    set_value(u, v, c);
                    show_value(u, v);

                    q.push({c, u, v, d});
                    expand = true;
                }

            }
        }

        if (expand)
            stay(i,j);
    }

    //i ,j  is goal
    if (get_info(i,j) & GOAL_FLAG)
        while (i != sr || j != sc) {
            int best_d = cost[i][j];
            i += dr[best_d];
            j += dc[best_d];

            set_flag(i, j, UCS_PATH_FLAG);
            draw_edge(i, j, rev_dir(best_d));

        }
}

void color_dfs() {
    set_info_color(DFS_VISITED_FLAG, 8);
    set_info_color(DFS_VISITED_FLAG | DFS_PATH_FLAG, 1);
    set_info_color(DFS_VISITED_FLAG | DFS_PATH_FLAG | CURRENT_FLAG, 2);
    set_info_color(DFS_VISITED_FLAG | DFS_PATH_FLAG | CURRENT_FLAG | GOAL_FLAG, 4);
    set_info_color(DFS_VISITED_FLAG | DFS_PATH_FLAG | GOAL_FLAG, 4);
}

void color_ucs() {
    set_info_color(UCS_QUEUE_FLAG, 1); //on queue
    set_info_color(UCS_VISITED_FLAG, 8); //visited
    set_info_color(UCS_VISITED_FLAG | CURRENT_FLAG, 2); //visited & current on
    set_info_color(UCS_QUEUE_FLAG | GOAL_FLAG, 7); //goal on queue
    set_info_color(UCS_VISITED_FLAG | CURRENT_FLAG | GOAL_FLAG, 4); //currently on goal
    set_info_color(UCS_VISITED_FLAG | GOAL_FLAG, 4);

    set_info_color(UCS_VISITED_FLAG | UCS_PATH_FLAG, 5);
}

void run_search(const char *al) {
    //std::ios_base::sync_with_stdio(false);
    show_cursor(false);

    clear_screen();
    draw_border();

    set_info_color(GOAL_FLAG, 3); //goal
    set_flag(er, ec, GOAL_FLAG);


    if (al == std::string("dfs")) {
        color_dfs();
        normal_dfs(sr, sc);
    } else if (al == std::string("idfs")) {
        color_dfs();
        incremental_dfs(sr, sc);
    } else if (al == std::string("bfs")) {
        color_ucs();
        ucs(sr,sc);
    } else if (al == std::string("ucs")) {
        color_ucs();
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n;  ++j)
                if (cost[i][j] > 0) {
                    set_value(i, j, cost[i][j] + 1);
                    show_value(i, j);
                }
        
        ucs(sr, sc);

    } else {
        std::cerr << "Unsupported algorithm" << std::endl;
    }
}

void reset() {
    gotoend();
    show_cursor(true);
}

int main(int argc, char **argv) {
    if (argc != 3) { 
        std::cerr << "Usage: run maze_file [bfs/dfs/idfs]" << std::endl;
        std::cerr << "Input 'r wait_time_in_ms\\n' to run" << std::endl;
        std::cerr << "Input 's\\n' to do step" << std::endl;
        std::cerr << "Press Ctrl+C to pause run" << std::endl;

        return 1;
    }

    signal(SIGINT, intHandler);
    setlocale(LC_ALL, "");

    read(argv[1], argv[2] == std::string("ucs"));
    init(m, n, [](int i,int j) {return maze[i][j];});
    run_search(argv[2]);

    reset();
}
