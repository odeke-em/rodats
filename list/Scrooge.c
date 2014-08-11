#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "Scrooge.h"

#define PRODUCER_CAPACITY 30
#define DEFAULT_PRODUCER_CAPACITY 10

#define DEBUG

Producer *allocProducer() {
  return (Producer *)malloc(sizeof(Producer));
}

Producer *initProducer(Producer *prod, const unsigned int capacity) {
  if (prod == NULL) {
    prod = allocProducer();
  }

  prod->consumerCount = 0;
  prod->consumerList = NULL;
  prod->workGenerate = NULL;
  prod->maxCapacity = capacity;

  return prod;
}

Producer *destroyProducer(Producer *prod) {
  if (prod != NULL) {
    if (prod->consumerList != NULL) {
      destroyList(prod->consumerList);
      prod->consumerList = NULL;
    }

    free(prod);
    prod = NULL;
  }

  return prod;
}

void *generateData(void) {
  return NULL;
}

// Cannot add a NULL JOB to the consumer's list
// Returns 0 on SUCCESS, non-zero on FAILURE
int insertJob(Producer *prod, void *job) {
#ifdef DEBUG
  printf("\033[33mIn %s\033[00m\n", __func__);
#endif
  int insertResult = -1;

  if (job == NULL)  {
    raiseWarning("NULL job trying to be inserted");
  } else {
    if (prod != NULL && getListSize(prod->consumerList) < prod->maxCapacity) {
      prod->consumerList = prepend(prod->consumerList, job);
      insertResult = 0;
    } else {
      insertResult = 2;
    } 
  }

  return insertResult;
}

int main() {
  Producer *prod = NULL;
  prod = initProducer(prod, PRODUCER_CAPACITY);

  int i;
  for (i=0; i < 500; ++i) {
    int *jobImitation = (int *)malloc(sizeof(int));
    *jobImitation = i;
    // Remember no NULL jobs allowed
    insertionPhase: {
      int status = insertJob(prod, jobImitation);
      if (status > 0) {
		prod->consumerList = purgeLRU(prod->consumerList);
		goto insertionPhase;
      } else if (! status){
      #ifdef DEBUG
		printf("Successful insertion: %d\n", i);
      #endif
      }
    }
  }

  for (i=300; i < 500; ++i) {
    printf("Searching for : %d\n", i);
    void *data = lookUpEntry(prod->consumerList, &i, intPtrComp);
    printf("Key: %d Data: %p\n", i, data);
  }

  prod = destroyProducer(prod);
  return 0;
}
