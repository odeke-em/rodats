// Author: Emmanuel Odeke <odeke@ualberta.ca>

#ifndef _HASHMAP_H
#define _HASHMAP_H
    #include "RTrie.h"
    
    typedef struct {
        RTrie *map;
        unsigned int base, count;
    } HashMap;

    inline HashMap *allocHashMap(void);
    inline HashMap *newHashMap(const UInt base);
    HashMap *destroyHashMap(HashMap *);

    void *get(HashMap *hm, const ULInt hash);
    HashMap *put(HashMap *hm, const ULInt hash, void *data, const UInt isHeapd);
    HashMap *pop(HashMap *hm, const ULInt hash, void (*prevFreer)(void *), const void **popSave);

    void mapOntoHashMap(HashMap *hm, void (*func)(void *));
#endif // _HASHMAP_H
