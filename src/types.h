#ifndef TYPES_H
#define TYPES_H

typedef unsigned int uint;

// Defines an address of a record stored as a block address with an offset.
struct Address
{
    void *blockAddress;
    short int offset;
};

// Defines a movie record
struct Record
{
    char tconst[10];     // ID of the movie
    float averageRating; // Average rating of the movie
    uint numVotes;        // Number of votes of the movie
};

#endif