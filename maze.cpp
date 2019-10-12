#include <queue>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <tuple>
#include "common.h"


bool visit[MAX_M][MAX_N];
int maze[MAX_M][MAX_N]; //maze with binary
double loop = 0;

int m, n;

void get_random_order(int order[4]) {
    for (int i = 0; i < 4; ++i) {
        int j = rand() % (i + 1);
        order[i] = order[j];
        order[j] = i;
    }
}

void add_edge(int i, int j, int d) {
    int u = i + dr[d];
    int v = j + dc[d];
    maze[i][j] |= (1 << d);
    maze[u][v] |= (1 << rev_dir(d));
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

        if (inside(m, n, u, v)) {
            if (!visit[u][v] || random() < loop * RAND_MAX )
                add_edge(i, j, d);

            if (!visit[u][v])
                dfs(u, v);
        }
    }
}

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

        if (d >= 0 && (!visit[i][j] || random() < loop * RAND_MAX ))
            add_edge(i, j, rev_dir(d));

        if (visit[i][j])
            continue;

        visit[i][j] = true;

        for (int d = 0; d < 4; ++d) {
            int u = i + dr[d];
            int v = j + dc[d];

            if (inside(m, n, u, v) && !visit[u][v])
                q.push({rand(), u, v, d});
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

    //random weight
    
    for(int i = 0; i < m; ++i){
        for (int j = 0; j < n; ++j) 
            if ((i == m - 1 && j == n - 1) || (i == 0 && j == 0))
                std::cout << 0 << " ";
            else
                std::cout << random() % 5 << " ";

        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {

    if (argc != 5 && argc != 6) {
        std::cerr << "Usage: maze height width loop [dfs/prim] (seed)" << std::endl;
        return 1;
    }

    if (argc == 5)
        srand(time(NULL));
    else
        srand(atoi(argv[5]));

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    loop = atof(argv[3]);

    if (argv[4] == std::string("dfs"))
        dfs();
    else if (argv[4] == std::string("prim"))
        prim();
    else {
        std::cerr << "Unsupported algorithm" << std::endl;
        return 1;
    }

    print();
}