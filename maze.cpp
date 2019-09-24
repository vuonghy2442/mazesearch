#include <queue>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <tuple>
#include "common.h"

#define MAX_M 100
#define MAX_N 100

bool visit[MAX_M][MAX_N];
int maze[MAX_M][MAX_N]; //maze with binary

int m, n;

void get_random_order(int order[4]) {
    for (int i = 0; i < 4; ++i) {
        int j = rand() % (i + 1);
        order[i] = order[j];
        order[j] = i;
    }
}
//DFS
void dfs(int i, int j) {
    visit[i][j] = true;

    int order[4];
    get_random_order(order);

    for (int k = 0; k < 4; ++k) {
        int d = order[k];
        int u = i + dr[d];
        int v = j + dc[d];

        if (inside(m, n, u, v) && !visit[u][v]) {
            maze[i][j] |= (1 << d);
            maze[u][v] |= (1 << rev_dir(d));
            dfs(u, v);
        }
    }
}

/*void convert() {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            int type = 0;
            
            if (maze[i][j] >= 0)
                type |= (1 << rev_dir(maze[i][j] - 1));

            for (int d = 0; d < 4; ++d) {
                int u = i + dr[d];
                int v = j + dc[d];

                if (inside(m, n, u,v) && maze[u][v] == d + 1) {
                    type |= (1 << d);
                }
            }
            bmaze[i][j] = type;
        }
    }
}*/

void dfs() {
    dfs(0, 0);
}

void prim() {
    std::priority_queue<std::tuple<int, int, int, int>> q;
    q.push({0, 0, 0, -1});

    maze[0][0] = 0;

    while (!q.empty()) {
        int order[4];
        int i, j, d;
        std::tie(std::ignore,i, j, d) = q.top();
        q.pop();

        if (visit[i][j])
            continue;

        visit[i][j] = true;

        if (d >= 0) {
            int u = i - dr[d];
            int v = j - dc[d];
            maze[i][j] |= 1 << rev_dir(d);
            maze[u][v] |= 1 << d;
        }

        for (int d = 0; d < 4; ++d) {
            int u = i + dr[d];
            int v = j + dc[d];

            if (inside(m, n, u, v) && !visit[u][v]) {
                q.push({rand(), u, v, d});
            }
        }
    }
}

void kruskal() {

}

void print() {
    std::cout << m << " " << n << std::endl;
    std::cout << 0 << " " << 0 << " " << m-1 << " " << n-1 << std::endl;
    
    for(int i = 0; i < m; ++i){
        for (int j = 0; j < n; ++j)
            std::cout << maze[i][j] << " ";
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {

    if (argc != 4 && argc != 5) {
        std::cerr << "Usage: maze height width [dfs/prim] (seed)" << std::endl;
        return 1;
    }

    if (argc == 4)
        srand(time(NULL));
    else
        srand(atoi(argv[4]));

    m = atoi(argv[1]);
    n = atoi(argv[2]);

    if (argv[3] == std::string("dfs"))
        dfs();
    else if (argv[3] == std::string("prim"))
        prim();
    else {
        std::cerr << "Unsupported algorithm" << std::endl;
        return 1;
    }

    print();
}