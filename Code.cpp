#include<bits/stdc++.h>
using namespace std;
void print(vector<int> v)
{
    for(int i=0;i<v.size();i++)
    {
        cout<<v[i]<<" ";
    }
    cout<<endl;
}


vector<int> topSortProcess(vector<vector<int>>& matrix){
    int process=matrix.size();
    vector<vector<int>> adj(process, vector<int>());  //directed graph for process priority
    vector<int> ind(process,0);// dependent process
    queue<int> cpu; //queue for maintaing remaining process
    vector<int> processTime;

    for(int i=0;i<process;i++){
        for(int j=0;j<process;j++){

            if(i!=j){
                if(matrix[i][j]==1){
                    adj[i].push_back(j);
                    ind[j]++;
                }
            }
        }
    }


    for(int i=0;i<process;i++)if(ind[i]==0)cpu.push(i);

    while(!cpu.empty()){
        int currentProcess=cpu.front();
        cpu.pop();
        processTime.push_back(currentProcess);

        for(auto childProcess:adj[currentProcess]){
            ind[childProcess]--;
            if(ind[childProcess]==0)cpu.push(childProcess);
        }
    }

    return processTime;

}


int main()
{
    //psize= number of processors

    // tsize= number of tasks

    int psize=3, tsize=6; //sample case

    //cin >> psize >> tsize ;   //for manual input

    vector<vector<int>> p(psize,vector<int>(psize,0)) ;

    vector<vector<int>> sampleP=
    {{0,1,1},
    {1,0,1},
    {1,1,0}};

    p=sampleP;

   /*
    //for taking input of p
    for(int i = 0; i < psize; i++){
        int u , v ;
        cin >> u >> v ;
        p[u][v] = 1 ;
          p[v][u] = 1 ;
    }

   */


    vector<vector<int>> t(tsize,vector<int>(tsize,0)) ;

    vector<vector<int>> sampleT=
    {
    {0,1,1,0,0,0},
    {0,0,0,1,0,0},
    {0,0,0,0,1,0},
    {0,0,0,0,0,1},
    {0,0,0,0,0,1},
    {0,0,0,0,0,0}
    };


    vector<int> processTime = topSortProcess(sampleT);

    print(processTime);





   return 0;
}
