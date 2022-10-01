# CZ4031 Project

## Introduction
This project is for NTU's CZ4031 module. 
It is a implementation of a B+ tree index component and a simulated disk storage component to store records from data/data.tsv, which contain information about movies from IMDB.

The project is written in C++.


## Compilation and running code
To compile and run the code, you will first need a C++ compiler (e.g. g++).

1. Download all files from the repo (including data folder)
```bash
git pull https://github.com/bobbyrayyy/cz4031BPT
```

2. cd to the 'src' directory
```bash
cd src
```

3. Compile each '.cpp' file. 

(MacOS users: I added -std=c++11 flag to solve compilation error.)
```bash
g++ -c main.cpp;
g++ -c bplustree.cpp;
g++ -c bplustree_delete.cpp;
g++ -c bplustree_insert.cpp;
g++ -c bplustree_search.cpp;
g++ -c disk_storage.cpp;
```

4. Link main.exe file
```bash
g++ -o main.exe bplustree_insert.o bplustree_delete.o bplustree_search.o bplustree.o disk_storage.o main.o
```
5. Setting block size and paramenter n
- To run experiment with block size 200B, go to disk_storage.h and uncomment lines 13 and 14, and comment out lines 17 and 18.
- To run experiment with block size 500B, go to disk_storage.h and comment out lines 13 and 14, and uncomment lines 17 and 18.
- (Clear instructions in disk_storage.h)

6. Run main.exe
```bash
./main.exe
```

Experiment results are printed in terminal.

## Contributors
- Bob Lin
- Gareth
- Jin Han
- Kai Feng
- Louis

## References
- https://www.programiz.com/dsa/deletion-from-a-b-plus-tree
- https://www.programiz.com/dsa/b-plus-tree


## License
[MIT](https://choosealicense.com/licenses/mit/)
