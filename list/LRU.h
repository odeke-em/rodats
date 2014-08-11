#ifndef _LRU_H
#define _LRU_H
#ifndef DEBUG
  #define DEBUG
#endif // DEBUG
  #include "list.h"

  #define Cache LRU // Achieve some type name aliasing
  typedef List Cache;

  // Save any elements that will be purged from the main cache
  // into purgedSav
  Cache *purgeAndSave(Cache *c, Cache **purgedSav);

  // Merely invokes purgeAndSave but with a NULL argument for purgedSav 
  Cache *purgeLRU(Cache *c);
 
  Cache *setTagValue(Cache *c, const unsigned int tagValue);

  void *lookUpEntry(Cache *c, const void *key, Comparator comp);
#endif // _LRU_H
