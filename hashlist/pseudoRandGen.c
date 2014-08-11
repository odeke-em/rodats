#include <stdio.h>
#include <stdlib.h>
#include "hashList.h"

/*  
  Author: Emmanuel Odeke <odeke@ualberta.ca> 
  Theory for pseudo random generation obtained from: 
    http://www.math.utah.edu/~pa/Random/Random.html 
    Given p1, p2, N(limit), and x0 
    Recurrance formula: xN+1 = (x0*p1) + p2 (mod N) 
*/

Element *pGen(const int p1, const int p2, const int x0, const int limit) {
  Element *randValues = NULL;
  if (limit) {
    HashList *hl = NULL;
    hl = initHashList(hl);

    int xN1, xN = x0;
    while (1) {
      xN1 = ((p1 * xN) + p2) % limit;

      Element **query = get(hl, xN1);
      if (*query == NULL) {
	int *eMem = (int *)malloc(sizeof(int));
	int *rMem = (int *)malloc(sizeof(int));
	*eMem = xN1;
	*rMem = xN1;
	insertElem(hl, eMem, xN1);
	randValues = addToHead(randValues, rMem);
      } else { // Cycle or collision detected time to end
	break;
      }

      xN = xN1;
    }
    destroyHashList(hl);
  }

  return randValues;
}

void printIntSL(Element *sl) {
  printf("[");
  Element *trav = sl;
  while (trav != NULL) {
    printf("%d ", *(int *)trav->value);
    trav = trav->next;
  }
  printf("]");
}

int main() {
  Element *randValues = pGen(29, 69, 999, 1000000);

  printIntSL(randValues);
  destroySList(randValues);
  return 0;
}
