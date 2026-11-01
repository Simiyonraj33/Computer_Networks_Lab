#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_ROUTERS 20
#define INF 9999
typedef struct
{
    int vertex;
    int distance;
} HeapNode;
typedef struct
{
    int size;
    int capacity;
    int position[MAX_ROUTERS];
    HeapNode array[MAX_ROUTERS];
} MinHeap;
void dijkstra(int n,int graph[MAX_ROUTERS][MAX_ROUTERS],int source);
void printIntermediateTable(int n,int dist[],int parent[],int visited[],int current);
void printRoutingTable(int n,int source,int dist[],int parent[]);
void printShortestTree(int n,int source,int parent[]);
void printFinalPaths(int n,int source,int dist[],int parent[]);
void printLSDB(int n,int graph[MAX_ROUTERS][MAX_ROUTERS]);
void printPath(int parent[],int j);
int getNextHop(int parent[],int source,int destination);
void swapHeapNode(HeapNode *a, HeapNode *b)
{
    HeapNode temp=*a;
    *a=*b;
    *b=temp;
}
void minHeapify(MinHeap *heap,int index)
{
    int smallest=index;
    int left=2*index+1;
    int right=2*index+2;
    if(left<heap->size &&
       heap->array[left].distance<
       heap->array[smallest].distance)
        smallest=left;
    if(right<heap->size &&heap->array[right].distance<heap->array[smallest].distance)smallest=right;
    if(smallest!=index)
    {
        HeapNode smallestNode=heap->array[smallest];
        HeapNode indexNode=heap->array[index];
        heap->position[smallestNode.vertex]=index;
        heap->position[indexNode.vertex]=smallest;
        swapHeapNode(&heap->array[smallest],&heap->array[index]);
        minHeapify(heap,smallest);
    }
}
int isEmpty(MinHeap *heap)
{
    return heap->size==0;
}
HeapNode extractMin(MinHeap *heap)
{
    HeapNode root,last;
    if(isEmpty(heap))
    {
        HeapNode temp;
        temp.vertex=-1;
        temp.distance=INF;
        return temp;
    }
    root=heap->array[0];
    last=heap->array[heap->size-1];
    heap->array[0]=last;
    heap->position[last.vertex]=0;
    heap->position[root.vertex]=heap->size-1;
    heap->size--;
    minHeapify(heap,0);
    return root;
}
void decreaseKey(MinHeap *heap,int vertex,int distance)
{
    int i=heap->position[vertex];
    heap->array[i].distance=distance;
    while(i &&heap->array[i].distance<heap->array[(i-1)/2].distance)
    {
        heap->position[heap->array[i].vertex]=(i-1)/2;
        heap->position[heap->array[(i-1)/2].vertex]=i;
        swapHeapNode(&heap->array[i],&heap->array[(i-1)/2]);
        i=(i-1)/2;
    }
}
void printPath(int parent[],int j)
{
    if(parent[j]==-1)
    {
        printf("%d",j+1);
        return;
    }
    printPath(parent,parent[j]);
    printf(" -> %d",j+1);
}
int getNextHop(int parent[],int source,int destination)
{
    int current=destination;
    if(destination==source)
        return -1;
    while(parent[current]!=-1 && parent[current]!=source)
    {
        current=parent[current];
    }
    return current+1;
}
void printLSDB(
        int n,
        int graph[MAX_ROUTERS][MAX_ROUTERS])
{
    int i,j;
    printf("\n");
    printf("=============================================================\n");
    printf("                LINK STATE DATABASE\n");
    printf("=============================================================\n");
    printf("%-12s%-12s%-10s\n","Router","Neighbor","Cost");
    printf("-------------------------------------------------------------\n");
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            if(i!=j &&
               graph[i][j]!=INF)
            {
                printf("%-12d%-12d%-10d\n",i+1,j+1,graph[i][j]);
            }
        }
    }
    printf("=============================================================\n");
}
void printIntermediateTable(int n,int dist[],int parent[],int visited[],int current)
{
    int i;
    printf("\n");
    printf("===============================================================\n");
    printf("Current Router : %d\n", current + 1);
    printf("===============================================================\n");
    printf("Visited Routers : ");
    for(i = 0; i < n; i++)
    {
        if(visited[i])
            printf("%d ", i + 1);
    }
    printf("\n\n");
    printf("%-12s%-12s%-12s%-12s\n","Router","Distance","Parent","Status");
    printf("---------------------------------------------------------------\n");
    for(i = 0; i < n; i++)
    {
        printf("%-12d", i + 1);
        if(dist[i] == INF)
            printf("%-12s", "INF");
        else
            printf("%-12d", dist[i]);
        if(parent[i] == -1)
            printf("%-12s", "-");
        else
            printf("%-12d", parent[i] + 1);
        if(i == current)
            printf("%-12s", "Current");
        else if(visited[i])
            printf("%-12s", "Visited");
        else
            printf("%-12s", "Pending");
        printf("\n");
    }
    printf("===============================================================\n");
}
void dijkstra(int n,int graph[MAX_ROUTERS][MAX_ROUTERS],int source)
{
    int dist[MAX_ROUTERS];
    int parent[MAX_ROUTERS];
    int visited[MAX_ROUTERS];
    MinHeap heap;
    int i;
    int step = 1;
    heap.size = n;
    heap.capacity = n;
    for(i = 0; i < n; i++)
    {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = 0;
        heap.array[i].vertex = i;
        heap.array[i].distance = INF;
        heap.position[i] = i;
    }
    dist[source] = 0;
    heap.array[source].distance = 0;
    decreaseKey(&heap, source, 0);
    printf("\n");
    printf("###############################################################\n");
    printf("                 SOURCE ROUTER : %d\n", source + 1);
    printf("###############################################################\n");
    printLSDB(n, graph);
    while(!isEmpty(&heap))
    {
        HeapNode minNode = extractMin(&heap);
        int u = minNode.vertex;
        if(u == -1)
            break;
        if(visited[u])
            continue;
        visited[u] = 1;
        printf("\n");
        printf("***************************************************************\n");
        printf("STEP %d\n", step++);
        printf("***************************************************************\n");
        printf("\nProcessing Router %d\n\n", u + 1);
        for(i = 0; i < n; i++)
        {
            if(graph[u][i] == INF)
                continue;
            if(visited[i])
                continue;
            if(dist[u] == INF)
                continue;
            printf("Checking Link : %d --> %d\n",u + 1,i + 1);
            printf("Current Distance = ");
            if(dist[i] == INF)
                printf("INF\n");
            else
                printf("%d\n", dist[i]);
            printf("New Cost = %d + %d = %d\n",dist[u],graph[u][i],dist[u] + graph[u][i]);
            if(dist[u] + graph[u][i] < dist[i])
            {
                printf("Status : UPDATED\n");
                dist[i] = dist[u] + graph[u][i];
                parent[i] = u;
                decreaseKey(&heap,i,dist[i]);
            }
            else
            {
                printf("Status : NOT UPDATED\n");
            }
            printf("\n");
        }
        printIntermediateTable(n,dist,parent,visited,u);
    }
    printf("\n");
    printf("===============================================================\n");
    printf("DIJKSTRA COMPLETED FOR SOURCE ROUTER %d\n",source + 1);
    printf("===============================================================\n");
    printRoutingTable(n,source,dist,parent);
    printShortestTree(n,source,parent);
    printFinalPaths(n,source,dist,parent);
}
void printRoutingTable(int n,int source,int dist[],int parent[])
{
    int i;
    int nextHop;
    printf("\n");
    printf("===============================================================\n");
    printf("                     ROUTING TABLE\n");
    printf("                 SOURCE ROUTER : %d\n", source + 1);
    printf("===============================================================\n");
    printf("%-15s%-12s%-12s%-25s\n","Destination","Cost","Next Hop","Path");
    printf("--------------------------------------------------------------------------\n");
    for(i = 0; i < n; i++)
    {
        printf("%-15d", i + 1);
        if(dist[i] == INF)
        {
            printf("%-12s%-12s%s\n","INF","-","No Path");
            continue;
        }
        printf("%-12d", dist[i]);
        nextHop = getNextHop(parent, source, i);
        if(nextHop == -1)
            printf("%-12s", "-");
        else
            printf("%-12d", nextHop);
        printPath(parent, i);
        printf("\n");
    }
    printf("===============================================================\n");
}
void printShortestTree(int n,int source,int parent[])
{
    int i;
    printf("\n");
    printf("===============================================================\n");
    printf("                  SHORTEST PATH TREE\n");
    printf("===============================================================\n");
    printf("Source Router : %d\n\n", source + 1);
    printf("%d\n", source + 1);
    for(i = 0; i < n; i++)
    {
        if(parent[i] != -1)
        {
            printf("|-- %d (Parent : %d)\n",i + 1,parent[i] + 1);
        }
    }
    printf("===============================================================\n");
}
void printFinalPaths(int n,int source,int dist[],int parent[])
{
    int i;
    int nextHop;
    printf("\n");
    printf("==========================================================================\n");
    printf("                 FINAL SHORTEST PATHS\n");
    printf("==========================================================================\n");
    printf("%-15s%-12s%-12s%-30s\n","Destination","Cost","Next Hop","Shortest Path");
    printf("--------------------------------------------------------------------------\n");
    for(i = 0; i < n; i++)
    {
        printf("%-15d", i + 1);
        if(dist[i] == INF)
        {
            printf("%-12s%-12s%s\n","INF","-","UNREACHABLE");
            continue;
        }
        printf("%-12d", dist[i]);
        nextHop = getNextHop(parent,source,i);
        if(nextHop == -1)
            printf("%-12s", "-");
        else
            printf("%-12d", nextHop);
        printPath(parent, i);
        printf("\n");
    }
    printf("==========================================================================\n");
}
int main()
{
    int graph[MAX_ROUTERS][MAX_ROUTERS];
    int n;
    int i,j;
    int source;
    printf("==========================================\n");
    printf("        LINK STATE ROUTING\n");
    printf("==========================================\n");
    while(1)
    {
        printf("Enter the number of routers (2-%d): ",MAX_ROUTERS);
        scanf("%d",&n);
        if(n>=2 && n<=MAX_ROUTERS)
            break;
        printf("\nInvalid number of routers.\n\n");
    }
    printf("\n");
    printf("Enter the cost matrix\n");
    printf("(Use 9999 for Infinity / No Link)\n\n");
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            while(1)
            {
                printf("Cost from Router %d to Router %d : ",i+1, j+1);
                scanf("%d",&graph[i][j]);
                if(i==j)
                {
                    graph[i][j]=0;
                    break;
                }
                if(graph[i][j]<0)
                {
                    printf("\n");
                    printf("Negative edge weights are NOT allowed.\n");
                    printf("Please enter again.\n\n");
                    continue;
                }
                break;
            }
        }
    }
    while(1)
    {
        printf("\nEnter Source Router (1-%d): ",n);
        scanf("%d",&source);
        if(source>=1 && source<=n)
            break;
        printf("Invalid Source Router.\n");
    }
    source--;
    printf("\n");
    printf("###############################################################\n");
    printf("        EXECUTING SOURCE ROUTER : %d\n", source + 1);
    printf("###############################################################\n");
    dijkstra( n,graph,source);
    for(i = 0; i < n; i++)
    {
        if(i == source)
            continue;
        printf("\n\n");
        printf("###############################################################\n");
        printf("        EXECUTING SOURCE ROUTER : %d\n", i + 1);
        printf("###############################################################\n");
        dijkstra(n, graph, i);
    }
    printf("\n");
    printf("===============================================================\n");
    printf("        LINK STATE ROUTING COMPLETED SUCCESSFULLY\n");
    printf("===============================================================\n");
    printf("\nAll routers have been processed successfully.\n");
    return 0;
}