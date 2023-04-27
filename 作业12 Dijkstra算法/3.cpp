#include <iostream>
#include <limits.h>
using namespace std;

// 定义一个表示图的邻接矩阵的结构体
struct Graph {
    int V; // 图中节点数量
    int **adj; // 邻接矩阵

    // 添加边（无向图）
    void addNoDirectionEdge(int u, int v, int w) {
        adj[u][v] = w;
        adj[v][u] = w;
    }
    // 添加边（有向图）
    void addDirectionEdge(int u, int v, int w) {
        adj[u][v] = w;
    }

};

// 创建图
Graph* createGraph(int V) {
    Graph* graph = new Graph;
    graph->V = V;
    graph->adj = new int*[V];
    for (int i = 0; i < V; ++i) {
        graph->adj[i] = new int[V];
        for (int j = 0; j < V; ++j)
            graph->adj[i][j] = 0;
    }
    return graph;
}



// 找到dist数组中的最小值
int minDistance(int dist[], bool sptSet[], int V) {
    int min = INT_MAX, min_index;
    for (int v = 0; v < V; ++v)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
    return min_index;
}

// 打印结果
void printSolution(int dist[], int V) {
    cout << "Distance from Source\n";
    for (int i = 1; i < V; ++i){
        // char ch = 'A' + i - 1;
        cout << i << "\t";
    }
    
        
    cout << endl;
    for (int i = 1; i < V; ++i){
        if (dist[i] == INT_MAX)
            cout << "INF" << "\t";
        else
            cout << dist[i] << "\t";
    }
    cout << endl;
}

// Dijkstra算法
void dijkstra(Graph* graph, int src) {
    cout << "Dijkstra Algorithm\n";
    int V = graph->V;
    int dist[V];
    bool sptSet[V];
    for (int i = 0; i < V; ++i)
        dist[i] = INT_MAX, sptSet[i] = false;
    dist[src] = 0;
    for (int count = 0; count < V - 1; ++count) {
        int u = minDistance(dist, sptSet, V);
        sptSet[u] = true;
        cout << u << "is added to SPT\n";
        for (int v = 0; v < V; ++v) {
            // sptSet是Dijkstra算法中的一个bool类型的数组，用于表示每个节点是否已经被加入最短路径树（SPT）中。
            // 只有当sptSet[v]为false时，并且存在一条从u到v的边，且u节点的最短路径树中的距离加上u到v的边的权值小于dist[v]时，才更新dist[v]。
            if (!sptSet[v] && graph->adj[u][v] && dist[u] != INT_MAX && dist[u] + graph->adj[u][v] < dist[v])
                dist[v] = dist[u] + graph->adj[u][v];
        }
        // 打印中间结果
        cout << "Step " << count+1 << ":" << endl;
        printSolution(dist, V);
    }
    // 打印最终结果
    cout << "Final Result:" << endl;
    printSolution(dist, V);
}

// 定义一个map，把节点的名字映射到节点的编号


int main(){
    int V;
    cin >> V;
    Graph* graph = createGraph(V);
    int u, v, w;
    while (cin >> u >> v >> w) {
        graph->addDirectionEdge(u, v, w);
    }
    dijkstra(graph, 1);

    return 0;
}