#include <bits/stdc++.h>
using namespace std;
void print(vector<int> v)
{
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i] << " ";
    }
    cout << endl;
}

vector<int> topSortProcess(vector<vector<int>> &matrix)
{
    int process = matrix.size();
    vector<vector<int>> adj(process, vector<int>()); // directed graph for process priority
    vector<int> ind(process, 0);                     // dependent process
    queue<int> cpu;                                  // queue for maintaing remaining process
    vector<int> processTime;

    for (int i = 0; i < process; i++)
    {
        for (int j = 0; j < process; j++)
        {

            if (i != j)
            {
                if (matrix[i][j] == 1)
                {
                    adj[i].push_back(j);
                    ind[j]++;
                }
            }
        }
    }

    for (int i = 0; i < process; i++)
        if (ind[i] == 0)
            cpu.push(i);

    while (!cpu.empty())
    {
        int currentProcess = cpu.front();
        cpu.pop();
        processTime.push_back(currentProcess);

        for (auto childProcess : adj[currentProcess])
        {
            ind[childProcess]--;
            if (ind[childProcess] == 0)
                cpu.push(childProcess);
        }
    }

    return processTime;
}

vector<vector<int>> convert(vector<vector<int>> a)
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

void bfs(vector<vector<int>> &t, vector<int> &sol)
{
    vector<vector<int>> adj = convert(t);
    int v = t.size();

    queue<int> q;
    vector<bool> visited(v + 1, false);

    q.push(0);
    visited[0] = true;

    while (!q.empty())
    {
        int t = q.front();
        q.pop();
        sol.push_back(t);

        for (auto u : adj[t])
        {
            if (!visited[u])
            {
                visited[u] = true;
                q.push(u);
            }
        }
    }
}

void stackwise(vector<vector<int>> &t, vector<int> &sol)
{
    vector<vector<int>> adj = convert(t);
    int v = t.size();

    stack<int> st1, st2;
    vector<bool> visited(v + 1, false);

    st1.push(0);
    visited[0] = true;

    while (!st1.empty())
    {
        if (st2.empty())
        {
            while (!st1.empty())
            {
                st2.push(st1.top());
                st1.pop();
            }
        }

        while (!st2.empty())
        {
            int t = st2.top();
            st2.pop();
            sol.push_back(t);

            for (auto u : adj[t])
            {
                if (!visited[u])
                {
                    visited[u] = true;
                    st1.push(u);
                }
            }
        }
    }
}

int heapApproach(vector<vector<int>> &t, vector<vector<int>> &time, int psize, vector<pair<int, int>> &processorSequence, vector<int> &processOrder)
{
    vector<vector<int>> adj = convert(t);
    int v = t.size();
    vector<bool> visit(v + 1, false);
    int ans = 0;

    queue<int> q;
    q.push(0);
    visit[0] = true;
    vector<int> processor(psize + 1, 0);
    // vector<int> order;
    processOrder.emplace_back(0);
    int minTime = INT_MAX;
    int proc = -1;
    for (int i = 0; i < psize; i++)
    {
        if (minTime > time[0][i])
        {
            proc = i;
            minTime = time[0][i];
        }
    }

    processorSequence.emplace_back(0, proc);
    processor[proc] += minTime;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        priority_queue<pair<int, pair<int, int>>> level_order;
        vector<int> processor_copy(psize + 1, 0);
        for (int i = 0; i < psize; i++)
            processor_copy[i] = processor[i];

        for (int k : adj[u])
        {
            if (!visit[k])
            {
                priority_queue<pair<int, int>> pq;
                for (int i = 0; i < psize; i++)
                {
                    for (int j = 0; j < psize; j++)
                    {
                        if (i == j)
                            pq.push({processor_copy[i] + time[k][i], i});
                        else
                            pq.push({processor_copy[i] + time[k][j], j});
                    }
                }

                pair<int, int> p = pq.top();
                // processor[p.second] += time[k][p.second];
                level_order.push({p.first, {k, p.second}});
                visit[k] = true;
                // q.push(k);
            }
        }

        while (!level_order.empty())
        {
            pair<int, pair<int, int>> p = level_order.top();
            pair<int, int> info = p.second;
            level_order.pop();
            processOrder.emplace_back(info.first);
            processor[info.second] += time[info.first][info.second];
            processorSequence.emplace_back(info.first, info.second);
            q.push(info.first);
        }
    }

    for (int i : processor)
        ans = max(i, ans);

    cout << endl;

    return ans;
}

void reversebfs(vector<vector<int>> &t, vector<int> &sol)
{
    vector<vector<int>> adj = convert(t);
    int v = t.size();

    queue<int> q;
    vector<bool> visited(v + 1, false);

    q.push(0);
    visited[0] = true;

    while (!q.empty())
    {
        int t = q.front();
        q.pop();
        sol.push_back(t);

        int x = adj[t].size();

        for (int i = x - 1; i >= 0; i--)
        {
            if (!visited[adj[t][i]])
            {
                visited[adj[t][i]] = true;
                q.push(adj[t][i]);
            }
        }
    }
}

int main()
{
    // psize= number of processors

    // tsize= number of tasks

    int psize = 3, tsize = 6; // sample case

    // cin >> psize >> tsize ;   //for manual input

    vector<vector<int>> p(psize, vector<int>(psize, 0));

    vector<vector<int>> sampleP =
        {{0, 1, 1},
         {1, 0, 1},
         {1, 1, 0}};

    p = sampleP;

    /*
     //for taking input of p
     for(int i = 0; i < psize; i++){
         int u , v ;
         cin >> u >> v ;
         p[u][v] = 1 ;
           p[v][u] = 1 ;
     }
    */

    vector<vector<int>> t(tsize, vector<int>(tsize, 0));

    vector<vector<int>> sampleT =
        {
            {0, 1, 1, 0, 0, 0},
            {0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0}};

    t = sampleT;

    /*
     //for taking input of t
     for(int i = 0; i < tsize; i++){
         int u , v ;
         cin >> u >> v ;
         t[u][v] = 1 ;
     }
    */

    // Data will be a 2D Matrix used to store time required to run each task on different processor

    vector<vector<int>> data(tsize, vector<int>(psize, 0));

    vector<vector<int>> sampleData =
        {
            {12, 9, 11},
            {16, 11, 12},
            {19, 13, 16},
            {15, 16, 18},
            {11, 12, 13},
            {7, 8, 12}};

    data = sampleData;

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

    // Topological wise implementation by Aryan Singh(2019UGCS007R)

    vector<int> processTime;
    processTime = topSortProcess(sampleT);
    cout << "A possible path  by Aryan Singh(2019UGCS007R) By using Topological traversal will be :" << endl;

    print(processTime);
    cout << endl;

    // Path will be stored in different vectors for different algorithms

    // BFS traversal Implementation by Saheb Kumar ( 2019UGCS009R)
    vector<int> bfsPath;
    bfs(t, bfsPath);

    cout << "A possible path by Saheb Kumar ( 2019UGCS009R) By using BFS traversal will be :" << endl;
    print(bfsPath);
    cout << endl;

    // Stack wise Implementation by Jeevan Kumar (2019UGCS061R)

    vector<int> stackwisepath;
    stackwise(t, stackwisepath);

    cout << "A possible path by Jeevan Kumar (2019UGCS061R) By using Stackwise traversal is:" << endl;
    print(stackwisepath);
    cout << endl;

    // Heap Based traversal Implementation by Sahil Gupta (2019UGCS003R)

    vector<pair<int, int>> processorSequence;
    vector<int> processOrder;

    int heapTime = heapApproach(sampleT, sampleData, psize, processorSequence, processOrder);
    cout << "Time taken to complete task using heap based approach = " << heapTime << " //implemented by Sahil Gupta (2019UGCS003R)" << endl;
    cout << "Order of execution of process" << endl;
    for (int i : processOrder)
        cout << i << " ";
    cout << endl;
    cout << "Processor mapping for each process is as follows:" << endl;
    for (auto &it : processorSequence)
        cout << "For process = " << it.first << " processor used = " << it.second << endl;

    // Reverse BFS traversal Implementation by Suraj kumar (2019UGCS027R)

    vector<int> revbfsPath;
    reversebfs(t, revbfsPath);

    cout << "A possible path by Suraj kumar (2019UGCS027R) By using Reverse BFS traversal will be :" << endl;
    print(revbfsPath);
    cout << endl;

    return 0;
}