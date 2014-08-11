#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "loadQ.h"

static pthread_mutex_t mainHeap_x = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t sizeMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t mainHeap_dt = PTHREAD_COND_INITIALIZER;

Comparison loadComp(const void *a, const void *b) {
  if (a == NULL) return b == NULL ? Equal : Greater;
  else if (b == NULL) return Less;

  Load *lA = (Load *)a, 
       *lB = (Load *)b;
  if (lA->id != lB->id) return lA->id < lB->id ? Less : Greater;
  else return Equal;
}

inline unsigned int getAvailableId(Heap *lH) {
  return getSize(lH) + 1;
}

void freeLoad(void *l) {
  if (l != NULL) {
    Load *lL = (Load *)l;
    if (lL->data != NULL) free(lL->data);
    free(lL);
  }
}

inline Load *allocLoad(void) {
  return (Load *)malloc(sizeof(Load));
}

inline void initLoad(Load *l) {
  if (l != NULL) {
    l->id = 0;
    l->data = NULL;
    l->compute = NULL;
    l->thId = 0;
    l->complete = NULL;
    l->completeFunc = NULL;
    l->srcHeap = NULL;
  }
}

void *onFinish(Heap *h, unsigned int id) {
  Load similarElem;
  similarElem.id = id;
  void *popd = removeElem(h, &similarElem);
  return popd;
}

Load *createLoad(void *data) {
  Load *l = allocLoad();
  initLoad(l);
  l->data = data;
  return l;
}

unsigned int addLoad(Heap *h, Load *l) {
  unsigned int newId = 0;
  pthread_mutex_lock(&mainHeap_x);
  if (h != NULL) {
    if (l != NULL) {
      newId = l->id = 0;
      l->srcHeap = h;
    }

    heapInsert(h, l);
  }
  pthread_mutex_unlock(&mainHeap_x);
  return newId;
}

void *loadConsume(void *d) {
  void *result = NULL;
  pthread_mutex_lock(&mainHeap_x);
  if (d != NULL) {
    Load *l = (Load *)d;
    unsigned int *t = (unsigned int *)l->data;
    printf("t: %d\n", *t);
    char *msg = malloc(sizeof(char) * (1 + *t));
    int i;
    for (i=0; i < (*t + 1); ++i) {
      msg[i] = 'a' + i;
    }
    msg[i] = '\0';
    result = msg;
    l->id = getAvailableId(l->srcHeap);
    printf("loadId: %d %p\n", l->id, l);
    heapifyFromHead(l->srcHeap);
  #ifdef DEBUG
    printf("\033[96mnew heapify ");
    printHeap(l->srcHeap, printLoad);
  #endif
    pthread_cond_broadcast(&mainHeap_dt);
  }
  pthread_mutex_unlock(&mainHeap_x);
  return result; 
}

void produce(unsigned int n, void *(*func)(void *)) {
  int maxHandle = 8, readyId=-1, nC = 0;
  Heap *wHeap = NULL, *restHeap = NULL;
  wHeap = initHeap(wHeap, loadComp, freeLoad);
  restHeap = initHeap(restHeap, loadComp, freeLoad);
  unsigned int minThreshold = maxHandle > n ? n : maxHandle;
  pthread_t thList[minThreshold];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
#ifdef __linux__
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#endif

  for (readyId=0; readyId < minThreshold; ++readyId) {
    unsigned int *iNew = (unsigned int *)malloc(sizeof(unsigned int));
    *iNew = readyId;
    Load *l = createLoad(iNew);
    l->thId = readyId;
    addLoad(wHeap, l);
    pthread_create(thList + readyId, &attr, func, l);
  }

  int i, minFreeLoadCount = minThreshold >> 1;
  if (readyId < n) {
    heapifyFromHead(wHeap);
    i = readyId;
    int chillThId = -1;
    Load *wHead = NULL;
    while (i < n) {
      printf("\033[32mwHeap: "); printHeap(wHeap, printLoad);
      printf("\n\033[33mrHeap: "); printHeap(restHeap, printLoad);
      printf("\ni:%d n:%d chillThId: %d\033[00m\n", i, n, chillThId);

      if ((wHead = peek(wHeap)) != NULL) {
        printf("wHead: %d\n", wHead->id);

        void *data = NULL;
        int join = pthread_join(thList[wHead->thId], &data);
        printf("newJoin: %d\n", join);
        if (! join) {
          printf("joined: %d\n", wHead->thId);
          if (data != NULL) {
            printf("\033[36m\nRetr %s :%d\033[00m\n", (char *)data, wHead->thId); 
            free(data);
          }
          chillThId = wHead->thId;
          printf("chillThId: %d\n", chillThId);
        #ifdef DEBUG
          printf("wHead->thId: %d\n", wHead->thId);
        #endif
          heapExtract(wHeap, (const void **)&wHead); 
          wHead->id = getAvailableId(restHeap);
          addLoad(restHeap, wHead); 
          printf("rHeap"); printHeap(restHeap, printLoad);
          wHead = NULL;
        }
      }

      if (getSize(wHeap) < minFreeLoadCount && peek(restHeap) != NULL) {
      #ifdef DEBUG
        printf("Peeked: %p\n", peek(restHeap));
        printf("\nrestHeap\n");
      #endif
        heapExtract(restHeap, (const void **)&wHead);
        if (wHead == NULL) continue;
      #ifdef DEBUG
        printf("wHead->thId:: %p\n", wHead);
      #endif
        wHead->thId = chillThId;
        *((int *)wHead->data) = i;

        addLoad(wHeap, wHead);
        int createStatus =\
          pthread_create(thList + wHead->thId, &attr, func, wHead);
        printf("createdStatus: %d i: %d\n", createStatus, i);
        if (! createStatus) {
          ++i;
        }
      }
    }
  }

  while (! isEmpty(wHeap)) {
    Load *tmpHead = NULL;
    if (! heapExtract(wHeap, (const void **)&tmpHead) && tmpHead != NULL) {
      void *data = NULL;
      if (! pthread_join(thList[tmpHead->thId], &data)) {
        if (data != NULL) {
          printf("i: %d Joined msg: %s\n", i, (char *)data);
          free(data);
        }
      }
    }
    freeLoad(tmpHead);
  }

  destroyHeap(wHeap);
  destroyHeap(restHeap);
}

void printLoad(void *l) {
  printf(" %d", l == NULL ? 0 : ((Load *)l)->id);
}

void registerLoad(Heap *loadHeap, Load *l) {
  if (l != NULL) { 
    unsigned int loadId = addLoad(loadHeap, l);
    l->id = loadId;
    l->srcHeap = loadHeap;
  }
}

int main() {
  Heap *loadH = NULL;
  loadH = initHeap(loadH, loadComp, freeLoad);

  int i;
  for (i=0; i < 10; ++i) {
    registerLoad(loadH, NULL);
  }

  produce(30, loadConsume);
  destroyHeap(loadH);
  return 0;
}
