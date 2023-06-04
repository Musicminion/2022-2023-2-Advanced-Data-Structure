#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// 生成无向联通图
vector<vector<int>> generateConnectedGraph(int n, int m) {
    vector<vector<int>> graph(n, vector<int>(n, 0));  // 邻接矩阵表示图
    srand(time(0));  // 设置随机数种子

    // 创建连通图
    for (int i = 1; i < n; i++) {
        int j = rand() % i;  // 随机选择一个已存在的节点
        int weight = rand() % 10 + 1;  // 随机生成边的权重（1到10之间的整数）
        graph[i][j] = graph[j][i] = weight;  // 添加边及其权重
    }

    // 添加额外的边，直到达到m条边
    int edgeCount = n - 1;
    while (edgeCount < m) {
        int u = rand() % n;  // 随机选择一个节点
        int v = rand() % n;  // 随机选择另一个节点

        if (u != v && graph[u][v] == 0) {  // 确保节点不同且边不存在
            int weight = rand() % 1000 + 1;  // 随机生成边的权重（1到10之间的整数）
            graph[u][v] = graph[v][u] = weight;  // 添加边及其权重
            edgeCount++;
        }
    }

    return graph;
}

// 将图信息输出到文本文件
void writeGraphToFile(const vector<vector<int>>& graph, int n, int m) {
    // string filename = "graph_info.txt";  // 输出文件名
    
    string filename = to_string(n)+"_"+to_string(m)+".txt";
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Failed to create file: " << filename << endl;
        return;
    }

    // 写入顶点数和边数
    outFile << n << " " << m << endl;

    // 写入边的信息
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph[i].size(); j++) {
            if (graph[i][j] > 0) {
                outFile << i << " " << j << " " << graph[i][j] << endl;
            }
        }
    }

    outFile.close();
}

void wrap(int n, int m){
    if (m < n - 1 || m > (n - 1) * (n - 1) / 2) {
        cerr << "Invalid number of edges: " << m << endl;
        return;
    }

    vector<vector<int>> graph = generateConnectedGraph(n, m);
    writeGraphToFile(graph, n, m);

    cout << "Graph information has been written to "<<n <<"_"<<m<<".txt" << endl;
}

int main() {
    // int n, m;
    // cout << "Enter the number of nodes (n): ";
    // cin >> n;
    // cout << "Enter the number of edges (m): ";
    // cin >> m;

    //  10000、100000、1000000
    wrap(2000,10000);
    wrap(2000,100000);
    wrap(2000,1000000);

    wrap(5000,10000);
    wrap(5000,100000);
    wrap(5000,1000000);

    wrap(10000,10000);
    wrap(10000,100000);
    wrap(10000,1000000);

    return 0;
}
