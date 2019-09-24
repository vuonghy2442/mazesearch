#include <iostream>
#include "viz.h"
#include "common.h"
#include <signal.h>
#include <fstream>
#include <queue>
#include <tuple>

//static volatile int keepRunning = 1;

static volatile bool run = false;

void intHandler(int dummy) {
    if (run) {
        run = false;
        return;
    }

    set_cursor(true);
    exit(dummy);
    //keepRunning = 0;
}

#define CURRENT_FLAG 4
#define GOAL_FLAG 8


#define DFS_VISITED_FLAG 1
#define DFS_PATH_FLAG 2
#define BFS_QUEUE_FLAG 1
#define BFS_VISITED_FLAG 2



int m, n;
int sr, sc;
int er, ec;
int maze[MAX_M][MAX_N];

static int wait_time = 300;


void read(const char *file) {
    std::ifstream fin;
    fin.open(file, std::ios_base::openmode::_S_in);

    fin >> m >> n;
    fin >> sr >> sc >> er >> ec;

    for (int i = 0; i < m; ++i)
        for(int j = 0; j < n; ++j)
            fin >> maze[i][j];

}

int get_maze(int i, int j) {
    return maze[i][j];
}

void wait_meo() {
    gotoend();

    if (run)
        wait(wait_time);
    else {
        wchar_t s;
        do {
            std::wcin.get(s);
        } while (s != 'r' && s != 's');

        if (s == 'r') {
            std::wcin >> wait_time;
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

void meo(int i, int j) {
    gotoxy(0, 0);

    set_flag(i, j, CURRENT_FLAG);
    wait_meo();
    reset_flag(i, j, CURRENT_FLAG);
}

static int dfs_depth = INT32_MAX;

bool dfs(int i, int j, int depth) {
    set_flag(i, j, DFS_VISITED_FLAG);
    set_flag(i, j, DFS_PATH_FLAG); //flag for current path

    if (get_info(i,j) & GOAL_FLAG) {
        //goal here
        meo(i, j);
        return true;
    }

    if (depth < dfs_depth) {    

        for (int d = 0; d < 4; ++d) {
            if(!(maze[i][j] & (1 << d)))
                continue;

            int u = i + dr[d];
            int v = j + dc[d];

            if (!(get_info(u,v) & DFS_VISITED_FLAG)) {
                meo(i, j);
                if (dfs(u, v, depth + 1))
                    return true;
            }
        }

    }

    meo(i, j);

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

void bfs(int i, int j) {
    std::queue<std::tuple<int, int>> q;

    q.push({i, j});
    set_flag(i, j, BFS_QUEUE_FLAG);


    while (!q.empty()) {
        std::tie(i, j) = q.front();
        q.pop();

        set_flag(i, j, BFS_VISITED_FLAG);
        reset_flag(i, j, BFS_QUEUE_FLAG);

        meo(i, j);

        if (get_info(i,j) & GOAL_FLAG) {
            //goal here
            return;
        }
        
        bool expand = false;

        for (int d = 0; d < 4; ++d) {
            if(!(maze[i][j] & (1 << d)))
                continue;
    
            int u = i + dr[d];
            int v = j + dc[d];

            if (!(get_info(u,v) & (BFS_VISITED_FLAG | BFS_QUEUE_FLAG))) {
                expand = true;
                q.push({u,v});
                set_flag(u, v, BFS_QUEUE_FLAG);
            }
        }

        if (expand)
            meo(i,j);
    }
}

void color_dfs() {
    set_info_str(0, L" ");
    set_info_str(DFS_VISITED_FLAG, L"\x1B[48;5;5m \x1B[49m");
    set_info_str(DFS_VISITED_FLAG | DFS_PATH_FLAG, L"\x1B[48;5;1m \x1B[49m");
    set_info_str(DFS_VISITED_FLAG | DFS_PATH_FLAG | CURRENT_FLAG, L"\x1B[48;5;2m \x1B[49m");
    set_info_str(DFS_VISITED_FLAG | DFS_PATH_FLAG | CURRENT_FLAG | GOAL_FLAG, L"\x1B[48;5;4m \x1B[49m");
}

void color_bfs() {
    set_info_str(0, L" ");
    set_info_str(BFS_QUEUE_FLAG, L"\x1B[48;5;1m \x1B[49m"); //on queue
    set_info_str(BFS_VISITED_FLAG, L"\x1B[48;5;5m \x1B[49m"); //visited
    set_info_str(BFS_VISITED_FLAG | CURRENT_FLAG, L"\x1B[48;5;2m \x1B[49m"); //visited & current on
    set_info_str(BFS_QUEUE_FLAG | GOAL_FLAG, L"\x1B[48;5;7m \x1B[49m"); //goal on queue
    set_info_str(BFS_VISITED_FLAG | CURRENT_FLAG | GOAL_FLAG, L"\x1B[48;5;4m \x1B[49m"); //currently on goal
}

void run_search(const char *al) {
    //std::ios_base::sync_with_stdio(false);
    set_cursor(false);

    clear_screen();
    draw_border();

    set_info_str(GOAL_FLAG, L"\x1B[48;5;3m \x1B[49m"); //goal
    set_flag(er, ec, GOAL_FLAG);


    if (al == std::string("dfs")) {
        color_dfs();
        normal_dfs(sr, sc);
    } else if (al == std::string("idfs")) {
        color_dfs();
        incremental_dfs(sr, sc);
    } else if (al == std::string("bfs")) {
        color_bfs();
        bfs(sr,sc);
    } else {
        std::wcerr << L"Unsupported algorithm" << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) { 
        std::wcerr << L"Usage: run maze_file [bfs/dfs/idfs]" << std::endl;
        std::wcerr << L"Input 'r wait_time_in_ms\\n' to run" << std::endl;
        std::wcerr << L"Input 's\\n' to do step" << std::endl;
        std::wcerr << L"Press Ctrl+C to pause run" << std::endl;

        return 1;
    }
    

    signal(SIGINT, intHandler);
    setlocale(LC_ALL, "");

    read(argv[1]);
    init(m, n, get_maze);

    run_search(argv[2]);


    set_cursor(true);
}