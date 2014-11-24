// Author: Emmanuel Odeke <odeke@ualberta.ca>
// DataStructure that enables acts as a set ie no duplicates,
// yet at the same time in constant time allows: Get, Push, Pop and Put operations

#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>

#include "DMap.h"
#include "DNode.h"
#include "HashMap.h"
#include "errors.h"

// #define DEBUG

static const unsigned int HASH_RADIX = 10;
static pthread_mutex_t dmLock = PTHREAD_MUTEX_INITIALIZER;

inline DMap *allocDMap(void) {
	return (DMap *)malloc(sizeof(DMap));
}

ULInt getSize(DMap *dm) {
        pthread_mutex_lock(&dmLock);
        ULInt res = 0;
        if (dm != NULL)
            res = dm->size;
        pthread_mutex_unlock(&dmLock);
        return res;
}

DMap *newDMap(void) {
	DMap *dm = allocDMap();
	assert((dm != NULL));	

	dm->hmap = newHashMap(HASH_RADIX);
	dm->dnode = NULL;
	dm->size = 0;

	return dm;
}

DMap *pushDMap(DMap *dm, void *data, const ULInt h, const UInt allocStyle) {
	if (dm == NULL)
		dm = newDMap();

	if (dm->hmap == NULL)
		dm->hmap = newHashMap(HASH_RADIX);

	void *retr = get(dm->hmap, h);
	if (retr == NULL) {
		dm->dnode = prependDNode(dm->dnode, data);

		// Enter it as isStackd. Using & 0 to take out unused parameter warning
		dm->hmap = put(dm->hmap, h, (void *)&(dm->dnode), allocStyle & 0);
		++dm->size;
	}

	return dm;
}

int pushDMapOp(DMap *dm, void *data, const ULInt h) {
    if (dm == NULL) { // The reason here is that we could suffer from a memory leak
                      // since the newly allocated dm no longer points to the old
                      // location yet this pointer never gets returned outside of this function
        return -1;
    }

    ULInt pSz = getSize(dm);
    dm = pushDMap(dm, data, h, 0); // AllocStyle can be any value since after all the ref
      
                                   // to the dnode is saved in the hashmap
    return pSz < getSize(dm) ? 1: 0;
}

void *putDMap(DMap *dm, void *data, const ULInt h, const UInt allocStyle) {
	// Evacuates the previously stored content, and returns it
	if (dm == NULL) // Not yet allowing PUT into an empty map
		return NULL;
	void *evac = NULL;
	dm = popDMap(dm, h, (const void **)&evac);
	dm = pushDMap(dm, data, h, allocStyle);

	return evac;
}

DMap *popDMap(DMap *dm, const ULInt h, const void **dSav) {
	if (dm != NULL && dm->hmap != NULL) {
		void *popd = NULL;
		dm->hmap = pop(dm->hmap, h, NULL, (const void **)&popd);

		DNode **nm = (DNode **)popd;
		if (nm != NULL  && *nm != NULL) {
			DNode *sav = NULL;
			if (dm->dnode == *nm) {
			    if (dm->dnode->prev != NULL)
				sav = (*nm)->prev;
			    else
				sav = (*nm)->next;
			}

			*dSav = (*nm)->data;
			(*nm)->data = NULL;
			*nm = destroyLoneDNode(*nm);

			if (sav != NULL)
			    dm->dnode = sav;

			--dm->size;
		}
	}

	return dm;
}

void *getDMap(DMap *dm, const ULInt h) {
	if (dm == NULL || dm->hmap == NULL)
		return NULL;
	else {
		DNode **retr = (DNode **)get(dm->hmap, h);
		if (retr != NULL && *retr != NULL)
			return (*retr)->data;
		return NULL;
	}
}

DMap *destroyDMap(DMap *dm) {
	if (dm != NULL) {
		printf("dm->dnode: %p\n", dm->dnode);
		dm->dnode = destroyDNode(dm->dnode);
		dm->hmap = destroyHashMap(dm->hmap);
		free(dm);
		dm = NULL;
	}

	return dm;
}

ULInt pjwCharHash(const char *s) {
    ULInt h = 0;
    if (s != NULL) {
        ULInt g;
        while (*s != '\0') {
            h = (h << 4) + *s;
            g = h & 0xf0000000;
            if (g) {
                h ^= (g >> 24);
                h ^= g;
            }
            ++s;
        }
    }

    return h;
}

DMap *fileToDM(const char *path) {
    printf("path: %s\n", path);
    DMap *dm = NULL;
    if (path != NULL) {

        int fd = open(path, O_RDONLY);
        if (fd < 0)
            goto doneLoad;
            
        struct stat sb;
        if (stat(path, &sb))
            close(fd);
        else {
            long pageSize = sysconf(_SC_PAGESIZE);
            ULInt mapLen = ((sb.st_size + pageSize - 1)/pageSize) * pageSize;
            printf("mpLen: %ld pgSz: %ld\n", mapLen, pageSize);

            char *fBuf = mmap(NULL, mapLen, PROT_READ, MAP_SHARED, fd, 0);
            close(fd);

            if (fBuf == MAP_FAILED) // TODO: Descriptive error handling
                goto doneLoad;
            else {
                register long int i=0;
                register ULInt j;
                ULInt sLen;

                unsigned int BUF_LEN = 10;

                while (i < sb.st_size) {
                    sLen = BUF_LEN;
                    char c, *s = (char *)malloc(sizeof(char) * sLen);
                    j = 0;
                    while (i < sb.st_size && (c = fBuf[i++]) != EOF && ! isspace(c)) {
                        if (j >= sLen) {
                            sLen += BUF_LEN;
                            s = (char *)realloc(s, sizeof(char) * sLen);
                            assert((s != NULL));
                        }

                        s[j++] = c;
                    }

                    if (! j)
                        free(s);
                    else {
                        s = (char *)realloc(s, sizeof(char) * (j + 1));
                        s[j] = '\0'; 
                        ULInt h = pjwCharHash(s);
                        pthread_mutex_lock(&dmLock);
                        sLen = dm != NULL ? dm->size: 0;
                        dm = pushDMap(dm, (void *)s, h, 1);

                        if (dm->size == sLen) // No insertion was made
                            free(s);
                        else {
                        #ifdef DEBUG
                            printf("s: %s\n", s);
                        #endif // DEBUG
                        }

                        pthread_mutex_unlock(&dmLock);
                    }
                }

                if (munmap(fBuf, mapLen)) {
                    raiseWarning(strerror(errno));
                }
            }
        }
    }

    doneLoad:
        return dm;
}
