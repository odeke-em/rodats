// Author: Emmanuel Odeke <odeke@ualberta.ca>

#include <stdio.h>
#include <stdlib.h>

#include "MBuf.h"
#include "errors.h"

struct MBuf *freshMBuf(const unsigned int sz) {
    struct MBuf *b = NULL;
    if (sz > 0) {
        b = (struct MBuf*)malloc(sizeof(struct MBuf) + (sizeof(void *) * sz));
        assert((b != NULL));
        b->size = sz;
    }

    return b;
}

struct MBuf *resizeMBuf(struct MBuf *b, const unsigned int rSz) {
    if (b != NULL) {
        b = (struct MBuf *)realloc(b, sizeof(struct MBuf) + sizeof(void *) * rSz);
        assert((b != NULL));
        b->size = rSz;
    }

    return b;
}

struct MBuf *destroyMBuf(struct MBuf *b) {
    if (b != NULL) {
        if (b->buf != NULL) {
            void **trace=b->buf, **end = b->buf + b->size;
            while (trace < end) {
                if (*trace != NULL)
                    free(*trace);

                ++trace;
            }
        }

        free(b);
        b = NULL;
    }

    return b;
}

void _fillBufWithContent(void **buf, void **end, void *(*func)(void)) {
    void **trav = buf;
    while (trav < end) {
        *trav = func();
        ++trav;
    }
}

inline void *twoCharFill(void) {
    return malloc(sizeof(char) * 2);
}

#ifdef _TRY_MBUF_MAIN
int main() {
    struct MBuf *mb = freshMBuf(20);
    _fillBufWithContent(mb->buf, mb->buf+mb->size, twoCharFill);
    mb = destroyMBuf(mb);
    return 0;
}
#endif
