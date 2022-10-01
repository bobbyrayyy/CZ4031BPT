#include "bplustree.h"
#include "types.h"

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void BPTree::remove(keys_struct x)
{
    if (root == NULL)
    {
        cout << "Empty B+ Tree" << endl;
    }
    else
    {
        Node *cursor = root;
        Node *parent;
        int leftSibling, rightSibling;
        while (not(cursor->isLeaf))
        {
            for (int i = 0; i < cursor->size; i++)
            {
                parent = cursor;
                leftSibling = i - 1;
                rightSibling = i + 1;
                if (x.key_value < cursor->key[i].key_value)
                {
                    cursor = cursor->ptr[i];
                    break;
                }
                if (i == cursor->size - 1)
                {
                    leftSibling = i;
                    rightSibling = i + 2;
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        bool found = false;
        int pos;
        for (pos = 0; pos < cursor->size; pos++)
        {
            if (cursor->key[pos].key_value == x.key_value)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "Not found" << endl;
            return;
        }
        for (int i = pos; i < cursor->size; i++)
        {
            cursor->key[i] = cursor->key[i + 1];
        }
        cursor->size--;
        if (cursor == root)
        {
            cout << "Deleted " << x.key_value << " from leaf node successfully" << endl;
            for (int i = 0; i < MAX + 1; i++)
            {
                cursor->ptr[i] = NULL;
            }
            if (cursor->size == 0)
            {
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted a node" << endl;
                nodesCount--;
                root = NULL;
            }
            return;
        }
        cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];
        cursor->ptr[cursor->size + 1] = NULL;
        cout << "Deleted " << x.key_value << " from leaf node successfully" << endl;
        if (cursor->size >= (MAX + 1) / 2)
        {
            return;
        }
        if (leftSibling >= 0)
        {
            Node *leftNode = parent->ptr[leftSibling];
            if (leftNode->size >= (MAX + 1) / 2 + 1)
            {
                for (int i = cursor->size; i > 0; i--)
                {
                    cursor->key[i] = cursor->key[i - 1];
                }
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[0] = leftNode->key[leftNode->size - 1];
                leftNode->size--;
                leftNode->ptr[leftNode->size] = cursor;
                leftNode->ptr[leftNode->size + 1] = NULL;
                parent->key[leftSibling] = cursor->key[0];
                return;
            }
        }
        if (rightSibling <= parent->size)
        {
            Node *rightNode = parent->ptr[rightSibling];
            if (rightNode->size >= (MAX + 1) / 2 + 1)
            {
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                cursor->key[cursor->size - 1] = rightNode->key[0];
                rightNode->size--;
                rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
                rightNode->ptr[rightNode->size + 1] = NULL;
                for (int i = 0; i < rightNode->size; i++)
                {
                    rightNode->key[i] = rightNode->key[i + 1];
                }
                parent->key[rightSibling - 1] = rightNode->key[0];
                return;
            }
        }
        if (leftSibling >= 0)
        {
            Node *leftNode = parent->ptr[leftSibling];
            for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++)
            {
                leftNode->key[i] = cursor->key[j];
            }
            leftNode->ptr[leftNode->size] = NULL;
            leftNode->size += cursor->size;
            leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];

            removeInternal(parent->key[leftSibling], parent, cursor);
            delete[] cursor->key;
            delete[] cursor->ptr;
            delete cursor;
            cout << "Deleted 1" << endl;
            nodesCount--;
        }
        else if (rightSibling <= parent->size)
        {
            Node *rightNode = parent->ptr[rightSibling];
            for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++)
            {
                cursor->key[i] = rightNode->key[j];
            }
            cursor->ptr[cursor->size] = NULL;
            cursor->size += rightNode->size;
            cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
            removeInternal(parent->key[rightSibling - 1], parent, rightNode);
            delete[] rightNode->key;
            delete[] rightNode->ptr;
            delete rightNode;
            cout << "Deleted a node" << endl;
            nodesCount--;
        }
    }
}
