/*
Created by Bob
For CZ4031 B+ Tree implementation and experiments project

This file implements the experiments (1-5)
*/

#include "disk_storage.h"
#include "bplustree.h"
#include "types.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <chrono>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

int main()
{
    std::cout << "=====================================Storing Data on Disk==========================================" << endl;
    // Block size is defined in project as 200B or 500B - defined in disk_storage.h

    // Create disk storage space - let disk be 150MB
    int DISKSIZE = 150000000;

    // Create disk
    MemPool disk(DISKSIZE, BLOCKSIZE);
    cout << "Blocksize is : " << BLOCKSIZE << endl;

    // Read data into file
    std::ifstream file("../data/data.tsv");

    // Create database by allocating memory pool
    vector<tuple<void*, uint>> dataset;
    bool flag = true;
    if (file.is_open())
    {
        std::string line;

        while (std::getline(file, line))
        {
            if (flag) {
                flag = false;
                continue;
            }
            Record temp;
            stringstream linestream(line);
            string data;

            // Give each record its tconst, averageRating and numVotes
            strcpy(temp.tconst, line.substr(0, line.find("\t")).c_str());
            std::getline(linestream, data, '\t');
            linestream >> temp.averageRating >> temp.numVotes;

            tuple<void*, uint> dataRecord = disk.writeRecord(sizeof(temp));
            dataset.push_back(dataRecord);

            void* blockAddress = (uchar*)get<0>(dataRecord);
            uint offset = get<1>(dataRecord);

            void* rcdAdr = blockAddress + offset;
            memcpy(rcdAdr, &temp, sizeof(temp));

            //cout << "Inserted movie " << temp.tconst << " at block address: " << blockAddress << " with offset " << offset << endl;
        }

        file.close();   
    }

    // Set max keys available in a node. Each key is a float, each pointer is a struct of {void *blockAddress, short int offset}.
    // Therefore, each key is 4 bytes. Each pointer is around 16 bytes.

    // Initialize node buffer with a pointer. P | K | P , always one more pointer than keys.
    tuple<void*, uint> dataRecord = dataset.front();
    size_t nodeBufferSize = BLOCKSIZE - sizeof(bool) - sizeof(int);
    void* blockAddress = (uchar*)get<0>(dataRecord);
    uint offset = get<1>(dataRecord);

    cout << "block address : " << blockAddress << " with offset " << offset << endl;
    cout << "Node Buffer Siz : " << nodeBufferSize << endl;
    size_t sum = sizeof(blockAddress + offset);
    cout << "Address Size : " << sum << endl;
    int maxKeys = 0;
 
    // Try to fit as many pointer key pairs as possible into the node block.
    while (sum + sizeof(blockAddress + offset) + 8 <= nodeBufferSize)
    {
        sum += (sizeof(blockAddress + offset) + 8);
        maxKeys += 1;
    }
    cout << "Max Keys : " << maxKeys << endl;

    std::cout << "\n=====================================Experiment 1==========================================" << endl;
    std::cout << "Number of blocks (Allocated) : " << disk.getNumAllocBlks() << endl;
    std::cout << "Size of database : " << (float)disk.getMemPoolUsedBlks() / 1000000 << " MB" << endl;

    std::cout << "=====================================Building B+ Tree==========================================" << endl;
    unordered_map<void*, void*> blocksInMemory;
    vector<tuple<void*, uint>>::iterator recordsIterator;
    BPTree node;
    int search_index;
    
    for (recordsIterator = dataset.begin(); recordsIterator != dataset.end(); ++recordsIterator) {
        void* blockAddress = (uchar*)get<0>(*recordsIterator);
        uint offset = get<1>(*recordsIterator);

        if (blocksInMemory.find(blockAddress) == blocksInMemory.end()) {
            void* mainMemoryBlock = operator new(disk.getBlkSize());
            memcpy(mainMemoryBlock, blockAddress, disk.getBlkSize());

            blocksInMemory[blockAddress] = mainMemoryBlock;
        }

        void* recordAddress = (uchar*)blocksInMemory.at(blockAddress) + offset;
        int numVotes = (*(Record*)recordAddress).numVotes;

        keys_struct key;
        key.key_value = numVotes;
        key.add_vect.push_back((uchar*)blocksInMemory.at(blockAddress) + offset);
        node.insert(key);
    }

    std::cout << "\n=====================================Experiment 2==========================================" << endl;
    int count = 0;
    std::cout << "Parameter n of B+ Tree : " << MAX << endl;
    std::cout << "Number of nodes in B+ Tree : " << node.getNodesCount() << endl;
    std::cout << "Height of B+ Tree : " << node.getLevels(node.getRoot()) << endl;
    node.display(node.getRoot(), count, true);


    // Experiment 3: retrieve those movies with the “numVotes” equal to 500 and report the following statistics:
    // - the number and the content of index nodes the process accesses; (for the content, it would be sufficient to report for the first 5 index nodes or data blocks only if there are more than 5, and this applies throughout Experiment 3 and Experiment 4).
    // - the number and the content of data blocks the process accesses;
    // - the average of “averageRating’s” of the records that are returned;
    std::cout << "\n=====================================Experiment 3==========================================" << endl;
    // Retrieve records with numVotes == 500 and print out Size and Number of Data blocks accessed
    // node.search(500, true, false);
    node.search(500, true, true);

    // Find the average of averageRating's of the records that are returned

    std::cout << "\n=====================================Experiment 4==========================================" << endl;
    // Retrieve records with numVotes == 500 and print out Size and Number of Data blocks accessed
    // node.search(500, true, false);
    // node.search(30000,40000, true,true);

    // Find the average of averageRating's of the records that are returned
    node.rangesearch(30000, 40000, true, true);


    std::cout << "\n=====================================Experiment 5==========================================" << endl;
    keys_struct key;
    key.key_value = 1000;
    key.add_vect.push_back((uchar*) nullptr);

    int beforeNum =  node.getNodesCount();

    node.remove(key);
    cout << "Try3" << endl;
    int afterNum = node.getNodesCount();
    cout << "Number of times node is deleted/merged : " << beforeNum - afterNum << endl;
    cout << "Number of nodes in updated B+ Tree : " << afterNum << endl;
    cout << "Height of updated B+ Tree : " << node.getLevels(node.getRoot()) << endl;
    cout << node.display(node.getRoot(), count, true) << endl;

    return 0;
}
