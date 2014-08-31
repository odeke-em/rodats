// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _DNODE_H
#define _DNODE_H

    typedef enum {
        Clean=0, Dirty=1
    } FreeTag;

    typedef struct DNode_ {
        FreeTag refFreeTag; // Broadcast to all memory refs of any freeing of memory.
        void *data;
        void (*dataDestroyer)(void *);
        struct DNode_ *prev, *next;
    } DNode;

    inline DNode *allocDNode(void);

    DNode *newDNode(void *);
    DNode *destroyDNode(DNode *);
    DNode *destroyLoneDNode(DNode *dn);
    DNode *appendDNode(DNode *cur, void *data);
    DNode *prependDNode(DNode *cur, void *data);
    DNode *prependDNodeDestroyer(DNode *cur, void *data, void (*freer)(void *));

    unsigned int isCircular(DNode *dn);
#endif // _DNODE_H
