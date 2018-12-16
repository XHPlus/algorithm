#include <iostream>
#include <limits.h>

using namespace std;
// number of cities
#define N  6
#define N_S (1<<N)

void tsp(int d[][N])
{
    int D[N_S][N];
    int choose[N_S][N];

    // initialize D
    for (int i = 0; i < N_S; i++) {
        for (int j = 0; j < N; j++) {
            D[i][j] = -1;
        }
    }
    //put v1 to S
    D[1][0] = 0;
    for (int s = 1; s < N_S; s++) {
        for (int i = 0; i < N; i++) {
            // i not in S
            if (D[s][i] == -1)
                continue;
            // path from i to j
            for (int j = 1; j < N; j++) {
                // j in S
                if ((s & (1<<j)) != 0)
                    continue;
                int s_new = s | (1<<j);
                if (D[s_new][j] == -1 || D[s_new][j] >= D[s][i] + d[i][j]) {
                    D[s_new][j] = D[s][i] + d[i][j];
                    choose[s_new][j] = i;
                }
            }
        }
    }

    // from last city to v1
    int shortest_dis = INT_MAX;
    int last_city = -1;
    for (int i = 1; i < N; i++) {
        if (D[N_S-1][i] > 0) {
            if (D[N_S-1][i] + d[i][0] < shortest_dis) {
                shortest_dis = D[N_S-1][i] + d[i][0];
                last_city = i;
            }
        } else {
            cout << "[WARNING] No path to " << i+1 << " as last vertex" << endl;
        }
    }

    // recover path
    int path[N];
    path[N-1] = last_city;
    for (int i = N-1, s=N_S-1; i > 0 && s > 0; i--) {
        path[i-1] = choose[s][path[i]];
        s = s - (1<<path[i]);
    }

    // print path and shortest path
    cout << "Path: ";
    for (int i = 0; i < N; i++) {
        cout << path[i]+1 << " -> ";
    }
    cout << "1" << endl;
    cout << "Distance: " << shortest_dis << endl;
}

int main()
{
    int d[N][N] = {0,  10, 20, 30, 40, 50,
                   12,  0, 18, 30, 25, 21,
                   23, 19,  0,  5, 10, 15,
                   34, 32,  4,  0,  8, 16,
                   45, 27, 11, 10,  0, 18,
                   56, 22, 16, 20, 12, 0};

    // dp solve tsp
    tsp(d);
    return 0;
}
