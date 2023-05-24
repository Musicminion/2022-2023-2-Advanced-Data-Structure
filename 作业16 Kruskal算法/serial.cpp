#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

class Edge{
public:
    int u;
    int v;
    int w;

    friend bool operator<(const Edge& E1,const Edge& E2){
        return E1.w < E2.w;
    }
};

vector<Edge> edges;//邻接矩阵


void MakeSet(vector<int>& uset,int n){
    uset.assign(n,0);
    for (int i = 0; i < n; i++)
        uset[i] = i;
}

int FindSet(vector<int>& uset,int u){
    int i = u;
    while (uset[i] != i) i = uset[i];
    return i;
}

void Kruskal(const vector<Edge>& edges,int n){
    vector<int> uset;
    vector<Edge> SpanTree;
    int Cost = 0,e1,e2;
    MakeSet(uset,n);
    for (int i = 0; i < edges.size(); i++){
        e1 = FindSet(uset,edges[i].u);
        e2 = FindSet(uset,edges[i].v);
        if (e1 != e2){
            SpanTree.push_back(edges[i]);
            Cost += edges[i].w;
            uset[e1] = e2;
        }
    }

    cout << "Result:\n";
    cout << "Cost: " << Cost << endl;
    cout << "Edges:\n";

    for (int j = 0; j < SpanTree.size(); j++){
        cout << SpanTree[j].u << " " << SpanTree[j].v << " " << SpanTree[j].w << endl;
    }
    cout << endl;

}

int main(){
    int n,m;
    cin >> n >> m;
    edges.assign(m,Edge());
    for (int i = 0; i < m; i++){
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
    }
        

    // 开始计时
    clock_t start = clock();
    sort(edges.begin(),edges.end());
    Kruskal(edges,n);//实现详见下文
    // 结束计时
    clock_t end = clock();

    cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

    // 把结果追加到文件末尾
    ofstream outFile("result.txt", ios::app);
    if (!outFile) {
        cerr << "Failed to create file: result.txt" << endl;
        return 0;
    }

    // 写入顶点数和边数
    outFile << n << "," << m << "," << (double)(end - start) / CLOCKS_PER_SEC << endl;
    outFile.close();

    // ofstream outFile("result.txt", ios::app);
    // if (!outFile) {
    //     cerr << "Failed to create file: result.txt" << endl;
    //     return 0;
    // }

    // // 写入顶点数和边数
    // outFile << n << "," << m << "," << (double)(end - start) / CLOCKS_PER_SEC << endl;

    // // 关闭文件
    // outFile.close();

    return 0;
}