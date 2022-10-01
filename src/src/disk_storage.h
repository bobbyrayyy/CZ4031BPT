#ifndef DISK_STORAGE_H
#define DISK_STORAGE_H
#include <cstring>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

// const int MAX = 30; // 30 is for 500, 11 is for 200
const int MAX = 11;
const int RECORDSIZE = 20;
// const int BLOCKSIZE = 500; // Or change to 500B manually
const int BLOCKSIZE = 200;
typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std;

class DiskStorage
{
private:
    uchar *diskStoragePtr;
    uchar *blkPtr;

    uint diskStorageSize;
    uint blkSize;
    uint diskStorageBlksUsed;
    uint curBlkSizeUsed;

    int blkAvailCount;
    int blkAllocCount;

public:
    DiskStorage(uint diskStorageSize, uint blkSize);
    ~DiskStorage();
    bool allocateBlk();
    tuple<void *, uint> writeRecord(uint recordSize);
    bool deleteRecord(uchar *blkAddress, uint offset, uint recordSize);

    uint getBlkSize()
    {
        return blkSize;
    }

    uint getDiskStorageSize()
    {
        return diskStorageSize;
    }

    int getDiskStorageBlksUsed()
    {
        return diskStorageBlksUsed;
    }

    int getCurBlkSizeUsed()
    {
        return curBlkSizeUsed;
    }

    int getBlkAllocCount()
    {
        return blkAllocCount;
    }

    int getBlkAvailCount()
    {
        return blkAvailCount;
    }
};

#endif
