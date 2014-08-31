// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DNode.h"
#include "errors.h"

inline DNode *allocDNode(void) {
    return (DNode *)malloc(sizeof(DNode));
}

DNode *newDNode(void *data) {
    DNode *dn = allocDNode();
    assert((dn != NULL));

    dn->data = data;
    dn->refFreeTag = Clean; // Virgin/UnFreed
    dn->dataDestroyer = NULL;
    dn->prev = NULL;
    dn->next = NULL;

    return dn;
}

DNode *prependDNodeDestroyer(DNode *cur, void *data, void (*freer)(void *)) {
    DNode *dn = newDNode(data);
    dn->dataDestroyer = freer;
    if (cur != NULL) {
        dn->prev = cur->prev;
    }

    dn->next = cur;
    cur = dn;
    return cur;
}

DNode *prependDNode(DNode *cur, void *data) {
    return prependDNodeDestroyer(cur, data, NULL);
}

DNode *appendDNode(DNode *cur, void *data) {
    DNode *nh = newDNode(data);

    if (cur != NULL) {
        nh->next = cur->next;
        nh->prev = cur;
        cur->next = nh;
    }
    else
        cur = nh;
    
    return cur;
}

DNode *destroyLoneDNode(DNode *dn) {
    if (dn != NULL) {
        if (dn->data != NULL) {
            if (dn->dataDestroyer != NULL)
                dn->dataDestroyer(dn->data);
            else
                free(dn->data);

            dn->data = NULL;
        }

        if (dn->prev != NULL)
            dn->prev->next = dn->next;

        if (dn->next != NULL)
            dn->next->prev = dn->prev;

        dn->refFreeTag = Dirty;
        free(dn);
        dn = NULL;
    }

    return dn;
}

DNode *destroyDNode(DNode *dn) {
    if (dn != NULL) {

        DNode *tmp;
        while (dn != NULL) {
            tmp = dn->next;
            if (dn->refFreeTag == Clean) {
                if (dn->data != NULL) {
                #ifdef DEBUG
                    printf("dn->data: %s\n", (char *)dn->data);
                #endif
                    if (dn->dataDestroyer != NULL)
                        dn->dataDestroyer(dn->data);
                    else
                        free(dn->data);

                    dn->data = NULL;
                }

                free(dn);                
                dn->refFreeTag = Dirty;
            }

            dn = tmp;
        }

        dn = NULL;
    }

    return dn;
}

unsigned int isCircular(DNode *dn) {
    if (dn == NULL)
        return 0;
        
    if (dn->next == NULL)
        return dn->prev == dn;

    if (dn->next->next == NULL)
        return dn == dn->next;

    DNode *n1 = dn, *n2 = dn->next->next;
    while (n1 != NULL && n2 != NULL) {
        if (n1 == n2)
            return 1;

        n1 = n1->next, n2 = n2->next; 
    }

    return 0;
}

#ifdef DNODE_MAIN
int main() {
    const char *hello = "HelloWorld\0";
    DNode *dn = newDNode((void *)strdup(hello));
    dn  = prependDNode(dn, (void *)strdup(__func__));
    dn  = prependDNode(dn, (void *)strdup(__FILE__));
    printf("Circular: %d\n", isCircular(dn));
    dn = destroyDNode(dn);
    return 0;
}
#endif
