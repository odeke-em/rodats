// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <stdio.h>
#include <stdlib.h>
#include "parcel.h"
  
inline Parcel *allocParcel() { 
  return (Parcel *)malloc(sizeof(Parcel)); 
}

Parcel *initParcel(Parcel *p) {
  if (p == NULL) {
    p = allocParcel();
  }

  p->cost = 0;
  p->data = NULL;
  p->dataAllocStyle = Clear;

  return p;
}

Parcel *destroyParcel(Parcel *p) {
  if (p != NULL) {
    if (p->dataAllocStyle == Set && p->data != NULL) {
      free(p->data);
    }

    free(p);
    p = NULL;
  }

  return p;
}

Parcel *createParcel(const double cost, void *data, Bit dAllocStyle) {
  Parcel *freshParcel = NULL;
  freshParcel = initParcel(freshParcel);
  freshParcel->cost = cost;
  freshParcel->data = data;
  freshParcel->dataAllocStyle = dAllocStyle;
}

Parcel *setDataCost(Parcel *p, const double cost, void *data, Bit dAllocStyle) {
  if (p != NULL) {
    p->cost = cost;
    p->data = data;
    p->dataAllocStyle = dAllocStyle;
  }
  return p;
}

void printParcel(const Parcel *p) {
  printf("[");
  if (p != NULL) {
    printf("\033[32m%.3f:%p\033[00m", p->cost, p->data);
  }
  printf("]");
}

#ifdef PARCEL_MAIN
int main() {
  Parcel parcels[] = {
    10, printParcel, Clear, 12, setDataCost, Clear,
    9, main, Clear
  };
  int i, pCount = sizeof(parcels)/sizeof(parcels[0]);

  for (i=0; i < pCount; ++i) {
    printParcel(&parcels[i]);
  }
  return 0;
}
#endif
