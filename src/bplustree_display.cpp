/*
Created by Gareth
For CZ4031 B+ Tree implementation and experiments project

This file implements the displaying of b+ tree via std::cout
*/

#include "bplustree.h"
#include "types.h"

#include <iostream>
#include <cstring>

using namespace std;

int BPTree::display(Node* cursor, int nodecount, bool first) {
    if (first == true) {
        cout << "Content of Root Node: " << endl;
        for (int i = 0; i < cursor->size + 1; i++) {
            if (i != cursor->size) {
                cout << cursor->ptr[i] << " | " << cursor->key[i].key_value << " | ";
            }
        }
        cout << cursor->ptr[cursor->size] << endl;

        cout << "Content of 1st Child Node: " << endl;
        for (int j = 0; j < cursor->ptr[0]->size; j++) {
            cout << cursor->ptr[0]->ptr[j] << " | " << cursor->ptr[0]->key[j].key_value << " | ";
        }
        cout << cursor->ptr[0]->ptr[cursor->ptr[0]->size] << endl;
        return 0;
    }
    else {
        int thisnode;
        if (cursor->IS_LEAF == false) {
            for (int i = 0; i < cursor->size + 1; i++) {
                thisnode = display(cursor->ptr[i], 0, false);
                nodecount = nodecount + thisnode;
            }
            nodecount++;
            return nodecount;
        }
        if (cursor->IS_LEAF == true) {

            for (int i = 0; i < cursor->size; i++) {
                cout << cursor->key[i].key_value << " ";
            }
            return 1;
        }
        return 0;
    }
}