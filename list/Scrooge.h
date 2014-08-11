#ifndef _SCROOGE_H
#define _SCROOGE_H
  #include "LRU.h"

  typedef struct {
    LRU *consumerList;
    unsigned int consumerCount,
		 maxCapacity;
    void (*workGenerate)(void *);
  } Producer;

  Producer *allocProducer();
  Producer *initProducer(Producer *prod, const unsigned int capacity);

  // Only adds the job to the producer's consumer list if the producer
  // is not NULL and if there is space on the consumer list
  // Cannot add a NULL JOB to the consumer's list
  // Returns 0 on SUCCESS, non-zero on FAILURE
  int insertJob(Producer *prod, void *job);
#endif
