#include <math.h>
#include <stdio.h>

#include "Merkle.h"

int main(int argc, char *argv[]) {
    /*
    int level[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
    };
    ssize_t levelSz = sizeof(level)/sizeof(level[0]); 

    register int i=levelSz - 1;
    int l, r;
    while (i >= 1) {
        l = level[i--];
        r = level[i];
        printf("%d/%d\n", l, r); // Simulating chunking of pairs' checksums
    }
    */

    MerkleTree *mkt = merkleTreefy(argc >= 2 ? argv[1]: __FILE__);
    printMerkleTree(mkt, stdout);
    mkt = destroyMerkleTree(mkt);

    return 0;
}
