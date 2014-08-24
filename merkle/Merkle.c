// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "errors.h"
#include "Merkle.h"

inline MerkleNode *allocMerkleNode(void) {
    return (MerkleNode *)malloc(sizeof(MerkleNode));
}

MerkleNode *newMerkleNode(const unsigned int hashCode, const int id) {
    MerkleNode *dfn = allocMerkleNode();
    assert(dfn != NULL);

    dfn->id = id;
    dfn->buf = NULL;
    dfn->hashCode = hashCode;
    dfn->left = dfn->right = NULL;

    return dfn;
}

MerkleNode *destroyMerkleNode(MerkleNode *dfn) {
    if (dfn != NULL) {
        dfn->buf = NULL;
        dfn->left = destroyMerkleNode(dfn->left);
        dfn->right = destroyMerkleNode(dfn->right);
        free(dfn);
        dfn = NULL;
    }

    return dfn;
}

inline long int align(const long int qsz, const long int blockSz) {
    return (qsz + blockSz - 1) & ~(blockSz - 1);
}

inline MerkleTree *allocMerkleTree(const unsigned int chunkCount) {
    return (MerkleTree *)malloc(sizeof(MerkleTree) + (sizeof(void *) * chunkCount));
}

MerkleTree *newMerkleTree(const unsigned int chunkCount) {
    MerkleTree *dft = allocMerkleTree(chunkCount);
    assert(dft != NULL && dft->chunkList != NULL);

    dft->root = NULL;
    dft->length = chunkCount;
    dft->chunkCount = chunkCount;
    return dft;
}

unsigned long int redefinedPJWCharHash(const char *str, unsigned int i, unsigned int end) {
    if (str == NULL)
        return 0;

    register unsigned long int h=0, g=0;

    while (i < end) {
        h = (h << 4) + str[i++];
        g = h & 0xf0000000;
        if (g) {
            h ^= g >> 24;
            h ^= g;
        }
    }

    return h;
}

inline unsigned int leftChild(const unsigned int i) {
    return (2 * i + 1);
}

inline unsigned int rightChild(const unsigned int i) {
    return (2 * i + 2);
}

MerkleTree *destroyMerkleTree(MerkleTree *mkt) {
    if (mkt != NULL) {
        mkt->root = destroyMerkleNode(mkt->root);
        free(mkt);
        mkt = NULL;
    }

    return mkt;
}

MerkleTree *merkleMMap(const char *path) {
    MerkleTree *dft = NULL;
    if (path != NULL && *path != '\0') {
        int fd = open(path, O_RDONLY);
        struct stat stSav;
        if (! (fd >= 0 && fstat(fd, &stSav) == 0)) {
            raiseWarning("Tried opening/stat-ing %s got error: %s\n", path, strerror(errno));
        } else if (stSav.st_size <= 0) {
            raiseWarning("Not allowing mapping on empty file: %p\n", path);
        } else {
            long int blkSize = sysconf(_SC_PAGESIZE);
            long int mapLength = align(stSav.st_size, blkSize);
            void **mappage = mmap(NULL, mapLength, PROT_READ, MAP_PRIVATE, fd, 0);
            if (mappage == MAP_FAILED) {
                raiseWarning("MMap of %s failed. Error: %s\n", path, strerror(errno));
            } else {
                MerkleTree *dft = newMerkleTree(
                    stSav.st_size > blkSize ? ceil(((float)stSav.st_size)/blkSize): 1
                );

                printf("dft: %p cc: %ld sz: %ld bk: %ld\n", dft, dft->chunkCount/1, (long int)stSav.st_size, blkSize/1);
                unsigned int isAtEnd=0;
                long int end=0, start;
                unsigned long int id;

                for (id=0, end=0; id < dft->chunkCount && isAtEnd == 0; ++id) {
                    start = end;
                    end += blkSize;
                    if (end >= stSav.st_size) {
                        isAtEnd = 1;
                        end = stSav.st_size - 1;
                    }
                    printf("#id: %ld s: %ld e: %ld maxCC: %ld\n", id, start, end, dft->chunkCount);

                    MerkleNode *dfn = newMerkleNode(
                        redefinedPJWCharHash((char *)mappage, start, end), id
                    );
                    printf("dfn: %d\n", dfn->hashCode);

                    dft->chunkList[id] = (void *)dfn;
                }

                // Time for hierachy instantiation
                if (id >= 1) {
                    dft->root = dft->chunkList[0];
                }
    
                unsigned long int j;
                end = dft->chunkCount >> 1;
                for (start=0; start <= end; ++start) {
                    id = leftChild(start); j = rightChild(start); // Re-using variables
                    MerkleNode *dfn = dft->chunkList[start];
                    if (id < dft->chunkCount)
                        dfn->left = dft->chunkList[id];
                    if (j < dft->chunkCount)
                        dfn->right = dft->chunkList[j];
                }
            }
        }
    }

    return dft;
}
