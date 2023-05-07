#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector<int> buildNext(string p){
    int m = p.size(), j = 0;
    vector<int> N(m, 0);
    int t = N[0] = -1;
    while(j < m - 1){
        if(0 > t || p[j] == p[t]){
            j++; t++;
            
            N[j] = t;
        }else{
            t = N[t];
        }
    }
    return N;
}


int main(){
    string p = "ABCDAABCDABCG";
    vector<int> N = buildNext(p);
    for (int i = 0; i < p.size(); i++){
        cout << p[i] << " ";
    }
    cout << endl;
    for(int i = 0; i < N.size(); i++){
        cout << N[i] << " ";
    }
    cout << endl;
}