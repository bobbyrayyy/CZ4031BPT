#include <iostream>
#include <unordered_map>
#include <cstring>
#include <array>
#include <tuple>
#include <vector>

#include "../storage/disk_storage.h"
#include "bplustree.h"
#include "types.h"

using namespace std;

Node::Node()
{
    key = new keys_struct[MAX];
    ptr = new Node *[MAX + 1];
    isLeaf = true;
}

BPTree::BPTree()
{
    root = NULL;
    nodesCount = 0;
}

void BPTree::createInternal(keys_struct x, Node *cursor, Node *child)
{
    if (cursor->size < MAX)
    {
        int i = 0;
        while (x.key_value > cursor->key[i].key_value && i < cursor->size)
            i++;

        for (int j = cursor->size; j > i; j--)
        {
            cursor->key[j] = cursor->key[j - 1];
        }
        for (int j = cursor->size + 1; j > i + 1; j--)
        {
            cursor->ptr[j] = cursor->ptr[j - 1];
        }
        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i + 1] = child;
    }
    else
    {
        Node *newInternal = new Node;
        nodesCount++;
        keys_struct iKey[MAX + 1];
        Node *iPtr[MAX + 2];

        for (int i = 0; i < MAX + 1; i++)
        {
            if (i < MAX)
            {
                iKey[i] = cursor->key[i];
            }
            iPtr[i] = cursor->ptr[i];
        }
        int i = 0, j;
        while (x.key_value > iKey[i].key_value && i < MAX)
            i++;

        for (int j = MAX + 1; j > i; j--)
        {
            iKey[j] = iKey[j - 1];
        }
        iKey[i] = x;

        for (int j = MAX + 2; j > i + 1; j--)
        {
            iPtr[j] = iPtr[j - 1];
        }
        iPtr[i + 1] = child;
        newInternal->isLeaf = false;
        cursor->size = (MAX + 1) / 2;
        newInternal->size = MAX - (MAX + 1) / 2;

        for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++)
        {
            newInternal->key[i] = iKey[j];
        }
        for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++)
        {
            newInternal->ptr[i] = iPtr[j];
        }
        if (cursor == root)
        {
            Node *newRoot = new Node;
            nodesCount++;
            newRoot->key[0] = cursor->key[cursor->size];
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newInternal;
            newRoot->isLeaf = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        {
            createInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
        }
    }
}

void BPTree::removeInternal(keys_struct x, Node *cursor, Node *child)
{
    if (cursor == root)
    {
        if (cursor->size == 1)
        {
            if (cursor->ptr[1] == child)
            {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                cout << "Deleted a node" << endl;
                nodesCount--;
                root = cursor->ptr[0];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted a node" << endl;
                return;
            }
            else if (cursor->ptr[0] == child)
            {
                delete[] child->key;
                delete[] child->ptr;
                delete child;
                cout << "Deleted a node" << endl;
                nodesCount--;
                root = cursor->ptr[1];
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted a node" << endl;
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
    if (cursor->size >= (MAX + 1) / 2 - 1)
    {
        cout << "Deleted " << x.key_value << " from internal node successfully" << endl;
        return;
    }
    if (cursor == root)
        return;
    Node *parent = findParent(root, cursor);
    int leftSibling, rightSibling;
    for (pos = 0; pos < parent->size + 1; pos++)
    {
        if (parent->ptr[pos] == cursor)
        {
            leftSibling = pos - 1;
            rightSibling = pos + 1;
            break;
        }
    }
    if (leftSibling >= 0)
    {
        Node *leftNode = parent->ptr[leftSibling];
        if (leftNode->size >= (MAX + 1) / 2)
        {
            for (int i = cursor->size; i > 0; i--)
            {
                cursor->key[i] = cursor->key[i - 1];
            }

            cursor->key[0] = parent->key[leftSibling];
            parent->key[leftSibling] = leftNode->key[leftNode->size - 1];

            for (int i = cursor->size + 1; i > 0; i--)
            {
                cursor->ptr[i] = cursor->ptr[i - 1];
            }

            cursor->ptr[0] = leftNode->ptr[leftNode->size];
            cursor->size++;
            leftNode->size--;
            return;
        }
    }
    if (rightSibling <= parent->size)
    {
        Node *rightNode = parent->ptr[rightSibling];

        if (rightNode->size >= (MAX + 1) / 2)
        {
            cursor->key[cursor->size] = parent->key[pos];
            parent->key[pos] = rightNode->key[0];
            for (int i = 0; i < rightNode->size - 1; i++)
            {
                rightNode->key[i] = rightNode->key[i + 1];
            }

            cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
            for (int i = 0; i < rightNode->size; ++i)
            {
                rightNode->ptr[i] = rightNode->ptr[i + 1];
            }
            cursor->size++;
            rightNode->size--;
            return;
        }
    }
    if (leftSibling >= 0)
    {
        Node *leftNode = parent->ptr[leftSibling];
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
        removeInternal(parent->key[leftSibling], parent, cursor);
    }
    else if (rightSibling <= parent->size)
    {
        Node *rightNode = parent->ptr[rightSibling];
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
        removeInternal(parent->key[rightSibling - 1], parent, rightNode);
    }
}

Node *BPTree::findParent(Node *cursor, Node *child)
{
    Node *parent;
    if (cursor->isLeaf || (cursor->ptr[0])->isLeaf)
    {
        return NULL;
    }
    for (int i = 0; i < cursor->size + 1; i++)
    {
        if (cursor->ptr[i] == child)
        {
            return cursor;
        }
        else
        {
            parent = findParent(cursor->ptr[i], child);
            if (parent != NULL)
                return parent;
        }
    }
    return parent;
}
