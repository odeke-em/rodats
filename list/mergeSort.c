#include <stdio.h>
#include <stdlib.h>
// #define EXHIBIT_MERGE

#include "list.h"
List *merge(Node **left, Node **right, const Comparator compare) {
  List *merged = NULL;
  void *tmp = NULL;
  Tag savTag = Unknown;

  while (*left != NULL && *right != NULL) {
    Comparison comp = compare(getData(*left), getData(*right)); 
    if (comp == Greater) goto addRight;

    addLeft: {
      savTag = getTag(*left);
      merged = appendAndTag(merged, pop(left), savTag);
      if (comp != Equal) continue;
    }

    addRight: {
      savTag = getTag(*right);
      merged = appendAndTag(merged, pop(right), savTag);
    }
  }

  while (*left != NULL) {
    savTag = getTag(*left);
    merged = appendAndTag(merged, pop(left), savTag);
  }

  while (*right != NULL) {
    savTag = getTag(*right);
    merged = appendAndTag(merged, pop(right), savTag);
  }

  return merged;
}

List *sort(List *l, Comparator compare) {
  unsigned int listSize = getListSize(l);
  if (listSize <= 2) {
    if	(listSize == 2 
    && compare(getData(l->head), getData(l->head->next)) == Greater) {
      swap(&(l->head->data), &(l->head->next->data));
    }
  } else {
    List *left = createNewList();
    int i = 0;

    Tag savTag = Unknown;
    while (i < listSize/2 && l->head != NULL) {
      savTag = getTag(l->head);
      left = appendAndTag(left, pop(&l->head), savTag);
      ++i;
    }

    List *right = createNewList();
    while (l->head != NULL) {
      savTag = getTag(l->head);
      right = appendAndTag(right, pop(&l->head), savTag);
      ++i;
    }

    List *lSorted = sort(left, compare),
	 *rSorted = sort(right, compare); 

    List *tmpL = NULL;
    if (lSorted != NULL && rSorted != NULL) {
      tmpL = merge(&lSorted->head, &rSorted->head, compare);
    }

    destroyList(l);
    destroyList(lSorted);
    destroyList(rSorted);
    l = tmpL;
  }

  return l;
}


int main() {
#ifdef EXHIBIT_MERGE
  int i;
  int left[] = { 12, 15, 18, 21, 24},
      right[] = {11, 13, 17, 20, 22 };

  int lSz = sizeof(left)/sizeof(left[0]);
  int rSz = sizeof(right)/sizeof(right[0]);

  List *L = NULL, *R = NULL;

  for (i=0; i < lSz; ++i) {
    int *lP = (int *)malloc(sizeof(int));
    *lP = left[i];
    L = append(L, lP);
  }

  for (i=0; i < rSz; ++i) {
    int *rP = (int *)malloc(sizeof(int));
    *rP = right[i];
    R = append(R, rP);
  }

  printList(L);
  printList(R);
  List *merged = merge(&L->head, &R->head, intPtrComp);
  printList(merged);
  destroyList(L);
  destroyList(R);
  destroyList(merged);
#else
  List *src = NULL;
  int index, values[] = {120, 23, -12, 10, 1450, 210, 1},
      vSize = sizeof(values)/sizeof(values[0]);

  for (index=0; index < vSize; ++index) {
    int *tp = (int *)malloc(sizeof(int));
    *tp = values[index];
    src = append(src, tp);
  }

  for (index=10; index < 1000000; ++index) {
    int *sav = (int *)malloc(sizeof(int));
    *sav = index;
    src = append(src, sav);
  }

  src = sort(src, intPtrComp);
  printList(src);
  destroyList(src);
#endif
  return 0;
}
