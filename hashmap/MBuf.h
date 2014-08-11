// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _MBUF_H
#define _MBUF_H
    struct MBuf {
        unsigned int size;
        void *buf[];
    };
    struct MBuf *freshMBuf(const unsigned int sz);
    struct MBuf *resizeMBuf(struct MBuf *b, const unsigned int rSz);
    struct MBuf *destroyMBuf(struct MBuf *b);
    void _fillBufWithContent(void **buf, void **end, void *(*func)(void));

    inline void *twoCharFill(void);
#endif // _MBUF_H
