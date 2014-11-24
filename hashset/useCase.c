#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "MBuf.h"
#include "HashSet.h"
#include "errors.h"

typedef unsigned long long int ULLInt;

int main(int argc, char *argv[]) {
    // Goal here is given an arbitrary stream of integers,
    // take out duplicates however, maintain the set order
    ULLInt n = 1000000, rNum, MAX_NUM = 578918283839281739, MIN_NUM=0;
    if (argc >= 2) {
        if (sscanf(argv[1], "%lld", &n) != 1)
            n = 1000000;
        if (argc >= 3) {
            if (sscanf(argv[2], "%lld", &MAX_NUM) != 1)
                MAX_NUM = 78919127392918;
            if (argc >= 4) {
                if (sscanf(argv[3], "%lld", &MIN_NUM) != 1)
                    MIN_NUM = 1;
            }
        }
    }

    printf("argc: %d c: %lld min: %lld max: %lld\n", argc, n, MIN_NUM, MAX_NUM);

    ULLInt i, i_mBuf=0, MBUF_BLK_SZ=sysconf(_SC_PAGE_SIZE);

    struct MBuf *mb = freshMBuf(MBUF_BLK_SZ);
    HashSet *dm = newDMap();
    double rFrac;
    printf("PG_SZ: %lld\n", MBUF_BLK_SZ);
    
    for (i=0; i < n; ++i) {
        if (i_mBuf >= mb->size) {
            printf("%lld/%lld\r", i_mBuf, n);
            mb = resizeMBuf(mb, mb->size + MBUF_BLK_SZ);
            _fillBufWithContent(mb->buf + i_mBuf, mb->buf+mb->size, twoCharFill);
        }

        do {
            rFrac = (float)rand() / RAND_MAX;
            rNum  = rFrac * MAX_NUM;
        } while (rNum  < MIN_NUM || rNum >= MAX_NUM);

        if (pushDMapOp(dm, *(mb->buf + i_mBuf), rNum) == 1)
            ++i_mBuf;
    }

    printf("\ndm->size: %ld\n", getSize(dm));
    free(mb); // Note: Won't be invoking destroyMBuf since memory in buf is freed during destroyDMap
    dm = destroyDMap(dm);
};
