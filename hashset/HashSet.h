#ifndef _HASH_SET_H
#define _HASH_SET_H
    #include "DMap.h"
    typedef DMap HashSet;
    HashSet *(*fileToHashSet)(const char *);
#endif // _HASH_SET_H
