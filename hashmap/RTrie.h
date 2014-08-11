// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _RTRIE_H
#define _RTRIE_H
    #include "errors.h"
    typedef unsigned int UInt;
    typedef unsigned long int ULInt;

    typedef enum {
        Pop, Get, Put
    } CrudEnum;

    typedef struct {
        void *data;
        UInt isHeapd:1;
        void (*prevFreer)(void *data);
    } DataSav;

    typedef struct _RTrie{
        void *data;
        UInt EOS:1; // EndOfSequence
        UInt isHeapd:1; // Set if data was dynamically allocated
        struct _RTrie **children;
    } RTrie;

    inline RTrie *allocRTrie(void);
    RTrie *newRTrie(const UInt base);
    RTrie *destroyRTrie(RTrie *rt, const UInt base);

    void *popRTrie(RTrie *, const DataSav *, const ULInt hash, const UInt base);
    void *getRTrie(RTrie *, const DataSav *, const ULInt hash, const UInt base);
    RTrie *putRTrie(RTrie *, const DataSav *, const ULInt hash, const UInt base);

    void *__accessRTrie(RTrie *, const DataSav *, ULInt hash, const UInt base, const CrudEnum);
#endif // _RTRIE_H
