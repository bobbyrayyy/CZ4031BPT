#include "bplustree.h"
#include "types.h"
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void BPTree::insert(keys_struct x)
{
    if (root == NULL)
    {
        root = new Node;
        nodesCount++;
        root->key[0] = x;
        root->size = 1;
        root->isLeaf = true;
        return;
    }

    Node *cursor = root;

    Node *parent;
    Node *result;
    result = search(x.key_value, false, false);
    if (result != nullptr)
    {
        for (int i = 0; i < result->size; i++)
        {
            if (result->key[i].key_value == x.key_value)
            {
                result->key[i].keyVector.push_back(x.keyVector[0]);
                break;
            }
        }
        return;
    }

    while (not(cursor->isLeaf))
    {
        parent = cursor;
        int i = 0;
        while (i < cursor->size)
        {
            if (x.key_value < cursor->key[i].key_value)
            {
                cursor = cursor->ptr[i];
                break;
            }
            if (i == cursor->size - 1)
            {
                cursor = cursor->ptr[i + 1];
                break;
            }
            i++;
        }
    }

    if (cursor->size < MAX)
    {

        int i = 0;
        while (x.key_value > cursor->key[i].key_value && i < cursor->size)
            i++;

        for (int j = cursor->size; j > i; j--)
        {
            cursor->key[j] = cursor->key[j - 1];
        }
        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
        cursor->ptr[cursor->size - 1] = NULL;
    }
    else
    {

        Node *newLeaf = new Node;
        nodesCount++;

        keys_struct iNode[MAX + 1];
        for (int i = 0; i < MAX; i++)
        {
            iNode[i] = cursor->key[i];
        }
        int i = 0, j;
        while (x.key_value > iNode[i].key_value && i < MAX)
            i++;

        for (int j = MAX + 1; j > i; j--)
        {
            iNode[j] = iNode[j - 1];
        }
        iNode[i] = x;
        newLeaf->isLeaf = true;

        cursor->size = (MAX + 1) / 2;
        newLeaf->size = MAX + 1 - (MAX + 1) / 2;

        cursor->ptr[cursor->size] = newLeaf;

        newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
        cursor->ptr[MAX] = NULL;

        for (i = 0; i < cursor->size; i++)
        {
            cursor->key[i] = iNode[i];
        }
        for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++)
        {
            newLeaf->key[i] = iNode[j];
        }

        if (cursor == root)
        {

            Node *newRoot = new Node;
            nodesCount++;
            newRoot->key[0] = newLeaf->key[0];
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newLeaf;
            newRoot->isLeaf = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        {

            createInternal(newLeaf->key[0], parent, newLeaf);
        }
    }
    return;
}