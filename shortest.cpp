#include <iostream>

using namespace std;

#include "mex.h"
#include "fibheap.h"

#define INF 0x7fffffff
#define INITIAL  0
#define ACTIVE   1
#define EXPANDED 2
#define I3(x, y, z) ((x)*w+(y)*8+(z))
#define I2(x, y) ((x)*w+(y))

int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

class Pixel {

public:

    int x;
    int y;
    double cost;
    int status;

    Pixel() {
        x = y = 0;
        cost = INF;
        status = INITIAL;
    };

    bool operator <(const Pixel& p) {
        return this->cost < p.cost;
    };

    bool operator ==(const Pixel& p) {
        return this->cost == p.cost;
    };
};

class HeapNode : public FibHeapNode {

public:

    Pixel key;

    HeapNode() : FibHeapNode() {};   

    virtual void operator =(FibHeapNode& RHS);
    virtual int  operator ==(FibHeapNode& RHS);
    virtual int  operator <(FibHeapNode& RHS);

    virtual void operator =(Pixel NewKeyVal);
    virtual void Print();
    Pixel GetKeyValue() { return key; };
    void SetKeyValue(Pixel inkey) { key = inkey; };

};

void HeapNode::Print()
{
    FibHeapNode::Print();
    // cout << key.cost;
    mexPrintf("%d,%d,%f", key.x, key.y, key.cost);
}

void HeapNode::operator =(Pixel newKeyVal)
{
    HeapNode temp;

    temp.key = key = newKeyVal;
    FHN_Assign(temp);
}

void HeapNode::operator =(FibHeapNode& RHS)
{
    FHN_Assign(RHS);
    key = ((HeapNode&) RHS).key;
}

int  HeapNode::operator ==(FibHeapNode& RHS)
{
    if (FHN_Cmp(RHS)) return 0;
    return key == ((HeapNode&) RHS).key ? 1 : 0;
}

int  HeapNode::operator <(FibHeapNode& RHS)
{
    int x;

    if ((x=FHN_Cmp(RHS)) != 0)
        return x < 0 ? 1 : 0;

    return key < ((HeapNode&) RHS).key ? 1 : 0;
}

void shortest(double adj[], int h, int w, int seed_x, int seed_y, int pred[]) {
    HeapNode *node = new HeapNode[h*w];
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            node[I2(i,j)].key.x = i;
            node[I2(i,j)].key.y = j;
        }
    node[I2(seed_x,seed_y)].key.cost = 0;
    node[I2(seed_x,seed_y)].key.status = ACTIVE;

    FibHeap heap;
    heap.Insert(&node[I2(seed_x,seed_y)]);

    while (heap.GetNumNodes() > 0) {
        HeapNode *p_curr = ((HeapNode*) heap.ExtractMin());

        for (int i = 0; i < 8; i++) {
            int new_x = p_curr->key.x + dx[i];
            int new_y = p_curr->key.y + dy[i];
            if (new_x < 0 || new_x >= h || new_y < 0 || new_y >= w || node[I2(new_x,new_y)].key.status == EXPANDED)
                continue;
            if (node[I2(new_x,new_y)].key.status == ACTIVE 
                && node[I2(p_curr->key.x,p_curr->key.y)].key.cost + adj[I3(p_curr->key.x,p_curr->key.y,i)] < node[I2(new_x,new_y)].key.cost) {
                HeapNode tmp = node[I2(new_x,new_y)];
                tmp.key.cost = node[I2(p_curr->key.x,p_curr->key.y)].key.cost + adj[I3(p_curr->key.x, p_curr->key.y, i)];
                heap.DecreaseKey(&node[I2(new_x,new_y)], tmp);
                pred[I2(new_x,new_y)] = (i+4)%8;
                node[I2(new_x,new_y)].key.status = EXPANDED;
            }
            if (node[I2(new_x,new_y)].key.status == INITIAL) {
                node[I2(new_x,new_y)].key.cost = node[I2(p_curr->key.x,p_curr->key.y)].key.cost + adj[I3(p_curr->key.x,p_curr->key.y,i)];
                heap.Insert(&node[I2(new_x,new_y)]);
                pred[I2(new_x,new_y)] = (i+4)%8;
                node[I2(new_x,new_y)].key.status = ACTIVE;
            }
        }
    }
    delete[] node;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    int h = (int) mxGetScalar(prhs[0]);
    int w = (int) mxGetScalar(prhs[1]);
    int seed_x = (int) mxGetScalar(prhs[2]);
    int seed_y = (int) mxGetScalar(prhs[3]);
    double *adj = (mxGetPr(prhs[4]));
    // mexPrintf("%f %f %f %f\n", adj[0], adj[2], adj[4], adj[6]);
    mwSize sz[2];
    sz[1] = h;
    sz[0] = w;
    plhs[0] = mxCreateNumericArray(2, sz, mxINT32_CLASS, mxREAL);
    int *out = (int*) mxGetData(plhs[0]);
    shortest(adj, h, w, seed_x, seed_y, out);
    // mexPrintf("%d %d %d %d %d", out[0], out[1], out[2], out[3], out[4]);
}