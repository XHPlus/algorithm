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

void parse_input(char *filename, int distance_matrix[][MAX_NODE_NUM])
{
    ifstream f(filename);
    for (int i = 0; i < MAX_NODE_NUM; i++) {
        for (int j = 0; j < MAX_NODE_NUM; j++) {
            f >> distance_matrix[i][j];
        }
    }
}

void print_result(int current_shortest_dist, vector<int> current_shortest_path, int cost_matrix[][MAX_NODE_NUM])
{
    cout << "shortest_distance: " << current_shortest_dist << endl;
    cout << "shortest_path: ";
    for (int i = 0; i < current_shortest_path.size(); i++) {
        if (i == current_shortest_path.size() - 1)
            cout << current_shortest_path[i]+1;
        else
            cout << current_shortest_path[i]+1 << "->";
    }
    cout << endl;
    int sum = 0;
    for (int i = 1; i < current_shortest_path.size(); i++) {
        sum += cost_matrix[current_shortest_path[i-1]][current_shortest_path[i]];
    }
    cout << "cost: " << sum << endl;
}

void dijkstra(int distance_matrix[][MAX_NODE_NUM], int source,
              int shortest_distance[], int shortest_path[])
{
    // found用于标记已经得到最短距离的目标结点
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

}

void search(vector<int> sequence, vector<int> remove,
            int sequence_distance, int sequence_cost,
            int &current_shortest_dist, vector<int> &current_shortest_path,
            int shortest_path[][MAX_NODE_NUM],
            int shortest_distance[][MAX_NODE_NUM], int min_cost[][MAX_NODE_NUM],
            int distance_matrix[][MAX_NODE_NUM], int cost_matrix[][MAX_NODE_NUM])
{
    int seq_len = sequence.size();
    int last_node = sequence[seq_len - 1];
    int opt_distance = sequence_distance + shortest_distance[last_node][MAX_NODE_NUM-1];
    int opt_cost = sequence_cost + min_cost[last_node][MAX_NODE_NUM-1];
    // 符合剪枝条件，则回溯
    //   0. 上一步搜索时向左分支
    //   1. shortest_distance[last_node][MAX_NODE_NUM-1] > 0 && sequence_distance + shortest_distance[sequence[-1]][MAX_NODE_NUM-1] > current best solution
    //   2. sequence_cost + min_cost[sequence[-1]][MAX_NODE_NUM-1] > 1500
    //   剪枝条件: 0 且 (1或2)
    if (remove.size() == 0 && ((shortest_distance[last_node][MAX_NODE_NUM-1] > 0 &&
        opt_distance >= current_shortest_dist) || opt_cost > COST_UPPER_BOUND)) {
        // 剪枝回溯
        return;
    } else {
        // 判断是否是可行解
        if (remove.size() == 0 && shortest_distance[last_node][MAX_NODE_NUM-1] > 0) {
            // 上一步是向左分支且当前序列去往B的后继路径存在时，才可能产生可行解
            // 合并整个路径
            vector<int> tmp_shortest_path = sequence;
            int _last = shortest_path[sequence[sequence.size()-1]][MAX_NODE_NUM-1];
            vector<int> tmp_path;
            tmp_path.push_back(49);
            while (_last != sequence[sequence.size()-1]) {
                tmp_path.push_back(_last);
                _last = shortest_path[sequence[sequence.size()-1]][_last];
            }
            reverse(tmp_path.begin(), tmp_path.end());
            tmp_shortest_path.insert(tmp_shortest_path.end(), tmp_path.begin(), tmp_path.end());
            // 计算该路径的费用代价
            int total_cost = 0;
            for (int i = 1; i < tmp_shortest_path.size(); i++) {
                total_cost += cost_matrix[tmp_shortest_path[i-1]][tmp_shortest_path[i]];
            }
            // 若费用满足要求，则更新最优解
            if (total_cost <= COST_UPPER_BOUND) {
                current_shortest_dist = opt_distance;
                current_shortest_path = tmp_shortest_path;
            }
        }
        // 继续前进
        // 二叉树
        // 找到不在sequence和remove中且可以与last_node连通的结点c
        for (int i = 0; i < MAX_NODE_NUM; i++){
            vector<int>::iterator it_seq;
            vector<int>::iterator it_rm;
            it_seq = find(sequence.begin(), sequence.end(), i);
            it_rm = find(remove.begin(), remove.end(), i);
            if (i != last_node && it_seq == sequence.end() && it_rm == remove.end() &&
                distance_matrix[last_node][i] < INT_MAX_VALUE) {
                // 左子树
                sequence.push_back(i);
                vector<int> tmp_remove;
                search(sequence, tmp_remove, sequence_distance+distance_matrix[sequence[sequence.size()-2]][sequence[sequence.size()-1]], sequence_cost+cost_matrix[sequence[sequence.size()-2]][sequence[sequence.size()-1]], current_shortest_dist, current_shortest_path, shortest_path, shortest_distance, min_cost, distance_matrix, cost_matrix);
                // 右子树
                sequence.pop_back();
                remove.push_back(i);
                search(sequence, remove, sequence_distance, sequence_cost, current_shortest_dist, current_shortest_path, shortest_path, shortest_distance, min_cost, distance_matrix, cost_matrix);
                break;
            }
        }
    }
}

int main()
{
    int distance_matrix[MAX_NODE_NUM][MAX_NODE_NUM]; // 距离矩阵
    int cost_matrix[MAX_NODE_NUM][MAX_NODE_NUM]; // 费用矩阵
    int shortest_distance[MAX_NODE_NUM-1][MAX_NODE_NUM]; // 记录各节点间的最短路径长度
    int min_cost[MAX_NODE_NUM-1][MAX_NODE_NUM]; // 记录各节点间的最小费用
    int shortest_path[MAX_NODE_NUM-1][MAX_NODE_NUM]; // 记录各节点间的最短路径
    int min_cost_path[MAX_NODE_NUM-1][MAX_NODE_NUM]; // 记录各节点间的最小费用对应的路径

    memset(shortest_distance, 0, sizeof(int)*MAX_NODE_NUM*MAX_NODE_NUM);

    char distance_file[] = "m1.txt";
    char cost_file[] = "m2.txt";
    parse_input(distance_file, distance_matrix);
    parse_input(cost_file, cost_matrix);
    // 用dijkstra算法找出在没有其余条件限制下，各节点到B的最短路径
    for (int i = 0; i < MAX_NODE_NUM - 1; i++) {
        dijkstra(distance_matrix, i, shortest_distance[i], shortest_path[i]);
    }
    // 用dijkstra算法找出在没有其余条件限制下，各节点到B的最小费用
    for (int i = 0; i < MAX_NODE_NUM - 1; i++) {
        dijkstra(cost_matrix, i, min_cost[i], min_cost_path[i]);
    }

    // 分支定界法求有过路费约束的从A到B的最短路径
    vector<int> sequence;
    vector<int> remove;
    vector<int> current_shortest_path;
    sequence.push_back(0);
    int current_shortest_dist = INT_MAX_VALUE;
    search(sequence, remove, 0, 0, current_shortest_dist, current_shortest_path, shortest_path, shortest_distance, min_cost, distance_matrix, cost_matrix);
    print_result(current_shortest_dist, current_shortest_path, cost_matrix);

    return 0;
}
