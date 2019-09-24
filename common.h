#ifndef _COMMON_
#define _COMMON_

#define MAX_M 100
#define MAX_N 100

static const int dr[4] = {0, 1, 0, -1};
static const int dc[4] = {1, 0, -1, 0};

inline bool inside(int m, int n, int i, int j){
    return (i >= 0) && (j >= 0) && (i < m) && (j < n);
}

inline int rev_dir(int d) {
    return d^2;
}

#endif