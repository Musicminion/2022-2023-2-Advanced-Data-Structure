#include<iostream>
#include<vector>
#include<thread>
#include<map>
#include<mutex>
#include<condition_variable>
#include<fstream>
#include <unistd.h>
using namespace std;

class edge{
    public:
    edge() {}
    edge(int a,int b,int c)
      :v1(a),v2(b),w(c),next(NULL) {}
    int v1,v2;
    int w,tid;
    edge* next;
};
class vertex {
    public:
    vertex(int v)
        :vid(v),next(NULL) {}
    int vid;
    edge* next;
};

const int thread_num = 3;//线程数量
vector<vertex> graph;//图邻接表
vector<edge> mst;//结果集
vector<thread> subthreads;//线程向量
multimap<int,edge> edge_queue;//全局队列
mutex mut;//互斥锁
mutex mut2, mut3;
condition_variable cond_v[thread_num];//条件变量
bool isfinished = false;
int n,m;

void output() {
    for (int i = 0; i < mst.size(); i++)
     cout << mst[i].v1 << " -> " << mst[i].v2 << endl;
    // cout << mst.size();
}

void construct(){
    // std::ifstream inFile(path);
    // inFile.open("7_20.txt");

    int v_num,a_num;
    cin >> v_num >> a_num;
    n = v_num;
    m = a_num;
    // inFile >> v_num >> a_num;
    // cout << v_num << a_num << endl;
    for (int i = 0; i < v_num; i++)
        graph.push_back(vertex(i));
    for (int i = 0; i < a_num; i++){
        edge *temp1 = new edge(), *temp2;
        cin >> temp1->v1 >> temp1->v2 >> temp1->w;
        // inFile >> temp1->v1 >> temp1->v2 >> temp1->w;
        // cout << temp1->v1 << temp1->v2 << temp1->w;
        temp2 = new edge(*temp1);
        temp1->next = graph[temp1->v1 ].next;
        graph[temp1->v1 ].next = temp1;
        temp2->next = graph[temp2->v2 ].next;
        graph[temp2->v2 ].next = temp2;
    }
}

void send_edge(multimap<int,edge>& m, int i) {
    if (!m.empty()) {
        edge_queue.insert(*(m.begin()));
        // cout << "thread "<<i<<" insert "<<m.begin()->second.v1 <<" "<<m.begin()->second.v2<<endl; 
        // cout << edge_queue.size()<<endl;
        m.erase(m.begin());
    }
}
//线程函数，执行“部分算法”
void subthread_func(vector<vertex> v,int tid){
    // cout << "test"<<endl;
    multimap<int,edge> e;//用于排序
    // cout << v.size() << endl;
    for (int i = 0; i < v.size(); i++){
        edge* temp = v[i].next;
        while (temp != NULL){
            temp->tid = tid;
            e.insert(pair<int,edge>(temp->w,*temp));
            temp = temp->next;
        }
    }
    //  cout <<tid<< " insert ok "<<e.size()<<endl;
    unique_lock<mutex> lk(mut);

    send_edge(e,tid);
    while (true){
        cond_v[tid].wait(lk);
        // cout << "wait" << endl;
        if (isfinished){
            return;
        }
        send_edge(e,tid);
    }
}

void partition() {
    vector<vertex> temp[thread_num];
    for (int i = 0; i < graph.size(); i++)
        temp[i % thread_num].push_back(graph[i]);
    for (int i = 0; i < thread_num; i++)
        subthreads.push_back(thread(subthread_func,temp[i],i));
}


void add_edge(edge e,map<int,int>& i,map<int,vector<int>>& rev_i){
    mst.push_back(e);
    // cout << "push " << e.v1 <<" "<<e.v2<<endl;
    int cid1 = i[e.v1],cid2 = i[e.v2];
    if (cid1 == -1 && cid2 == -1){
        int cid = rev_i.size() + 1;
        vector<int> temp;
        temp.push_back(e.v1);
        temp.push_back(e.v2);
        rev_i[cid] = temp;
        i[e.v1] = cid;
        i[e.v2] = cid;
    }
    else if (cid1 == -1){
        i[e.v1] = cid2;
        rev_i[cid2].push_back(e.v1);
    }
    else if (cid2 == -1){
        i[e.v2] = cid1;
        rev_i[cid1].push_back(e.v2);
    }
    else if (rev_i[cid1].size() <= rev_i[cid2].size()){
        for (int k = 0; k < rev_i[cid1].size(); k++){
            i[rev_i[cid1][k]] = cid2;
            rev_i[cid2].push_back(rev_i[cid1][k]);
        }
        rev_i.erase(cid1);
    }
    else{
        for (int k = 0; k < rev_i[cid2].size(); k++){
            i[rev_i[cid2][k]] = cid1;
            rev_i[cid1].push_back(rev_i[cid2][k]);
        }
        rev_i.erase(cid2);
    }
}

void kruskal(){
    // cout << "kruskal "<< endl;
    sleep(1);
    map<int,int> index;
    map<int,vector<int>> rev_index;
    for (int i = 0; i < graph.size(); i++){
        index[i] = -1;
    }
    while (mst.size() < graph.size() - 1){
        unique_lock<mutex> lk(mut);
        // cout << "loop"<<endl;
        if (edge_queue.empty())
            // break;
        {
            // cout << "is empty" << endl;
        
            lk.unlock();
            for (int i = 0; i < thread_num; i++){
                cond_v[i].notify_all();
            }
            continue;
        }
        pair<int,edge> temp = *(edge_queue.begin());
        edge_queue.erase(edge_queue.begin());
        lk.unlock();
        cond_v[temp.second.tid].notify_all();

        if (index[temp.second.v1] != index[temp.second.v2] ||
        index[temp.second.v1] == -1)
            add_edge(temp.second,index,rev_index);
    }
    isfinished = true;
    for (int i = 0; i < thread_num; i++){
        cond_v[i].notify_all();
        subthreads[i].join();
    }
}

int main(int argc, char* argv[])
{
    construct();//构造图的邻接表

     // 开始计时
    clock_t start = clock();
    partition();//图的划分，创建并发进程，执行“部分算法”
    kruskal();//全局进程执行“仲裁算法”
    // output();//结果输出

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

    return 0;
}
