// Author: Emmanuel Odeke <odeke@ualberta.ca>

#ifndef _DIFFIE_H
#define _DIFFIE_H
    typedef struct MerkleNode_ {
        int id;
        void *buf;
        unsigned int hashCode;
        unsigned long int size;
        struct MerkleNode_ *left, *right;
    } MerkleNode;

    typedef struct {
        MerkleNode *root;
        void **mappedBuf;
        long int stSize;
        unsigned long int length, blkSize, chunkCount, mapLength;
        void *chunkList[];
    } MerkleTree;
    
    inline MerkleNode *allocMerkleNode(void);
    MerkleNode *destroyMerkleNode(MerkleNode *dfn);
    MerkleNode *newMerkleNode(const unsigned int hashCode, const int id);

    MerkleTree *newMerkleTree(const unsigned int chunkCount);
    inline MerkleTree *allocMerkleTree(const unsigned int chunkCount);
    MerkleTree *destroyMerkleTree(MerkleTree *mkt);

    MerkleTree *merkleMMap(const char *path);
    MerkleTree *merkleTreefy(const char *path);

    void printMerkleNode(const MerkleNode *mkn, FILE *ofp);
    void printMerkleTree(const MerkleTree *mkt, FILE *ofp);

    inline long int align(const long int qsz, const long int blockSz);
    unsigned long int redefinedPJWCharHash(const char *, unsigned int start, unsigned int end);
#endif // _DIFFIE_H
