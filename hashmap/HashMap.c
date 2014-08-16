// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdlib.h>
#include <pthread.h>

#include "RTrie.h"
#include "HashMap.h"

inline HashMap *allocHashMap(void) {
    return (HashMap *)malloc(sizeof(HashMap));
}

inline HashMap *newHashMap(const UInt base) {
    HashMap *hm = allocHashMap();
    assert((hm != NULL));
    hm->map = NULL;
    hm->base = base;

    return hm;
}

HashMap *destroyHashMap(HashMap *hm) {
    if (hm != NULL) {
        hm->map = destroyRTrie(hm->map, hm->base);
        pthread_mutex_t destMutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_lock(&destMutex);
        free(hm);
        hm = NULL;
        pthread_mutex_unlock(&destMutex);
    }

    return hm;
}

HashMap *put(HashMap *hm, const ULInt hash, void *data, const UInt allocStyle) {
    return putWithFreer(hm, hash, data, free, allocStyle);
}

HashMap *putWithFreer(
    HashMap *hm, const ULInt hash, void *data, void (*freer)(void *), const UInt allocStyle
) {
    if (hm != NULL) {
        if (hm->map == NULL)
            hm->map = newRTrie(hm->base);

        DataSav dataSav = {.isHeapd=allocStyle, .data=data, .prevFreer=freer};
        hm->map = (RTrie *)putRTrie(hm->map, &dataSav, hash, hm->base);
    }

    return hm;
}

void *get(HashMap *hm, const ULInt hash) {
    void *retr = NULL;
    if (hm != NULL && hm->map != NULL)
        retr = getRTrie(hm->map, NULL, hash, hm->base);

    return retr;
}

HashMap *pop(HashMap *hm, const ULInt hash, void (*prevFreer)(void *), const void **popSave) {
    if (hm != NULL && hm->map != NULL && popSave != NULL) {
        DataSav dSav = {.prevFreer=prevFreer};
        *popSave = popRTrie(hm->map, &dSav, hash, hm->base); 
    }

    return hm;
}

void mapOntoHashMap(HashMap *hm, void (*func)(void *)) {
    if (hm != NULL)
        mapOntoTrie(hm->map, func, hm->base);
}

#ifdef _HASHMAP_MAIN
int main() {
    HashMap *hm = newHashMap(10);
    hm = put(hm, 9, (void *)hm, 0);
    void *retr;
    hm = pop(hm, 9, free, (const void **)&retr);
    printf("retr: %p\n", retr);
    destroyHashMap(hm);
    return 0;
}
#endif // _HASHMAP_MAIN
