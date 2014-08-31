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
    MerkleTree *mkt = allocMerkleTree(chunkCount);
    assert(mkt != NULL && mkt->chunkList != NULL);

    mkt->root = NULL;
    mkt->length = chunkCount;
    mkt->chunkCount = chunkCount;
    return mkt;
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

        if (mkt->mappedBuf != NULL && mkt->mappedBuf != MAP_FAILED) {
            int result = munmap(mkt->mappedBuf, mkt->mapLength);
            if (result != 0) {
                raiseWarning("Failed to munmap buf. Got result: %d error: %s\n", result, strerror(errno));
            }

            mkt->mappedBuf = NULL;
        }

        free(mkt);
        mkt = NULL;
    }

    return mkt;
}

MerkleTree *merkleMMap(const char *path) {
    MerkleTree *mkt = NULL;
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
                mkt = newMerkleTree(
                    stSav.st_size > blkSize ? ceil(((float)stSav.st_size)/blkSize): 1
                );

                mkt->blkSize = blkSize;
                mkt->mappedBuf = mappage;
                mkt->mapLength = mapLength;
                mkt->stSize = stSav.st_size;
            }
        }
    }

    return mkt;
}

void printMerkleTree(const MerkleTree *mkt, FILE *ofp) {
    if (mkt != NULL) {
        fprintf(ofp, "{chunkCount: %ld, totalSize: %ld, blkSize: %ld, mapLength: %ld, root: ",
            mkt->chunkCount, mkt->stSize, mkt->blkSize, mkt->mapLength
        );
        printMerkleNode(mkt->root, ofp);
        fputc('}', ofp);
    }
}

void printMerkleNode(const MerkleNode *mkn, FILE *ofp) {
    if (mkn != NULL) {
        fprintf(ofp, "[id:%d, hash:%ld]", mkn->id, mkn->hashCode);
        if (mkn->left != NULL) {
            fputc(',', ofp);
            printMerkleNode(mkn->left, ofp);
        }

        if (mkn->right != NULL) {
            fputc(',', ofp);
            printMerkleNode(mkn->right, ofp);
        }
    }
}

MerkleTree *merkleTreefy(const char *path) {
    MerkleTree *mkt = merkleMMap(path);

    if (mkt != NULL) {
        unsigned int isAtEnd=0;
        long int end=0, start;
        unsigned long int id;

        MerkleNode *mkn = NULL;
        for (id=0, end=0; id < mkt->chunkCount && isAtEnd == 0; ++id) {
            start = end;
            end += mkt->blkSize;
            if (end >= mkt->stSize) {
                isAtEnd = 1;
                end = mkt->stSize;
            }

            mkn = newMerkleNode(
                redefinedPJWCharHash((char *)mkt->mappedBuf, start, end), id
            );

        #ifdef DEBUG
            printf("HashCode: %d\n", mkn->hashCode);
        #endif

            mkt->chunkList[id] = (void *)mkn;
        }

        // Time for hierachy instantiation
        if (id >= 1) {
            mkt->root = mkt->chunkList[0];
        }
    
        unsigned long int j;
        end = mkt->chunkCount >> 1;
      
        for (start=0; start <= end; ++start) {
            id = leftChild(start); j = rightChild(start); // Re-using variables
            mkn = mkt->chunkList[start];
            if (id < mkt->chunkCount)
                mkn->left = mkt->chunkList[id];
            if (j < mkt->chunkCount)
                mkn->right = mkt->chunkList[j];
        }
    }

    return mkt;
}
