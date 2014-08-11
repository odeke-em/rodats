// Author: Emmanuel Odeke <odeke@ualberta.ca> 
#ifndef _HEAP_H
#define _HEAP_H
  typedef enum Comparison_ {
    Less=-1, Equal=0, Greater=1
  } Comparison;

  typedef void (*Destructor)(void *data);
  typedef Comparison (*Comparator)(const void *, const void *);

  typedef struct Heap_ {
    int size;
    void **tree;
    Comparator compare; 
    Destructor destroy;
  } Heap;

  #define getParent(i) (i - 1)/2
  #define leftChild(i) (2 * i + 1)
  #define rightChild(i) (2 * i + 2)
  #define isEmpty(h) (h == NULL || h->tree == NULL || h->size == 0)

  Heap *newHeap();
  Heap *initHeap(Heap *h, Comparator comp, Destructor destroy);

  int heapInsert(Heap *h, void *data);
  int heapExtract(Heap *h, const void **storage);
  Heap *heapify(Heap *h, const int targetIndex);
  Heap *heapifyFromHead(Heap *h);

  inline int getSize(Heap *h);

  Heap *heapSort(Heap *h);

  Heap *destroyHeap(Heap *h);

  // Find the address of the query element
  void **getAddrOf(Heap *h, void *query);

  // Pop method
  void *removeElem(Heap *h, void *similarElem);

  // Miscellaneous
  inline void *peek(Heap *p);
  inline void swap(void **a, void **b);
  void printHeap(Heap *h, void (*iterPrint)(void *));
#endif
