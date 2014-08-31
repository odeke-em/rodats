// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _DMAP_H
#define _DMAP_H

    #include "DNode.h"
    #include "HashMap.h"

    typedef struct {
	ULInt size;
	DNode *dnode;
	HashMap *hmap;
    } DMap;

    DMap *newDMap(void);
    inline DMap *allocDMap(void);
    DMap *destroyDMap(DMap *dm);

    void *getDMap(DMap *dm, const ULInt h);
    void *putDMap(DMap *dm, void *data, const ULInt h, const UInt allocStyle);

    DMap *popDMap(DMap *dm, const ULInt h, const void **dSav);
    DMap *pushDMap(DMap *dm, void *data, const ULInt h, const UInt allocStyle);
    int pushDMapOp(DMap *dm, void *data, const ULInt h);

    ULInt pjwCharHash(const char *s);
    DMap *fileToDM(const char *path);

    ULInt getSize(DMap *dm);
#endif // _DMAP_H
