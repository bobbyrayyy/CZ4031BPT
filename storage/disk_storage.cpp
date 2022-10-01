#include "disk_storage.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std;

DiskStorage::DiskStorage(uint diskStorageSize, uint blkSize)
{
    this->blkPtr = nullptr;
    this->blkSize = blkSize;
    this->diskStorageSize = diskStorageSize;
    uchar *diskStoragePtr = nullptr;
    this->diskStoragePtr = new uchar[diskStorageSize];
    this->diskStorageBlksUsed = 0;
    this->blkAllocCount = 0;
    this->blkAvailCount = diskStorageSize / blkSize;
    this->curBlkSizeUsed = 0;
}

bool DiskStorage::allocateBlk()
{

    if (blkAvailCount > 0)
    {
        uint sizeAllocated = blkAllocCount * blkSize;
        blkPtr = diskStoragePtr + sizeAllocated;
        diskStorageBlksUsed += blkSize;
        blkAvailCount--;
        blkAllocCount++;
        curBlkSizeUsed = 0;
        return true;
    }

    else
    {
        cout << "Error - No space left in memory" << endl;
        return false;
    }
}

tuple<void *, uint> DiskStorage::writeRecord(uint recordSize)
{

    if (blkSize < (curBlkSizeUsed + recordSize) or blkAllocCount == 0)
    {
        if (!allocateBlk())
            throw "Error - No space left in block/ No blocks have been allocated";
    }

    if (blkSize < recordSize)
    {
        throw "Error - Size of record is greater than size of block";
    }

    tuple<void *, uint> recordAddress(blkPtr, curBlkSizeUsed);

    curBlkSizeUsed += recordSize;

    return recordAddress;
}

bool DiskStorage::deleteRecord(uchar *blkAddress, uint relativeOffset, uint recordSize)
{
    try
    {
        fill(blkAddress + relativeOffset, blkAddress + relativeOffset + recordSize, '\0');

        uchar cmpBlk[recordSize];
        fill(cmpBlk, cmpBlk + recordSize, '\0');

        if (equal(cmpBlk, cmpBlk + recordSize, blkAddress))
        {
            diskStorageBlksUsed -= blkSize;
        }

        return true;
    }

    catch (exception &e)
    {
        cout << "Error - Unable to delete record" << endl;
        return false;
    }
}

DiskStorage::~DiskStorage()
{
    delete diskStoragePtr;
    diskStoragePtr = nullptr;
}