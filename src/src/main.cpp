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
    std::cout << "=================================Storing Data on Disk======================================" << endl;

    int DISKSIZE = 150000000;

    DiskStorage disk(DISKSIZE, BLOCKSIZE);
    cout << "Blocksize is : " << BLOCKSIZE << endl;

    std::ifstream file("../data/data.tsv");

    vector<tuple<void *, uint>> dataset;
    bool flag = true;
    if (file.is_open())
    {
        std::string line;

        while (std::getline(file, line))
        {
            if (flag)
            {
                flag = false;
                continue;
            }
            Record temp;
            stringstream linestream(line);
            string data;

            strcpy(temp.tconst, line.substr(0, line.find("\t")).c_str());
            std::getline(linestream, data, '\t');
            linestream >> temp.averageRating >> temp.numVotes;

            tuple<void *, uint> dataRecord = disk.writeRecord(sizeof(temp));
            dataset.push_back(dataRecord);

            void *blockAddress = (uchar *)get<0>(dataRecord);
            uint offset = get<1>(dataRecord);

            void *rcdAdr = blockAddress + offset;
            memcpy(rcdAdr, &temp, sizeof(temp));
        }

        file.close();
    }

    tuple<void *, uint> dataRecord = dataset.front();
    size_t nodeBufferSize = BLOCKSIZE - sizeof(bool) - sizeof(int);
    void *blockAddress = (uchar *)get<0>(dataRecord);
    uint offset = get<1>(dataRecord);

    cout << "Block address : " << blockAddress << " with offset " << offset << endl;
    cout << "Node Buffer Siz : " << nodeBufferSize << endl;
    size_t sum = sizeof(blockAddress + offset);
    cout << "Address Size : " << sum << endl;
    int maxKeys = 0;

    while (sum + sizeof(blockAddress + offset) + 8 <= nodeBufferSize)
    {
        sum += (sizeof(blockAddress + offset) + 8);
        maxKeys += 1;
    }
    cout << "Max Keys : " << maxKeys << endl;

    std::cout << "=====================================Experiment 1==========================================" << endl;
    std::cout << "Number of blocks (Allocated) : " << disk.getBlkAllocCount() << endl;
    std::cout << "Size of database : " << (float)disk.getDiskStorageBlksUsed() / 1000000 << " MB" << endl;

    std::cout << "===================================Building B+ Tree========================================" << endl;
    unordered_map<void *, void *> blocksInMemory;
    vector<tuple<void *, uint>>::iterator recordsIterator;
    BPTree node;
    int search_index;

    for (recordsIterator = dataset.begin(); recordsIterator != dataset.end(); ++recordsIterator)
    {
        void *blockAddress = (uchar *)get<0>(*recordsIterator);
        uint offset = get<1>(*recordsIterator);

        if (blocksInMemory.find(blockAddress) == blocksInMemory.end())
        {
            void *mainMemoryBlock = operator new(disk.getBlkSize());
            memcpy(mainMemoryBlock, blockAddress, disk.getBlkSize());

            blocksInMemory[blockAddress] = mainMemoryBlock;
        }

        void *recordAddress = (uchar *)blocksInMemory.at(blockAddress) + offset;
        int numVotes = (*(Record *)recordAddress).numVotes;

        keys_struct key;
        key.key_value = numVotes;
        key.keyVector.push_back((uchar *)blocksInMemory.at(blockAddress) + offset);
        node.insert(key);
    }

    std::cout << "=====================================Experiment 2==========================================" << endl;
    int count = 0;
    std::cout << "Parameter n of B+ Tree : " << MAX << endl;
    std::cout << "Number of nodes in B+ Tree : " << node.getNodesCount() << endl;
    std::cout << "Height of B+ Tree : " << node.getLevels(node.getRoot()) << endl;
    node.displayRootChild(node.getRoot());

    std::cout << "=====================================Experiment 3==========================================" << endl;
    node.search(500, true, true);

    std::cout << "=====================================Experiment 4==========================================" << endl;
    node.rangesearch(30000, 40000, true, true);

    std::cout << "=====================================Experiment 5==========================================" << endl;
    keys_struct key;
    key.key_value = 1000;
    key.keyVector.push_back((uchar *)nullptr);

    int beforeNum = node.getNodesCount();

    node.remove(key);
    int afterNum = node.getNodesCount();
    cout << "Number of times node is deleted/merged : " << beforeNum - afterNum << endl;
    cout << "Number of nodes in updated B+ Tree : " << afterNum << endl;
    cout << "Height of updated B+ Tree : " << node.getLevels(node.getRoot()) << endl;
    node.displayRootChild(node.getRoot());
    return 0;
}
