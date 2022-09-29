/*
Created by Kai Feng
For CZ4031 B+ Tree implementation and experiments project

This file implements the search/traversal of b+ tree
*/

#include <iostream>
#include <unordered_map>
#include <cstring>
#include <array>
#include <tuple>
#include <vector>
#include <cmath>

#include "bplustree.h"
#include "types.h"
#include "disk_storage.h"

using namespace std;

// Search operation
Node* BPTree::search(int lowerBound, bool flag, bool printer)
{
    float totalAverageRating = 0.000000;
    int countOfIndexNodesAccessed = 1;  // the root node counts as 1 index node accessed
    int printedIndexNodes = 0;
    //search logic
    if (root == NULL)
    {
        cout << "Tree is empty" << endl;
    }
    else
    {
        // Point cursor to root to start searching from root
        Node* cursor = root;

        //in the following while loop, cursor will travel to the leaf node possibly consisting the key
        while (cursor->IS_LEAF == false)
        {
            // Iterate through each item in the node
            for (int i = 0; i < cursor->size; i++)
            {
                // At this point, we have found the pointer to follow to lead us to next node
                if (lowerBound < cursor->key[i].key_value)
                {
                    // Print number and content of non-leaf index nodes here (including the root)
                    if (printer == true && printedIndexNodes < 5) {
                        printf("Non-leaf index node - ");
                        for (int j = 0; j < cursor->size; j++) {
                            cout << cursor->key[j].key_value << " ";
                        }
                        cout << "\n";
                        printedIndexNodes++;
                    }
                    // Increment count of index nodes accessed
                    countOfIndexNodesAccessed++;    
                    // Set cursor to follow this pointer (which points to the correct next path to follow)
                    cursor = cursor->ptr[i];
                    // Break out of for loop to check while condition again (if is leaf)
                    break;
                }
                
                // At this point, we are at the last element in the node, the next node to search is the child node already
                if (i == cursor->size - 1)
                {
                    if (printer == true && printedIndexNodes < 5) {
                        printf("Non-leaf index node - ");
                        for (int j = 0; j < cursor->size; j++) {
                            cout << cursor->key[j].key_value << " ";
                        }
                        cout << "\n";
                        printedIndexNodes++;
                    }
                    // Increment count of index nodes accessed
                    countOfIndexNodesAccessed++;    
                    // Sets cursor to the next index node one level below (child node)
                    cursor = cursor->ptr[i + 1];
                    // Break out of for loop, and check if its a leaf node again (if is leaf)
                    break;
                }
            }
        }
        
        // At this point, we have reached the leaf node

        // Prints out all key values of the leaf index nodes accessed
        if (printer == true && printedIndexNodes < 5) {
            printf("Leaf index node - ");
            for (int j = 0; j < cursor->size; j++) {
                cout << cursor->key[j].key_value << " ";
            }
            cout << "\n";
            printedIndexNodes++;
        }

        for (int i = 0; i < cursor->size; i++)
        {
            if (cursor->key[i].key_value == lowerBound)
            {
                if (flag == true) {
                    cout << "Size: " << cursor->key[i].add_vect.size() << "\n";
                    
                    // Prints number of data blocks, which is the size of the array containing data blocks
                    printf("RECORDSIZE %d", RECORDSIZE);
                    printf("BLOCKSIZE %d", BLOCKSIZE);
                    float res = ceil((cursor->key[i].add_vect.size() * RECORDSIZE) / BLOCKSIZE);
                    printf("Number of data blocks: %f", res);
                    // printf("Number of Data Blocks: %d", ceil(double((cursor->key[i].add_vect.size() * RECORDSIZE) / BLOCKSIZE)));
                    printf("\nadd_vect array: %d", cursor->key[i].add_vect);   // NEW
                    printf("\n");
                    
                    for (int j = 0; j < cursor->key[i].add_vect.size(); ++j) {
                        // Sums up all the averageRatings for each record that matches search condition
                        totalAverageRating = totalAverageRating + float((*(Record *) cursor->key[i].add_vect[j]).averageRating);
                    }
                    // Second for loop just to print top 5 data blocks
                    for (int k = 0; k < 5; ++k) {
                        printf("Data Block: ");
                        printf("%p", (uchar *) cursor->key[i].add_vect[k]);
                        printf("Contents of data block: ");
                        printf("\ntconst: ");
                        cout << (*(Record *) cursor->key[i].add_vect[k]).tconst << " | ";
                        printf("averageRating: ");
                        cout << (*(Record *) cursor->key[i].add_vect[k]).averageRating << " | ";
                        printf("numVotes: ");
                        cout << (*(Record *) cursor->key[i].add_vect[k]).numVotes << "\n";
                    }

                    cout << totalAverageRating << endl;
                    // Prints average of aerageRatings for records whose numVotes == 500
                    cout << "Average of averageRatings from records whose numVotes == 500: " << totalAverageRating / cursor->key[i].add_vect.size() << endl;
                    // Print count of index nodes accessed to get to this point
                    //cout << "Number of index nodes accessed: " << countOfIndexNodesAccessed << endl;
                    printf("\nNumber of index nodes accessed: %d", countOfIndexNodesAccessed);
                }
                return cursor;
            }
        }
        //cout<<"Not found\n";
        return nullptr;
    }
}

// TESTING
Node* BPTree::rangesearch(int lowerBound, int upperBound, bool flag, bool printer){
    float totalAverageRating = 0.000000;
    int countOfIndexNodesAccessed = 1;  // the root node counts as 1 index node accessed
    int countOfRecords = 0;
    bool isFound = false;

    // Question told us to print first 5 index nodes and data blocks, hence we are keeping count, so we don't print more than 5
    int printedIndexNodes = 0;
    int printedDataBlocks = 0;
    
    // Search logic
    if (root == NULL)
    {
        cout << "Tree is empty" << endl;
    }
    else
    {
        // Point cursor to root to start searching from root
        Node* cursor = root;

        // This while loop traverses the B+ tree until we reach the leaf node of the lower bound key
        while (cursor->IS_LEAF == false)
        {
            // Iterate through each item in the node
            for (int i = 0; i < cursor->size; i++)
            {
                // At this point, we have found the pointer to follow to lead us to next node
                if (lowerBound < cursor->key[i].key_value)
                {
                    // Print number and content of non-leaf index nodes here (including the root)
                    if (printer == true && printedIndexNodes < 5) {
                        printedIndexNodes++;
                        printf("Non-leaf index node - ");
                        for (int j = 0; j < cursor->size; j++) {
                            cout << cursor->key[j].key_value << " ";
                        }
                        cout << "\n";
                    }
                    // Increment count of index nodes accessed
                    countOfIndexNodesAccessed++;    
                    // Set cursor to follow this pointer (which points to the correct next path to follow)
                    cursor = cursor->ptr[i];
                    // Break out of for loop to check while condition again (if is leaf)
                    break;
                }
                
                // At this point, we are at the last element in the node, the next node to search is the child node already
                if (i == cursor->size - 1)
                {
                    if (printer == true && printedIndexNodes < 5) {
                        printedIndexNodes++;
                        printf("Non-leaf index node - ");
                        for (int j = 0; j < cursor->size; j++) {
                            cout << cursor->key[j].key_value << " ";
                        }
                        cout << "\n";
                    }
                    // Increment count of index nodes accessed
                    countOfIndexNodesAccessed++;    
                    // Sets cursor to the next index node one level below (child node)
                    cursor = cursor->ptr[i + 1];
                    // Break out of for loop, and check if its a leaf node again (if is leaf)
                    break;
                }
            }
        }
        
        // At this point, we have reached the leaf node

        // Prints out all key values of the leaf index nodes accessed
        if (printer == true && printedIndexNodes < 5) {
            printedIndexNodes++;
            printf("Leaf index node - ");
            for (int j = 0; j < cursor->size; j++) {
                cout << cursor->key[j].key_value << " ";
            }
            cout << "\n";
        }

        // At the leaf level
        // Find the starting point of leaf-level traversal (which is the ele with key == lower bound)
        // for (int i = 0; i < cursor->size; i++)
        // {
        //     // If the cur key value == lower bound, we have reached the starting point of travesal
        //     if (cursor->key[i].key_value == lowerBound)
        //     {
        //         // at this stage, lowerbound is reached
        //         printf("lower bound reached");
        //         break;
        //     }
        // }

        // At this stage, both i and cursor are at the starting key and starting node
        // Start traversing at the leaf node level
        int i = 0;
        
        // cout << "cursor ptr" << cursor->ptr[MAX] << endl;
        //cout << "key value" << cursor->key[i].key_value << endl;
        while(cursor->key[i].key_value < lowerBound){
            i++;
        }
        // printf("lower bound reached!");
        // cout << cursor->key[i].key_value << endl;
        // i is the index of the element that we want to start travesal in leaf node (smallest record that fits condition)
        // cursor->key[i].key_value === 30291

        //printf("ptr");
        for (int x = 0; x<MAX; x++){
            // cout << cursor->ptr[x] << endl;
        }
        
// newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];

// while(cursor->ptr[leafsize]!=null)
        // cout << cursor->ptr[cursor->size] << endl;
        while(cursor->ptr[cursor->size]!=NULL && cursor->key[i].key_value <= upperBound){
            // Print current leaf index node and increment count
            //cout << "cursor ptr" << cursor->ptr[i] << endl;
            //printf("traverse leaf nodes");
            // cout << cursor->key[i].key_value;
            if (printer == true && printedIndexNodes < 5) {
                printedIndexNodes++;
                printf("Leaf index node - ");
                for (int j = 0; j < cursor->size; j++) {
                    cout << cursor->key[j].key_value << " ";
                }
                cout << "\n";
            }
            
            // Print number and content of data blocks
            
            for (int j = 0; j < cursor->key[i].add_vect.size(); ++j) {
                    // Sums up all the averageRatings for each record that matches search condition
                    //printf("in flag");
                    totalAverageRating = totalAverageRating + float((*(Record *) cursor->key[i].add_vect[j]).averageRating);
                    countOfRecords++;
                    isFound = true;

                    // Print data blocks as long as printedDataBlocks < 5 - This prints the first 5 data blocks
                    if (printer == true && printedDataBlocks < 5) {
                        printf("Data Block: ");
                        printf("%p", (uchar *) cursor->key[i].add_vect[j]);
                        printf("Contents of data block: ");
                        printf("\ntconst: ");
                        cout << (*(Record *) cursor->key[i].add_vect[j]).tconst << " | ";
                        printf("averageRating: ");
                        cout << (*(Record *) cursor->key[i].add_vect[j]).averageRating << " | ";
                        printf("numVotes: ");
                        cout << (*(Record *) cursor->key[i].add_vect[j]).numVotes << "\n";
                        printedDataBlocks++;
                    }
            }


            i++;
            // If we reach last element in leaf node, reset index i and point cursor to the next leaf node
            if(i == cursor->size){
                i = 0;
                cursor = cursor->ptr[cursor->size];
                countOfIndexNodesAccessed++;
            }

        }

        // Print stats
        // Print number of data blocks accessed
        // cout << ((float(countOfRecords) * RECORDSIZE) / float(BLOCKSIZE)) << endl;;
        float res = ceil((float(countOfRecords) * RECORDSIZE) / float(BLOCKSIZE));
        printf("Number of data blocks accessed: %f", res);
        // Print number of index nodes accessed
        printf("Number of index nodes accessed: %d", countOfIndexNodesAccessed);
        // Print average of averageRating 
        printf("totalAverageRating: %f", totalAverageRating);
        printf("countOfRecords: %d", countOfRecords);
        cout << "Average of averageRatings from records whose numVotes between 30000 to 40000 (inclusive): " << totalAverageRating / countOfRecords << endl;

        if(isFound){
            return cursor;
        }
        
    }

    return nullptr;

}