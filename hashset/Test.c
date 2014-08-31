#include <stdio.h>
#include <string.h>

#include "DMap.h"
#include "errors.h"


int main(int argc, char *argv[]) {
    char *target = __FILE__;
    if (argc >= 2)
        target = argv[1];

    DMap *dm = fileToDM(target);

    // Assuming that the file exists
    assert((dm != NULL));

    char *fDupd = strdup(__func__);
    assert((fDupd != NULL));

    ULInt h = pjwCharHash(fDupd);
    void *prevEvac = putDMap(dm, fDupd, h, 1);

    // Read here
    void *retr = getDMap(dm, h); 
    assert((retr != NULL));

    // Pointer comparison should work as well
    assert((retr == fDupd));

    // Pop it now
    void *evac = NULL;
    ULInt prepop_sz = getSize(dm);
    dm = popDMap(dm, h, (const void **)&evac);
    
    assert((evac != NULL && evac == retr));
    assert((prepop_sz == (getSize(dm) + 1)));

    // Dust to dust
    free(fDupd);
    memset(fDupd, 0, strlen(fDupd));

    // Try that again
    evac = NULL;
    prepop_sz = getSize(dm);
    dm = popDMap(dm, h, (const void **)&evac);

    assert((evac == NULL && evac != retr));
    assert((prepop_sz == (getSize(dm))));

    // Put op here
    prevEvac = putDMap(dm, (void *)dm, h, 0);
    prevEvac = retr;
    retr = getDMap(dm, h);

    assert((retr != prevEvac));
    assert((retr == dm));

    evac = NULL;
    dm = popDMap(dm, h, (const void **)&evac);
    assert((evac == dm));

    printf("dmSize: %ld evacuated: %s\n", dm->size, (char *)evac);
    dm = destroyDMap(dm);

    return 0;
}
