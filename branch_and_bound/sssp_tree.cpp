/*
 * solve single source shortest path problem using branch and bound algorithm
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>

#define MAX_NODE_NUM 50
#define INT_MAX_VALUE 9999
#define COST_UPPER_BOUND 1500

using std::ifstream;
using std::vector;
using std::find;
using std::cout;
using std::endl;
using std::reverse;

class Path
{
    public:
        int distance;
        int cost;
        vector<int> nodes;

        Path();
        void add_node(int node, int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM]);
        void set_path(int source, int shortest_distance[], int shortest_path[]);
        void update_cost(int cost_matrix[][MAX_NODE_NUM]);
        int length();
        int last();
        void add_path(Path path, int start);
        bool has(int node);
        void pop_back(int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM]);
};

Path::Path()
{
    this->distance = 0;
    this->cost = 0;
}

void Path::add_node(int node, int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM])
{
    this->nodes.push_back(node);
    int node_num = this->nodes.size();
    if (node_num >= 2) {
        int start = this->nodes[node_num-2];
        int end = this->nodes[node_num-1];
        this->distance += distance_matrix[start][end];
        this->cost += cost_matrix[start][end];
    }
}

void Path::set_path(int source, int shortest_distance[], int shortest_path[])
{
    int i = MAX_NODE_NUM-1;
    do {
        this->nodes.push_back(i);
        i = shortest_path[i];
    } while (i != source);
    if (source != MAX_NODE_NUM-1)
        this->nodes.push_back(i);
    reverse(this->nodes.begin(), this->nodes.end());
    this->distance = shortest_distance[MAX_NODE_NUM-1];
}

void Path::update_cost(int cost_matrix[][MAX_NODE_NUM])
{
    this->cost = 0;
    for (int i = 1; i < this->nodes.size(); i++) {
        this->cost += cost_matrix[this->nodes[i-1]][this->nodes[i]];
    }
}

int Path::length()
{
    return this->nodes.size();
}

int Path::last()
{
    return this->nodes[this->nodes.size()-1];
}

void Path::add_path(Path path, int start)
{
    this->nodes.insert(this->nodes.end(), path.nodes.begin()+start, path.nodes.end());
    this->distance += path.distance;
    this->cost += path.cost;
}

bool Path::has(int node)
{
    bool flag = false;
    for (int i = 0; i < this->nodes.size(); i++) {
        if (node == this->nodes[i]) {
            flag = true;
            break;
        }
    }
    return flag;
}

void Path::pop_back(int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM])
{
    int last = this->nodes.size();
    int start = this->nodes[last-2];
    int end = this->nodes[last-1];
    this->distance -= distance_matrix[start][end];
    this->cost -= cost_matrix[start][end];
    this->nodes.pop_back();
}

void parse_input(char *filename, int distance_matrix[][MAX_NODE_NUM])
{
    ifstream f(filename);
    for (int i = 0; i < MAX_NODE_NUM; i++) {
        for (int j = 0; j < MAX_NODE_NUM; j++) {
            f >> distance_matrix[i][j];
        }
    }
}

void print_path(Path path)
{
    if (path.length() == 0) {
        cout << "No Solution" << endl;
        return;
    }
    cout << "path: ";
    for (int i = 0; i < path.length()-1; i++) {
        cout << path.nodes[i]+1 << "->";
    }
    cout << path.last()+1 << endl;
    cout << "distance: " << path.distance << endl;
    cout << "cost: " << path.cost << endl;
}

void dijkstra(int distance_matrix[][MAX_NODE_NUM], int source, Path &path)
{
    // found用于标记已经得到最短距离的目标结点
    int shortest_distance[MAX_NODE_NUM];
    int shortest_path[MAX_NODE_NUM];
    static int found[MAX_NODE_NUM];
    // 初始化为1
    memset(found, 0, sizeof(found));

    // 找出从源点到其相邻节点的最短路径
    found[source] = 1;
    for (int i = 0; i < MAX_NODE_NUM; i++) {
        shortest_distance[i] = distance_matrix[source][i];
        shortest_path[i] = source;
    }

    int min_distance;
    int min_distance_idx;

    // 找到从源点出发到其他各结点的最短路径
    for (int i = 1; i < MAX_NODE_NUM; i++) {
        min_distance = INT_MAX_VALUE;
        // 找到未被标记的路径最短的结点
        for (int j = 0; j < MAX_NODE_NUM; j++) {
            if (found[j] == 0 && shortest_distance[j] < min_distance) {
                min_distance = shortest_distance[j];
                min_distance_idx = j;
            }
        }
        // 标记该结点为找到最短路径
        found[min_distance_idx] = 1;

        // 利用该该结点的最短路径更新其他结点的最短路径
        for (int j = 0; j < MAX_NODE_NUM; j++) {
            int dist_idx_j = distance_matrix[min_distance_idx][j]; // 从该结点到j的路径
            int new_distance = min_distance + dist_idx_j; // 从source到该结点的最短路径+从该结点到j的路径
            if (found[j] == 0 && dist_idx_j != INT_MAX_VALUE && new_distance < shortest_distance[j]) {
                shortest_distance[j] = new_distance;
                shortest_path[j] = min_distance_idx;
            }
        }
    }
    path.set_path(source, shortest_distance, shortest_path);

}

void search(Path sequence, Path &solution,
            Path shortest_path[], Path min_cost_path[],
            int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM])
{
    int last_node = sequence.last();
    int opt_distance = sequence.distance + shortest_path[last_node].distance;
    int opt_cost = sequence.cost + min_cost_path[last_node].cost;
    // 符合剪枝条件，则回溯
    //   1. sequence长度 + sequence末尾节点到终点的最短路径长度 > 当前最优解
    //   2. sequence费用 + min_cost[sequence[-1]][MAX_NODE_NUM-1] > 1500
    //   剪枝条件: 0 且 (1或2)
    if (((shortest_path[last_node].distance > 0 &&
        opt_distance >= solution.distance) || opt_cost > COST_UPPER_BOUND)) {
        // 剪枝回溯
        return;
    } else {
        // 判断是否是可行解
        if (shortest_path[last_node].distance > 0) {
            // 当前序列去往B的后继路径存在时，才可能产生可行解
            // 合并整个路径
            Path tmp;
            tmp.add_path(sequence, 0);
            tmp.add_path(shortest_path[last_node], 1);
            // 若费用满足要求，则更新最优解
            if (tmp.cost <= COST_UPPER_BOUND) {
                solution = tmp;
            }
        }
        // 继续前进
        // 找到不在sequence和且可以与last_node连通的结点c
        for (int i = 0; i < MAX_NODE_NUM; i++){
            if (!sequence.has(i) &&
                distance_matrix[last_node][i] < INT_MAX_VALUE) {
                // 左子树
                sequence.add_node(i, distance_matrix, cost_matrix);
                search(sequence, solution, shortest_path, min_cost_path, distance_matrix, cost_matrix);
                sequence.pop_back(distance_matrix, cost_matrix);
            }
        }
    }
}

int main()
{
    int distance_matrix[MAX_NODE_NUM][MAX_NODE_NUM]; // 距离矩阵
    int cost_matrix[MAX_NODE_NUM][MAX_NODE_NUM]; // 费用矩阵
    Path shortest_path[MAX_NODE_NUM-1]; // 记录各节点间的最短路径
    Path min_cost_path[MAX_NODE_NUM-1]; // 记录各节点间的最小费用对应的路径

    char distance_file[] = "m1.txt";
    char cost_file[] = "m2.txt";
    parse_input(distance_file, distance_matrix);
    parse_input(cost_file, cost_matrix);
    // 用dijkstra算法找出在没有其余条件限制下，各节点到B的最短路径
    for (int i = 0; i < MAX_NODE_NUM - 1; i++) {
        dijkstra(distance_matrix, i, shortest_path[i]);
        // 更新最短路径的费用
        shortest_path[i].update_cost(cost_matrix);
    }
    // 用dijkstra算法找出在没有其余条件限制下，各节点到B的最小费用
    for (int i = 0; i < MAX_NODE_NUM - 1; i++) {
        dijkstra(cost_matrix, i, min_cost_path[i]);
    }

    // 分支定界法求有过路费约束的从A到B的最短路径
    Path sequence;
    Path solution;
    solution.distance = INT_MAX_VALUE;
    sequence.add_node(0, distance_matrix, cost_matrix);
    int current_shortest_dist = INT_MAX_VALUE;
    search(sequence, solution, shortest_path, min_cost_path, distance_matrix, cost_matrix);
    print_path(solution);

    return 0;
}
