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

vector<vector<int>> convert( vector<vector<int>> a)
{
    // converts from adjacency matrix to adjacency list
    vector<vector<int>> adjList(a.size());
    for (int i = 0; i < a.size(); i++)
    {

        for (int j = 0; j < a[i].size(); j++)
        {
            if (a[i][j] == 1)
            {
                adjList[i].push_back(j);
            }
        }
    }
    return adjList;
}


void stackwise(vector<vector<int>> &t ,vector<int> &sol)
{
      vector<vector<int>> adj = convert(t);
      int v=t.size();

      stack<int>st1,st2;
        vector<bool>visited(v+1,false);

        st1.push(0);
        visited[0]=true;

        while(!st1.empty())
        {   
            if(st2.empty()){
                while(!st1.empty()){
                    st2.push(st1.top());
                    st1.pop();
                }
            }
        
            while(!st2.empty()){
                    int t=st2.top();
            st2.pop();
            sol.push_back(t);

            for(auto u:adj[t])
            {
                if(!visited[u])
                {
                    visited[u]=true;
                    st1.push(u);
                }
            } 
            }
            
            
        } 
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


    t=sampleT;

   /*
    //for taking input of t
    for(int i = 0; i < tsize; i++){
        int u , v ;
        cin >> u >> v ;
        t[u][v] = 1 ;
    }

   */



    //Data will be a 2D Matrix used to store time required to run each task on different processor

    vector<vector<int>> data(tsize,vector<int>(psize,0)) ;

    vector<vector<int>> sampleData=
    {
    {11,12,13},
    {15,16,18},
    {9,11,12},
    {16,11,12},
    {19,13,16},
    {7,8,12}
    };

    data=sampleData;

  /*
    //for taking input of data
    for(int i = 0; i < tsize; i++){
            for(int j=0;j<psize;j++)
            {
                int u  ;
                cin >> u   ;
                data[i][j] = u ;
            }
    }

   */

   // Path will be stored in different vectors for different algorithms

   // BFS traversal Implementation by Saheb Kumar ( 2019UGCS009R)
    
   // Stack wise Implementation by Jeevan Kumar (2019UGCS061R)

   vector<int> bfsPath;

   stackwise(t,bfsPath);

   cout<<"A possible path By using BFS traversal will be :"<<endl;
   print(bfsPath);


   // Heap Based traversal Implementation by  ---------

   // Topological sorting Based traversal Implementation by  ---------

   // DFS Based traversal Implementation by  ---------

   // Multi Level Queue (MLQ) Based traversal Implementation by  ---------

   // Level Wise Stack (LWS) Based traversal Implementation by  ---------



   return 0;
}
