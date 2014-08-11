// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "RTrie.h"
#include "errors.h"

inline RTrie *allocRTrie(void) {
    return (RTrie *)malloc(sizeof(RTrie));
}

RTrie *newRTrie(const UInt b) {
    if (b < 2)
        raiseError("Minimum b value is 2")
    else {
        RTrie *rt = allocRTrie();
        assert((rt != NULL));

        rt->children = (RTrie **)calloc(b, sizeof(RTrie *));
        assert(rt->children);

        rt->EOS = 0;
        rt->data = NULL;

        return rt;
    }
}

void *getRTrie(RTrie *rt, const DataSav *dSav, ULInt h, const UInt b) {
    return __accessRTrie(rt, dSav, h, b, Get);
}

void *popRTrie(RTrie *rt, const DataSav *dSav, ULInt h, const UInt b) {
    return __accessRTrie(rt, dSav, h, b, Pop);
}

RTrie *putRTrie(RTrie *rt, const DataSav *dSav, ULInt h, const UInt b) {
    return (RTrie *)__accessRTrie(rt, dSav, h, b, Put);
}

void *__accessRTrie(
    RTrie *rt, const DataSav *dSav, ULInt h, const UInt b, const CrudEnum cEnum
) {
    if (rt == NULL) {
        if (cEnum == Get || cEnum == Pop)
            return NULL;

        rt = newRTrie(b);
    }
        
    void *result = NULL;

    if (rt->children != NULL) {
        pthread_mutex_t accessMutex = PTHREAD_MUTEX_INITIALIZER;
        UInt residue;
        RTrie *target = rt;

        pthread_mutex_lock(&accessMutex);
        do {
            residue = h % b;
            if (*(target->children + residue) == NULL) {
                if (cEnum == Get || cEnum == Pop)
                    return NULL;
                else 
                    *(target->children + residue) = newRTrie(b);
            }

            target = *(target->children + residue);
            h /= b;

        } while (h);

        if (target) {
            switch (cEnum) {
                case Pop: {
                    if (target->EOS) {
                        result = target->data;
                        target->EOS = 0;
                        target->data = NULL;
                        target->isHeapd = 0;
                    }
                    break;
                }

                case Get: {
                    if (target->EOS)
                        result = target->data;
                    break;
                }

                case Put: {
                    if (dSav != NULL) {
                        void (*freer)(void *) = free;
                        if (dSav->prevFreer != NULL)
                            freer = dSav->prevFreer;

                        if (target->isHeapd && target->data)
                            freer(target->data);

                        target->EOS = 1;
                        target->data = dSav->data;
                        target->isHeapd = dSav->isHeapd;
                    }

                    result = (void *)rt;
                    break;
                }
            }
        }

        pthread_mutex_unlock(&accessMutex);
    }

    return result;
}

RTrie *destroyRTrie(RTrie *rt, const UInt b) {
    if (rt != NULL) {
        if (rt->children != NULL) {
            RTrie **headPtr = rt->children, **end=headPtr + b;
            while (headPtr < end) {
                if (*headPtr != NULL)
                    *headPtr = destroyRTrie(*headPtr, b);

                ++headPtr;
            }

            free(rt->children);
        }


        pthread_mutex_t freeMutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&freeMutex);
        if (rt->isHeapd)
            free(rt->data); 

        free(rt);
        rt = NULL;
        pthread_mutex_unlock(&freeMutex);
    }

    return rt;
}
