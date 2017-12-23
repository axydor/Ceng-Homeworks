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
        void dijkstra(int S,vector<Node>& p,set<int>& dests);
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

void Graph::dijkstra(int S,vector<Node>& p,set<int>& dests){
    vector<long> distances(this->vNo,INF);
    priority_queue<Node,vector<Node> , decltype(&Compare)>  Q(&Compare);
    distances[S] = 0;
    Q.push(Node(S,0)); 
    while (!Q.empty()){
        int v = Q.top().dest;  // vertex
        int w = Q.top().weight; // weigh
        Q.pop();
        if ( w > distances[v])
            continue;
        for(list<Node>::iterator it = this->v[v].begin(); it != this->v[v].end(); it++){
            int road = distances[v] + it->weight ;
            if( distances[it->dest] > road ){
                distances[it->dest] = road;
                Q.push(Node(it->dest,road));
            }
        }
    }
    //cout << "VERTEX DISTANCE FROM SOURCE " << endl;
    for (int i=0; i < vNo; i++){
        if(dests.count(i))
            p.push_back(Node(i,distances[i]));
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
    vector<Node> path_s1;  // HOLDING THE DISTANCES FROM SOURCE 1 TO DESTINATIONS
    vector<Node> path_s2;  // HOLDING THE DISTANCES FROM SOURCE 2 TO DESTINATIONS
    g.dijkstra(s1,path_s1,dests);
    g.dijkstra(s2,path_s2,dests);
    vector<Node> diff;
    vector<Node> rambo;
    int counter = 0, len=0;
    /*
    for(auto i : road1){
        if( dests.count(i.dest) == 1)
            path_s1.push_back(Node(i.dest,i.weight));
     }
    for(auto i : road2){           
        if( dests.count(i.dest) == 1)
            path_s2.push_back(Node(i.dest,i.weight));
    } 
    */
    sort(path_s1.begin(),path_s1.end(),accordingto);// SORTING ELEMENTS IN ASCENDING ORDER WRT DISTANCE vertex
    sort(path_s2.begin(),path_s2.end(),accordingto);  
    vector<Node>::iterator it1 = path_s1.begin();
    vector<Node>::iterator it2 = path_s2.begin();
    while( it1 < path_s1.end() ){     
        diff.push_back(Node(it1->dest,(it1->weight) - (it2->weight) ) );
        it1++;
        it2++;
    }
    sort(diff.begin(),diff.end(),Compare);   // SORT WITH RESPECT TO DIFFERENCE
    vector<Node>::iterator dit = diff.begin();
    while( counter < (destNo /2) ){          // PUT THE FIRST HALF INTO S1
        rambo.push_back(Node(dit->dest,s1));
        counter++;
        dit++;
    }
    while( counter < destNo){
        rambo.push_back(Node(dit->dest,s2));
        counter++;
        dit++;
    }
    sort(rambo.begin(),rambo.end(),accordingto);
    it1 = path_s1.begin();
    it2 = path_s2.begin();
    vector<Node>::iterator ram = rambo.begin();
    while(it1 < path_s1.end()){
        if( ram -> weight == s1 )
            len += it1->weight;
        else
            len += it2->weight;
        it1++;
        it2++;
        ram++;
    }
    cout << len << endl;
    for(auto i : rambo){
        cout << i.dest << " " << i.weight << endl;
    }
    argc=argc;
    return 0;
}   

