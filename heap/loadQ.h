#ifndef _LOAD_QUEUE_H
#define _LOAD_QUEUE_H
  #include "heap.h"
  typedef void *(*onComplete)(Heap *h, unsigned int id);
  typedef struct Load_ {
    void *data;
    unsigned int id;
    unsigned int thId;
    Heap *srcHeap;
    onComplete completeFunc;
    void *(*complete)(void);
    void *(*compute)(void *, onComplete f);
  } Load;

  Load *createLoad(void *data);  

  inline Load *allocLoad(void);

  inline void initLoad(Load *l);

  unsigned int addLoad(Heap *h, Load *l);

  void printLoad(void *l);
  void freeLoad(void *l);

  inline unsigned int getAvailableId(Heap *lH);

  void registerLoad(Heap *loadHeap, Load *l);
  void produce(unsigned int n, void *(*func)(void *));
  void *loadConsume(void *d);

  Comparison loadComp(const void *, const void *);
#endif
