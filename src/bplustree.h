#ifndef BPLUSTREE_H
#define BPLUSTREE_H

// Include all the files that are needed
#include "types.h"
#include "disk_storage.h"

// Include all the libraries needed
#include <cstddef>
#include <array>

struct keys_struct {
    int key_value;
    vector <void*> add_vect;
};

class Node
{
    bool IS_LEAF;
    keys_struct* key;
    int size;
    Node** ptr;
    friend class BPTree;    // Allow BPTree to see Node class
public:
    Node();
};

// The actual b+ tree structure
class BPTree
{
private:
    // Variables
    Node *root;           // Pointer to the root node
    int nodesCount;       // Count of num of nodes in b+ tree

    // Functions (Private)
    void insertInternal(keys_struct x, Node* cursor, Node* child);
    void removeInternal(keys_struct x, Node* cursor, Node* child);
    Node* findParent(Node* cursor, Node* child);

public:
    // Constructor - input blockSize is used in constructor to find out how many keys and pointers can fit in a node.
    BPTree();

    Node* search(int lowerBound, bool flag, bool printer);
    Node* rangesearch(int lowerBound, int upperBound, bool flag, bool printer);
    void insert(keys_struct x);
    void remove(keys_struct x);
    int display(Node* cursor, int nodecount, bool firstChild);

    // Returns nodesCount of B+ Tree
    int getNodesCount()
    {
        return nodesCount;
    }

    int getLevels(Node* cursor) {
        if (cursor->IS_LEAF == false) {
            return getLevels(cursor->ptr[0]) + 1;
        }
        else if (cursor->IS_LEAF == true) {
            return 1;
        }
        else {
            return NULL;
        }
    }

    Node* getRoot()
    {
        return root;
    }

};

#endif