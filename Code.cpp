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

void heapApproach(vector<vector<int>> &t, vector<vector<int>> &time, int psize, vector<int> &processOrder, vector<int> &parent)
{
    // cout << "Hello" << endl;
    vector<vector<int>> adj = convert(t);
    int v = t.size();

    queue<int> q;
    vector<bool> visited(v + 1, false);
    vector<int> ll(v + 1, 0);
    vector<vector<int>> level(v + 1, vector<int>());

    q.push(0);
    ll[0] = 0;
    visited[0] = true;
    level[0].emplace_back(0);

    while (!q.empty())
    {
        int t = q.front();
        q.pop();

        for (auto u : adj[t])
        {
            if (!visited[u])
            {
                visited[u] = true;
                ll[u] = ll[t] + 1;
                level[ll[t] + 1].emplace_back(u);
                q.push(u);
            }
        }
    }

    vector<int> processorTime(v + 1, 0);
    vector<int> processMap(v + 1, 0);

    for (vector<int> &lev : level)
    {
        priority_queue<pair<int, int>> pq;
        for (int pro : lev)
        {
            int prev = 0;
            if (parent[pro] != -1)
                prev = processorTime[processMap[parent[pro]]];
            int minTime = INT_MAX, processor = -1;
            for (int j = 0; j < psize; j++)
            {
                int totalTime = processorTime[j] + time[pro][j] + prev;
                if (minTime > totalTime)
                {
                    minTime = totalTime;
                    processor = j;
                }
            }

            processorTime[processor] = minTime;
            processMap[pro] = processor;
            pq.push({minTime, pro});
        }

        while (!pq.empty())
        {
            pair<int, int> pp = pq.top();
            pq.pop();
            processOrder.emplace_back(pp.second);
        }
    }
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

int timeCalc(vector<int> &series, vector<int> &parent, vector<vector<int>> &time, int psize)
{
    int total = 0;
    vector<int> processor(psize + 1, 0);

    vector<int> processMap(series.size(), -1);
    vector<pair<int, int>> start_end;

    for (int i : series)
    {
        int prev = 0;
        if (parent[i] != -1)
            prev = processor[processMap[parent[i]]];
        // cout << "Parent of " << i << " = " << parent[i] << " Prev time = " << prev << endl;

        int minTime = INT_MAX, pro = -1;
        for (int j = 0; j < psize; j++)
        {
            int totalTime = max(prev, processor[j]) + time[i][j];
            // cout << "For Process = " << i << " totalTime = " << totalTime << " using processor = " << j << " prev val = " << prev << endl;
            if (minTime > totalTime)
            {
                minTime = totalTime;
                pro = j;
            }
        }
        processMap[i] = pro;
        start_end.emplace_back(prev, minTime);

        processor[pro] = minTime;
    }

    for (int i : processor)
    {
        total = max(i, total);
    }

    for (int i = 0; i < series.size(); i++)
    {
        int xx = processMap[series[i]];
        pair<int, int> yy = start_end[i];

        cout << "For Process = " << series[i] << " processor used = " << xx << " start time = " << yy.first << " end time = " << yy.second << endl;
    }

    return total;
}

vector<int> getParent(vector<vector<int>> &t)
{
    vector<vector<int>> adj = convert(t);
    int v = t.size();
    vector<int> parent(v + 1, -1);

    queue<int> q;
    vector<bool> visited(v + 1, false);

    q.push(0);
    visited[0] = true;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        // sol.push_back(t);

        for (auto v : adj[u])
        {
            if (!visited[v])
            {
                parent[v] = u;
                visited[v] = true;
                q.push(v);
            }
        }
    }

    return parent;
}

int main()
{
    // psize= number of processors

    // tsize= number of tasks

    int psize = 10, tsize = 10; // sample case

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

    // vector<vector<int>> sampleT =
    //     {
    //         {0, 1, 1, 0, 0, 0},
    //         {0, 0, 0, 1, 0, 0},
    //         {0, 0, 0, 0, 1, 0},
    //         {0, 0, 0, 0, 0, 1},
    //         {0, 0, 0, 0, 0, 1},
    //         {0, 0, 0, 0, 0, 0}};

    vector<vector<int>> sampleT = {
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

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

    // vector<vector<int>> sampleData =
    //     {
    // {12, 9, 11},
    // {16, 11, 12},
    // {19, 13, 16},
    // {15, 16, 18},
    // {11, 12, 13},
    // {7, 8, 12}};

    vector<vector<int>> sampleData = {
        {12, 9, 11, 16, 11, 12, 11, 12, 13, 10},
        {16, 11, 12, 19, 13, 16, 15, 16, 18, 12},
        {19, 13, 16, 15, 16, 18, 11, 12, 13, 12},
        {15, 16, 18, 11, 12, 13, 11, 12, 13, 10},
        {11, 12, 13, 7, 8, 12, 18, 11, 12, 13},
        {7, 8, 12, 11, 12, 13, 11, 12, 13, 10},
        {12, 9, 11, 16, 11, 12, 11, 12, 13, 10},
        {16, 11, 12, 19, 13, 16, 15, 16, 18, 12},
        {19, 13, 16, 15, 16, 18, 11, 12, 13, 12},
        {15, 16, 18, 11, 12, 13, 11, 12, 13, 10},
        {11, 12, 13, 7, 8, 12, 18, 11, 12, 13},
        {7, 8, 12, 11, 12, 13, 11, 12, 13, 10}};

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

    vector<int> parent = getParent(sampleT);
    int x;

    // Topological wise implementation by Aryan Singh(2019UGCS007R)

    vector<int> processTime;
    processTime = topSortProcess(sampleT);
    cout << "A possible path  by Aryan Singh(2019UGCS007R) By using Topological traversal will be :" << endl;

    print(processTime);
    cout << endl;

    x = timeCalc(processTime, parent, sampleData, psize);
    cout << "Total time for Topological based approach = " << x << endl;

    // Path will be stored in different vectors for different algorithms

    // BFS traversal Implementation by Saheb Kumar ( 2019UGCS009R)
    vector<int> bfsPath;
    bfs(t, bfsPath);

    cout << "A possible path by Saheb Kumar ( 2019UGCS009R) By using BFS traversal will be :" << endl;
    print(bfsPath);
    cout << endl;

    x = timeCalc(bfsPath, parent, sampleData, psize);
    cout << "Total time for bfs based approach = " << x << endl;

    // Stack wise Implementation by Jeevan Kumar (2019UGCS061R)

    vector<int> stackwisepath;
    stackwise(t, stackwisepath);

    cout << "A possible path by Jeevan Kumar (2019UGCS061R) By using Stackwise traversal is:" << endl;
    print(stackwisepath);
    cout << endl;

    x = timeCalc(stackwisepath, parent, sampleData, psize);
    cout << "Total time for Stack based approach = " << x << endl;

    // Reverse BFS traversal Implementation by Suraj kumar (2019UGCS027R)

    vector<int>
        revbfsPath;
    reversebfs(t, revbfsPath);

    cout << "A possible path by Suraj kumar (2019UGCS027R) By using Reverse BFS traversal will be :" << endl;
    print(revbfsPath);
    cout << endl;

    x = timeCalc(revbfsPath, parent, sampleData, psize);
    cout << "Total time for reverse bfs approach = " << x << endl;

    // Heap Based traversal Implementation by Sahil Gupta (2019UGCS003R)

    vector<int> processOrder;

    heapApproach(sampleT, sampleData, psize, processOrder, parent);
    cout << "Possible path for heap based approach implemented by Sahil Gupta (2019UGCS003R)" << endl;
    for (int i : processOrder)
        cout << i << " ";
    cout << endl;
    // cout << "Processor mapping for each process is as follows:" << endl;
    // for (auto &it : processorSequence)
    //     cout << "For process = " << it.first << " processor used = " << it.second << endl;

    x = timeCalc(processOrder, parent, sampleData, psize);
    cout << "Total time for heap based approach = " << x << endl;

    return 0;
}