/*
Created by Bob Lin
For CZ4031 B+ Tree implementation and experiments project

This file implements the b+ tree (initialisation of b+ tree and some extra functions)
*/

// include all the files that are needed
#include <iostream>
#include <unordered_map>
#include <cstring>
#include <array>
#include <tuple>
#include <vector>
#include "disk_storage.h"
#include "bplustree.h"
#include "types.h"

using namespace std;

Node::Node() {
    key = new keys_struct[MAX];
    ptr = new Node * [MAX + 1];
    IS_LEAF = true;
}

BPTree::BPTree() {
    root = NULL;
    nodesCount = 0;
}

void BPTree::insertInternal(keys_struct x, Node* cursor, Node* child)
{
    if (cursor->size < MAX)
    {
        //if cursor is not full
        //find the correct position for new key
        int i = 0;
        while (x.key_value > cursor->key[i].key_value && i < cursor->size) i++;
        //make space for new key
        for (int j = cursor->size; j > i; j--)
        {
            cursor->key[j] = cursor->key[j - 1];
        }//make space for new pointer
        for (int j = cursor->size + 1; j > i + 1; j--)
        {
            cursor->ptr[j] = cursor->ptr[j - 1];
        }
        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i + 1] = child;
        // cout<<"Inserted key in an Internal node successfully\n";
    }
    else
    {
        // cout<<"Inserted key in an Internal node successfully\n";
        // cout<<"Overflow in internal node!\nSplitting internal node\n";
        //if overflow in internal node
        //create new internal node
        Node* newInternal = new Node;
        nodesCount++;
        //create virtual Internal Node;
        keys_struct virtualKey[MAX + 1];
        Node* virtualPtr[MAX + 2];
        for (int i = 0; i < MAX; i++)
        {
            virtualKey[i] = cursor->key[i];
        }
        for (int i = 0; i < MAX + 1; i++)
        {
            virtualPtr[i] = cursor->ptr[i];
        }
        int i = 0, j;
        while (x.key_value > virtualKey[i].key_value && i < MAX) i++;
        //make space for new key
        for (int j = MAX + 1; j > i; j--)
        {
            virtualKey[j] = virtualKey[j - 1];
        }
        virtualKey[i] = x;
        //make space for new ptr
        for (int j = MAX + 2; j > i + 1; j--)
        {
            virtualPtr[j] = virtualPtr[j - 1];
        }
        virtualPtr[i + 1] = child;
        newInternal->IS_LEAF = false;
        //split cursor into two nodes
        cursor->size = (MAX + 1) / 2;
        newInternal->size = MAX - (MAX + 1) / 2;
        //give elements and pointers to the new node
        for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++)
        {
            newInternal->key[i] = virtualKey[j];
        }
        for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++)
        {
            newInternal->ptr[i] = virtualPtr[j];
        }
        // m = cursor->key[cursor->size]
        if (cursor == root)
        {
            //if cursor is a root node, we create a new root
            Node* newRoot = new Node;
            nodesCount++;
            newRoot->key[0] = cursor->key[cursor->size];
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newInternal;
            newRoot->IS_LEAF = false;
            newRoot->size = 1;
            root = newRoot;
            //cout<<"Created new root\n";
        }
        else
        {
            //recursion
            //find depth first search to find parent of cursor
            insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
        }
    }
}

void BPTree::removeInternal(keys_struct x, Node* cursor, Node* child)
{
    //deleting the key x first
    //checking if key from root is to be deleted
    if (cursor == root)
    {
        if (cursor->size == 1)//if only one key is left, change root
        {
            if (cursor->ptr[1] == child)
            {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                cout << "Deleted 1" << "\n";
                nodesCount--;
                root = cursor->ptr[0];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted 1" << "\n";
                //cout<<"Changed root node\n";
                return;
            }
            else if (cursor->ptr[0] == child)
            {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                cout << "Deleted 1" << "\n";
                nodesCount--;
                root = cursor->ptr[1];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted 1" << "\n";
                //cout<<"Changed root node\n";
                return;
            }
        }
    }
    int pos;
    for (pos = 0; pos < cursor->size; pos++)
    {
        if (cursor->key[pos].key_value == x.key_value)
        {
            break;
        }
    }
    for (int i = pos; i < cursor->size; i++)
    {
        cursor->key[i] = cursor->key[i + 1];
    }
    //now deleting the pointer child
    for (pos = 0; pos < cursor->size + 1; pos++)
    {
        if (cursor->ptr[pos] == child)
        {
            break;
        }
    }
    for (int i = pos; i < cursor->size + 1; i++)
    {
        cursor->ptr[i] = cursor->ptr[i + 1];
    }
    cursor->size--;
    if (cursor->size >= (MAX + 1) / 2 - 1)//no underflow
    {
        cout << "Deleted " << x.key_value << " " << " from internal node successfully\n";
        return;
    }
    //cout<<"Underflow in internal node!\n";
    //underflow, try to transfer first
    if (cursor == root)return;
    Node* parent = findParent(root, cursor);
    int leftSibling, rightSibling;
    //finding left n right sibling of cursor
    for (pos = 0; pos < parent->size + 1; pos++)
    {
        if (parent->ptr[pos] == cursor)
        {
            leftSibling = pos - 1;
            rightSibling = pos + 1;
            break;
        }
    }
    //try to transfer
    if (leftSibling >= 0)//if left sibling exists
    {
        Node* leftNode = parent->ptr[leftSibling];
        //check if it is possible to transfer
        if (leftNode->size >= (MAX + 1) / 2)
        {
            //make space for transfer of key
            for (int i = cursor->size; i > 0; i--)
            {
                cursor->key[i] = cursor->key[i - 1];
            }
            //transfer key from left sibling through parent
            cursor->key[0] = parent->key[leftSibling];
            parent->key[leftSibling] = leftNode->key[leftNode->size - 1];
            //transfer last pointer from leftnode to cursor
            //make space for transfer of ptr
            for (int i = cursor->size + 1; i > 0; i--)
            {
                cursor->ptr[i] = cursor->ptr[i - 1];
            }
            //transfer ptr
            cursor->ptr[0] = leftNode->ptr[leftNode->size];
            cursor->size++;
            leftNode->size--;
            //cout<<"Transferred "<<cursor->key[0].key_value<<" "<< x.add_vect[0] <<" from left sibling of internal node\n";
            return;
        }
    }
    if (rightSibling <= parent->size)//check if right sibling exist
    {
        Node* rightNode = parent->ptr[rightSibling];
        //check if it is possible to transfer
        if (rightNode->size >= (MAX + 1) / 2)
        {
            //transfer key from right sibling through parent
            cursor->key[cursor->size] = parent->key[pos];
            parent->key[pos] = rightNode->key[0];
            for (int i = 0; i < rightNode->size - 1; i++)
            {
                rightNode->key[i] = rightNode->key[i + 1];
            }
            //transfer first pointer from rightnode to cursor
            //transfer ptr
            cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
            for (int i = 0; i < rightNode->size; ++i)
            {
                rightNode->ptr[i] = rightNode->ptr[i + 1];
            }
            cursor->size++;
            rightNode->size--;
            //cout<<"Transferred "<<cursor->key[0].key_value << " " << x.add_vect[0] <<" from right sibling of internal node\n";
            return;
        }
    }
    //transfer wasnt posssible hence do merging
    if (leftSibling >= 0)
    {
        //leftnode + parent key + cursor
        Node* leftNode = parent->ptr[leftSibling];
        leftNode->key[leftNode->size] = parent->key[leftSibling];
        for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++)
        {
            leftNode->key[i] = cursor->key[j];
        }
        for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++)
        {
            leftNode->ptr[i] = cursor->ptr[j];
            cursor->ptr[j] = NULL;
        }
        leftNode->size += cursor->size + 1;
        cursor->size = 0;
        //delete cursor
        removeInternal(parent->key[leftSibling], parent, cursor);
        //cout<<"Merged with left sibling\n";

    }
    else if (rightSibling <= parent->size)
    {
        //cursor + parent key + rightnode
        Node* rightNode = parent->ptr[rightSibling];
        cursor->key[cursor->size] = parent->key[rightSibling - 1];
        for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++)
        {
            cursor->key[i] = rightNode->key[j];
        }
        for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++)
        {
            cursor->ptr[i] = rightNode->ptr[j];
            rightNode->ptr[j] = NULL;
        }
        cursor->size += rightNode->size + 1;
        rightNode->size = 0;
        //delete cursor
        removeInternal(parent->key[rightSibling - 1], parent, rightNode);
        //cout<<"Merged with right sibling\n";
    }
}

Node* BPTree::findParent(Node* cursor, Node* child)
{
    //finds parent using depth first traversal and ignores leaf nodes as they cannot be parents
    //also ignores second last level because we will never find parent of a leaf node during insertion using this function
    Node* parent;
    if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF)
    {
        return NULL;
    }
    for (int i = 0; i < cursor->size + 1; i++)
    {
       if (cursor->ptr[i] == child)
       {
            parent = cursor;
            return parent;
        }
        else
        {
           parent = findParent(cursor->ptr[i], child);
           if (parent != NULL)return parent;
        }
    }
    return parent;
}

 