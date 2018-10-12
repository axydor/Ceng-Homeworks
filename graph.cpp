#include <iostream>
#include <limits>
#define INF std::numeric_limits<int>::max()
#include <vector>
#include <list>
#include <set>
#include <iterator>
#include <queue>
#include <fstream>
#include <algorithm>
#include <utility>
using namespace std;

class Node{
    public:
        int dest;   // DESTINATION VERTEX
        int weight; // LENGTH OR DISTANCE OR WEIGHT
        Node(int dest,int weight);
    };

Node::Node(int dest,int weight){
    this->dest   = dest;
    this->weight = weight;
}

bool accordingto(Node const& l, Node const& r){
    return l.dest < r.dest;
}

bool Compare(Node const& l, Node const& r){
    return l.weight < r.weight;
}

class Graph{
    public:
        int vNo;
        list<Node> *v;
        Graph(int vNo);
        ~Graph();
        void addEdge(int u, int v, int w);
        void dijkstra(int S,vector<int>& distances);
};

Graph::Graph(int vNo){
    this-> vNo = vNo; 
    this-> v = new list<Node> [vNo];
}

Graph::~Graph(){
    delete[] v;   
}

void Graph::addEdge(int u, int v, int w){
    this->v[u].push_back(Node(v,w));
}

void Graph::dijkstra(int S,vector<int>& distances){
    int road;
    distances.resize(this->vNo,INF);
    priority_queue<pair<int,int>> Q;
    distances[S] = 0;
    Q.push(make_pair(S,0)); 
    while (!Q.empty()){
        int v = Q.top().first;  // vertex
        int w = Q.top().second; // weigh
        Q.pop();
        if ( w > distances[v])
            continue;
        for(list<Node>::iterator it = this->v[v].begin(); it != this->v[v].end(); it++){
            road = distances[v] + it->weight ;
            if( distances[it->dest] > road ){
                distances[it->dest] = road;
                Q.push(make_pair(it->dest,road));
            }
        }
    }
}

int main(int argc,char* argv[]){
    ifstream file(argv[1]);
    int vertexNo;
    file >> vertexNo;
    set<int> dests;
    int destNo;
    file >> destNo;
    int i,s1,s2;
    file >> s1;
    file >> s2;
    for (int k = 0; k < destNo; k++){
        file >> i;
        dests.insert(i);
    }
    Graph g(vertexNo);
    int x;
    for(int i = 0; i < vertexNo; i++){
        for(int j = 0; j < vertexNo; j++){
              file >> x ;
              if (x == 0)
                  continue;
              g.addEdge(i,j,x);
        }
    }
    vector<int> path_s1;  // HOLDING THE DISTANCES FROM SOURCE 1 TO DESTINATIONS
    vector<int> path_s2;  // HOLDING THE DISTANCES FROM SOURCE 2 TO DESTINATIONS
    g.dijkstra(s1,path_s1);
    g.dijkstra(s2,path_s2);
    vector<int>::iterator it1=path_s1.begin();
    vector<int>::iterator it2=path_s2.begin();
    vector<Node> diff;  // HOLDING THE DIFFERENCE FOR EACH DESTINATION :  WAREHOUSE1 - WAREHOUSE2
    vector<Node> rambo;
    int len=0;
    for( int i =0; i <vertexNo; i++ ){
        if (dests.count(i))     
            diff.push_back(Node(i,(path_s1[i] - path_s2[i] ) ) );
    }
    sort(diff.begin(),diff.end(),Compare);   // SORT WITH RESPECT TO DIFFERENCE
    vector<Node>::iterator dit = diff.begin();
    while( dit < diff.begin()+ destNo/2 ){          // PUT THE FIRST HALF INTO S1
        rambo.push_back(Node(dit->dest,s1));
        dit++;
    }
    while( dit < diff.end()){
        rambo.push_back(Node(dit->dest,s2));
        dit++;
    }
    sort(rambo.begin(),rambo.end(),accordingto);
    it1 = path_s1.begin();
    it2 = path_s2.begin();
    vector<Node>::iterator ram = rambo.begin();
    for(int i=0; i < vertexNo; i++){
        if( dests.count(i)  ){
            if( ram -> weight == s1 )
                len += it1[i];
            else
                len += it2[i];
            ram++;
        }
    }
    cout << len << endl;
    for(auto i : rambo){
        cout << i.dest << " " << i.weight << endl;
    }
    argc=argc;
    return 0;
}   

