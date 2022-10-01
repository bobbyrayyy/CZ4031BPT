#include <iostream>
#include <unordered_map>
#include <cstring>
#include <array>
#include <tuple>
#include <vector>
#include <cmath>

#include "bplustree.h"
#include "types.h"
#include "../storage/disk_storage.h"

using namespace std;

Node *BPTree::search(int lowerBound, bool flag, bool printer)
{
    float totalAverageRating = 0.000000;
    int countOfIndexNodesAccessed = 1;
    int printedIndexNodes = 0;

    if (root == NULL)
    {
        cout << "Tree is empty" << endl;
    }
    else
    {

        Node *cursor = root;

        while (not(cursor->isLeaf))
        {
            for (int i = 0; i < cursor->size; i++)
            {
                if (lowerBound < cursor->key[i].key_value)
                {
                    if (printer == true && printedIndexNodes < 5)
                    {
                        nodePrinter(2, cursor);
                        printedIndexNodes++;
                    }

                    countOfIndexNodesAccessed++;
                    cursor = cursor->ptr[i];
                    break;
                }

                if (i == cursor->size - 1)
                {
                    if (printer == true && printedIndexNodes < 5)
                    {
                        nodePrinter(2, cursor);
                        printedIndexNodes++;
                    }

                    countOfIndexNodesAccessed++;
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }

        if (printer == true && printedIndexNodes < 5)
        {
            nodePrinter(3, cursor);
            printedIndexNodes++;
        }

        for (int i = 0; i < cursor->size; i++)
        {
            if (cursor->key[i].key_value == lowerBound)
            {
                if (flag == true)
                {
                    cout << "Number of records returned: " << cursor->key[i].keyVector.size() << "\n";
                    float res = ceil((cursor->key[i].keyVector.size() * RECORDSIZE) / BLOCKSIZE);
                    cout << "Number of data blocks: " << res << endl;

                    for (int j = 0; j < cursor->key[i].keyVector.size(); ++j)
                    {
                        totalAverageRating = totalAverageRating + float((*(Record *)cursor->key[i].keyVector[j]).averageRating);
                    }

                    for (int k = 0; k < 5; ++k)
                    {
                        cout << "Data Block: " << (uchar *)cursor->key[i].keyVector[k] << endl;
                        cout << "Contents of data block: " << endl;
                        cout << "tconst: " << (*(Record *)cursor->key[i].keyVector[k]).tconst << " | averageRating: " << (*(Record *)cursor->key[i].keyVector[k]).averageRating << " | numVotes: " << (*(Record *)cursor->key[i].keyVector[k]).numVotes << endl;
                    }

                    cout << "Average of averageRatings from records whose numVotes == 500: " << totalAverageRating / cursor->key[i].keyVector.size() << endl;
                    cout << "Number of index nodes accessed: " << countOfIndexNodesAccessed << endl;
                }
                return cursor;
            }
        }
        return nullptr;
    }
}

Node *BPTree::rangesearch(int lowerBound, int upperBound, bool flag, bool printer)
{
    float totalAverageRating = 0.000000;
    int countOfIndexNodesAccessed = 1;
    int countOfRecords = 0;
    bool isFound = false;
    int printedIndexNodes = 0;
    int printedDataBlocks = 0;

    if (root == NULL)
    {
        cout << "Tree is empty" << endl;
    }
    else
    {
        Node *cursor = root;
        while (not(cursor->isLeaf))
        {
            for (int i = 0; i < cursor->size; i++)
            {
                if (lowerBound < cursor->key[i].key_value)
                {
                    if (printer == true && printedIndexNodes < 5)
                    {
                        printedIndexNodes++;
                        nodePrinter(2, cursor);
                    }

                    countOfIndexNodesAccessed++;
                    cursor = cursor->ptr[i];
                    break;
                }

                if (i == cursor->size - 1)
                {
                    if (printer == true && printedIndexNodes < 5)
                    {
                        printedIndexNodes++;
                        nodePrinter(2, cursor);
                    }

                    countOfIndexNodesAccessed++;
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }

        if (printer == true && printedIndexNodes < 5)
        {
            printedIndexNodes++;
            nodePrinter(3, cursor);
        }

        int i = 0;
        while (cursor->key[i].key_value < lowerBound)
        {
            i++;
        }

        while (cursor->ptr[cursor->size] != NULL && cursor->key[i].key_value <= upperBound)
        {
            if (printer == true && printedIndexNodes < 5)
            {
                printedIndexNodes++;
                nodePrinter(3, cursor);
            }

            for (int j = 0; j < cursor->key[i].keyVector.size(); ++j)
            {
                totalAverageRating = totalAverageRating + float((*(Record *)cursor->key[i].keyVector[j]).averageRating);
                countOfRecords++;
                isFound = true;

                if (printer == true && printedDataBlocks < 5)
                {
                    cout << "Data Block: " << (uchar *)cursor->key[i].keyVector[j] << endl;
                    cout << "Contents of data block: " << endl;
                    cout << "tconst: " << (*(Record *)cursor->key[i].keyVector[j]).tconst << " | averageRating: " << (*(Record *)cursor->key[i].keyVector[j]).averageRating << " | numVotes: " << (*(Record *)cursor->key[i].keyVector[j]).numVotes << endl;
                    printedDataBlocks++;
                }
            }

            i++;

            if (i == cursor->size)
            {
                i = 0;
                cursor = cursor->ptr[cursor->size];
                countOfIndexNodesAccessed++;
            }
        }

        float res = ceil((float(countOfRecords) * RECORDSIZE) / float(BLOCKSIZE));
        cout << "Number of data blocks accessed: " << res << endl;
        cout << "Number of index nodes accessed: " << countOfIndexNodesAccessed << endl;
        cout << "Average of averageRatings from records whose numVotes between 30000 to 40000 (inclusive): " << totalAverageRating / countOfRecords << endl;

        if (isFound)
        {
            return cursor;
        }
    }

    return nullptr;
}

void BPTree::displayRootChild(Node *cursor)
{
    nodePrinter(1, cursor);
    nodePrinter(4, cursor);
    return;
}

void BPTree::nodePrinter(int option, Node *cursor)
{
    if (option == 1)
    {
        cout << "Root Node contents: " << endl;
        for (int i = 0; i < cursor->size + 1; i++)
        {
            if (i != cursor->size)
            {
                cout << cursor->ptr[i] << " | " << cursor->key[i].key_value << " | ";
            }
        }
        cout << cursor->ptr[cursor->size] << endl;
    }
    else if (option == 2)
    {
        cout << "Non-leaf index node: " << endl;
        for (int j = 0; j < cursor->size; j++)
        {
            cout << cursor->ptr[j] << " | " << cursor->key[j].key_value << " | ";
        }
        cout << endl;
    }
    else if (option == 3)
    {
        cout << "Leaf index node: " << endl;
        for (int j = 0; j < cursor->size; j++)
        {
            cout << cursor->key[j].key_value << " | ";
        }
        cout << endl;
    }
    else if (option == 4)
    {
        cout << "First child node contents: " << endl;
        for (int j = 0; j < cursor->ptr[0]->size; j++)
        {
            cout << cursor->ptr[0]->ptr[j] << " | " << cursor->ptr[0]->key[j].key_value << " | ";
        }
        cout << cursor->ptr[0]->ptr[cursor->ptr[0]->size] << endl;
    }
    return;
}