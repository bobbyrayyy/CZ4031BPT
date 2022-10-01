#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

struct Record
{
    char tconst[10];
    float averageRating;
    uint numVotes;
};

#endif