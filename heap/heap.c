// Author: Emmanuel Odeke <odeke@ualberta.ca> 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <pthread.h>

#include "heap.h"

static pthread_mutex_t heapMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t heapCondt = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t removeElemMutex = PTHREAD_MUTEX_INITIALIZER;

inline int getParent(int i) { return (i - 1)/2; }
inline int leftChild(int i) { return 2 * i + 1; }
inline int rightChild(int i) { return 2 * i + 2; }
inline void *peek(Heap *p) { return isEmpty(p) ? NULL : p->tree[0]; }

inline Heap *newHeap() {
  return (Heap *)malloc(sizeof(Heap));
}

inline void swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}

inline int getSize(Heap *h) {
  return (h == NULL || h->tree == NULL ? 0 : h->size);
}

inline int isEmpty(Heap *h)  {
  return (h == NULL || h->tree == NULL || h->size == 0);
}


Heap *initHeap(Heap *h, Comparator comp, Destructor destroy) {
  if (h == NULL) {
    h = newHeap();
  }

  h->size = 0;
  h->tree = NULL;
  h->compare = comp;
  h->destroy = destroy;

  return h;
}

Heap *heapify(Heap *h, const int targetIndex) {
  pthread_mutex_lock(&heapMutex);

  if (!(targetIndex >= 0 && targetIndex <= h->size))
    goto done;

  Comparator comp = h->compare;

  int endIndex;
  int parentIndex = targetIndex;
  
  while (parentIndex >= 0) {
    endIndex    = parentIndex;
    parentIndex = getParent(endIndex);

    if (comp(h->tree[endIndex], h->tree[parentIndex]) != Greater)
      break;
      
  #ifdef DEBUG
    printf("swapping\n");
  #endif
    
    swap(h->tree + endIndex, h->tree + parentIndex);
  }

done:
  pthread_mutex_unlock(&heapMutex);

  return h;
}

void **getAddrOf(Heap *h, void *elem) {
  void *addr = NULL;
  if (h != NULL && h->tree != NULL && h->compare != NULL) {
    int left=0, right=getSize(h) - 1;
    while (left <= right) {
      if (h->compare(elem, h->tree[left]) == Equal) {
        addr = h->tree + left; break;
      } else if  (h->compare(elem, h->tree[right]) == Equal) {
        addr = h->tree + right; break;
      } else {
        ++left, --right;
      }
    }
  }

  return addr;
}

void *removeElem(Heap *h, void *similarElem) {
  void *popdElem = NULL;
  pthread_mutex_lock(&removeElemMutex);

  if (h == NULL || h->tree == NULL || h->compare == NULL || h->size < 1)
    goto done;

  void **addrOfElem = getAddrOf(h, similarElem);
  if (addrOfElem != NULL) {
    popdElem = *addrOfElem;

    int unWantedIdx = addrOfElem - h->tree;
    void **newTree = (void **)malloc(sizeof(void *) * (h->size - 1));

    int travIdx, i;
    for (travIdx=0, i=0; travIdx < h->size; ++travIdx) {
      if (h->compare(similarElem, h->tree[travIdx]) != Equal) {
	newTree[i++] = h->tree[travIdx];
      }
    }

    --h->size;
    free(h->tree);

    h->tree = newTree;
    h = heapifyFromHead(h);
  }

done:
  pthread_mutex_unlock(&removeElemMutex);

  return popdElem;
}

int heapInsert(Heap *h, void *data) {
  if (h == NULL) {
    fprintf(stderr, "Can't insert into null heap\n");
    return -1;
  } else {
    int extraSize = h->size + 1;
    if (h->tree == NULL) {
      h->tree = (void **)malloc(sizeof(void *) * extraSize);
    } else {
      h->tree = (void **)realloc(h->tree, sizeof(void *) * extraSize);
    }

    if (h->tree == NULL) return -1;
    h->tree[h->size] = data;
    h = heapify(h, h->size);
    h->size = extraSize;
    return 0;
  }
}

Heap *heapifyFromHead(Heap *h) {
  // Push the contents of the new top downward
  int lPos, rPos, markedPos, curPos;

  pthread_mutex_lock(&heapMutex);
  printf("heapifyLock in\n");
  int size = getSize(h);
  Comparator comp = h->compare; 

  if (comp == NULL)
    goto done;

  for (curPos=0; curPos < size;){
    lPos = leftChild(curPos);
    rPos = rightChild(curPos);

    markedPos = curPos;
    if (lPos < size && comp(h->tree[lPos], h->tree[curPos]) == Greater) {
       markedPos = lPos;
    }

    // TODO: Mark this as to be visited and operate on it too, heapify(...)
    if (rPos < size && comp(h->tree[rPos], h->tree[markedPos]) == Greater) {
      markedPos = rPos;
    }

    // When the marked position is the current positon,
    // heap property has been restored
    if (markedPos == curPos)
      break;

    swap(h->tree + markedPos, h->tree + curPos);

    // Continue heapifying by moving another level down
    curPos = markedPos;
  }

done:
  // pthread_cond_broadcast(&heapCondt);
  pthread_mutex_unlock(&heapMutex);

  return h;
}

int heapExtract(Heap *h, const void **storage) {
  if (h == NULL || h->tree == NULL) {
    return -1;
  } else {
    void *headNode = h->tree[0];
    *storage = headNode; 
    int decrSz = h->size - 1;
    if (decrSz > 0) { // When there is more than one node present
      void *lastNode = h->tree[decrSz];
      h->tree = (void **)realloc(h->tree, sizeof(void *) * decrSz);
      h->tree[0] = lastNode;
      --h->size;
      // Push the contents of the new top downward
      h = heapifyFromHead(h);
    } else { // Last element
      h->size = 0;
      free(h->tree);
      h->tree = NULL;
    }

    return 0;
  }
}

Heap *destroyHeap(Heap *h) {
  pthread_mutex_lock(&heapMutex);

  if (h == NULL ||h->tree == NULL)
    goto done;
    
  if (h->destroy != NULL) {
    void **it = h->tree;
    void **end = it + h->size;

    while (it != end) {
      if (*it != NULL)
	h->destroy(*it);
    
      ++it;
    }
  }

  free(h->tree);

  h->size = 0;
  free(h);

done:
  pthread_mutex_unlock(&heapMutex);
  return h;
}

void intPtrPrint(void *it) {
  printf(" %d", it == NULL ? 0 : *(int *)it);
}

void printHeap(Heap *h, void (*iterPrint)(void *)) {
  printf("[ ");
  if (h != NULL && h->tree != NULL) {
    void **it = h->tree,
         **end = it + h->size;

    while (it != end) {
      iterPrint(*it++);
    }
  }
  printf("]\n");
}

Comparison intPtrComp(const void *a, const void *b) {
  if (a == NULL)
    return a == b ? Equal: Less;

  const int *aPtr = (int *)a;
  const int *bPtr = (int *)b;

  if (*aPtr != *bPtr) {
    return *aPtr < *bPtr ? Less : Greater;
  } else {
    return Equal;
  }
}

#ifdef SAMPLE_RUN
int main() {
  Heap *h = NULL;
  h = initHeap(h, intPtrComp, free);
  int i, *apt;

  for (i=0; i < 10000; ++i) {
    apt = (int *)malloc(sizeof(int));
    *apt = i;
    heapInsert(h, apt);
  }

  printHeap(h, intPtrPrint);
#ifdef DEMO_EXTRACTION
  while (! isEmpty(h)) {
    int *tp = NULL;
    heapExtract(h, (const void **)&tp);
    printf("tp: %d\n", *tp);
    free(tp);
  }
#endif

  destroyHeap(h);
  return 0;
}
#endif
