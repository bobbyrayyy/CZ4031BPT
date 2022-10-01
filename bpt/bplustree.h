#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "types.h"
#include "../storage/disk_storage.h"

#include <cstddef>
#include <array>

struct keys_struct
{
    int key_value;
    vector<void *> keyVector;
};

class Node
{
    bool isLeaf;
    keys_struct *key;
    int size;
    Node **ptr;
    friend class BPTree;

public:
    Node();
};

class BPTree
{
private:
    Node *root;
    int nodesCount;

    void createInternal(keys_struct x, Node *cursor, Node *child);
    void removeInternal(keys_struct x, Node *cursor, Node *child);
    Node *findParent(Node *cursor, Node *child);

public:
    BPTree();

    Node *search(int lowerBound, bool flag, bool printer);
    Node *rangesearch(int lowerBound, int upperBound, bool flag, bool printer);
    void insert(keys_struct x);
    void remove(keys_struct x);
    void displayRootChild(Node *cursor);
    void nodePrinter(int option, Node *cursor);

    int getNodesCount()
    {
        return nodesCount;
    }

    int getLevels(Node *cursor)
    {
        if (cursor->isLeaf == false)
        {
            return getLevels(cursor->ptr[0]) + 1;
        }
        else if (cursor->isLeaf == true)
        {
            return 1;
        }
        else
        {
            return NULL;
        }
    }

    Node *getRoot()
    {
        return root;
    }
};

#endif