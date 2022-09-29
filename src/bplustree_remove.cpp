/*
Created by Jin Han
For CZ4031 B+ Tree implementation and experiments project

This file implements deletion on a b+ tree
*/

// include all the files that are needed
#include "bplustree.h"
#include "types.h"

// include all the libraries needed
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

// To delete a movie from the B+ Tree index. Key: Movies's numVotes
void BPTree::remove(keys_struct x)
{
    if (root == NULL)
    {
        cout << "B+ Tree is empty" << endl;
    }
    else
    {
        Node* cursor = root;
        Node* parent;
        int leftSibling, rightSibling;
        //in the following while loop, cursor will will travel to the leaf node possibly consisting the key
        while (cursor->IS_LEAF == false)
        {
            for (int i = 0; i < cursor->size; i++)
            {
                parent = cursor;
                leftSibling = i - 1; //leftSibling is the index of left sibling in the parent node
                rightSibling = i + 1; //rightSibling is the index of right sibling in the parent node
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
        //in the following for loop, we search for the key if it exists
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

        if (!found)//if key does not exist in that leaf node
        {
            cout << "Not found" << endl;
            return;
        }
        //deleting the key
        for (int i = pos; i < cursor->size; i++)
        {
            cursor->key[i] = cursor->key[i + 1];
        }
        cursor->size--;
        if (cursor == root)//if it is root node, then make all pointers NULL
        {
            cout << "Deleted " << x.key_value << " " << "from leaf node successfully\n";
            for (int i = 0; i < MAX + 1; i++)
            {
                cursor->ptr[i] = NULL;
            }
            if (cursor->size == 0)//if all keys are deleted
            {
                //cout<<"Tree died\n";
                delete[] cursor->key;
                delete[] cursor->ptr;
                delete cursor;
                cout << "Deleted 1" << "\n";
                nodesCount--;
                root = NULL;
            }
            return;
        }
        cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];
        cursor->ptr[cursor->size + 1] = NULL;
        cout << "Deleted " << x.key_value << " " << " from leaf node successfully\n";
        if (cursor->size >= (MAX + 1) / 2)//no underflow
        {
            return;
        }
        //cout<<"Underflow in leaf node!\n";
        //underflow condition
        //first we try to transfer a key from sibling node
        //check if left sibling exists
        if (leftSibling >= 0)
        {
            Node* leftNode = parent->ptr[leftSibling];
            //check if it is possible to transfer
            if (leftNode->size >= (MAX + 1) / 2 + 1)
            {
                //make space for transfer
                for (int i = cursor->size; i > 0; i--)
                {
                    cursor->key[i] = cursor->key[i - 1];
                }
                //shift pointer to next leaf
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                //transfer
                cursor->key[0] = leftNode->key[leftNode->size - 1];
                //shift pointer of leftsibling
                leftNode->size--;
                leftNode->ptr[leftNode->size] = cursor;
                leftNode->ptr[leftNode->size + 1] = NULL;
                //update parent
                parent->key[leftSibling] = cursor->key[0];
                //cout<<"Transferred "<<cursor->key[0].key_value << " " << cursor->key[0].add_vect[0] <<" from left sibling of leaf node\n";
                return;
            }
        }
        if (rightSibling <= parent->size)//check if right sibling exist
        {
            Node* rightNode = parent->ptr[rightSibling];
            //check if it is possible to transfer
            if (rightNode->size >= (MAX + 1) / 2 + 1)
            {
                //shift pointer to next leaf
                cursor->size++;
                cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
                cursor->ptr[cursor->size - 1] = NULL;
                //transfer
                cursor->key[cursor->size - 1] = rightNode->key[0];
                //shift pointer of rightsibling
                rightNode->size--;
                rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
                rightNode->ptr[rightNode->size + 1] = NULL;
                //shift conent of right sibling
                for (int i = 0; i < rightNode->size; i++)
                {
                    rightNode->key[i] = rightNode->key[i + 1];
                }
                //update parent
                parent->key[rightSibling - 1] = rightNode->key[0];
                //cout<<"Transferred "<<cursor->key[cursor->size-1].key_value <<" "<< cursor->key[cursor->size-1].add_vect[0] <<" from right sibling of leaf node\n";
                return;
            }
        }
        //must merge and delete a node
        if (leftSibling >= 0)//if left sibling exist
        {
            Node* leftNode = parent->ptr[leftSibling];
            // transfer all keys to leftsibling and then transfer pointer to next leaf node
            for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++)
            {
                leftNode->key[i] = cursor->key[j];
            }
            leftNode->ptr[leftNode->size] = NULL;
            leftNode->size += cursor->size;
            leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];

            //cout<<"Merging two leaf nodes\n";
            removeInternal(parent->key[leftSibling], parent, cursor);// delete parent node key
            delete[] cursor->key;
            delete[] cursor->ptr;
            delete cursor;
            cout << "Deleted 1" << "\n";
            nodesCount--;
        }
        else if (rightSibling <= parent->size)//if right sibling exist
        {
            Node* rightNode = parent->ptr[rightSibling];
            // transfer all keys to cursor and then transfer pointer to next leaf node
            for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++)
            {
                cursor->key[i] = rightNode->key[j];
            }
            cursor->ptr[cursor->size] = NULL;
            cursor->size += rightNode->size;
            cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
            //cout<<"Merging two leaf nodes\n";
            removeInternal(parent->key[rightSibling - 1], parent, rightNode);// delete parent node key
            delete[] rightNode->key;
            delete[] rightNode->ptr;
            delete rightNode;
            cout << "Deleted 1 \n";
            nodesCount--;
        }
    }
}
