// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _hashTable_H
#define _hashTable_H

  #define INIT_HASH_LIST_SIZE 10201
  #define MAX_SAFETY_HTABLE_SIZE 1002001 // Arbitrary value

  typedef int hashValue;

  typedef enum {
    False=0, True=1
  } Bool;

  typedef struct MetaRankMule_ {
    int rank;
    int metaInfo;
    hashValue hash;
  } MetaRankMule;

  typedef struct Element_ {
    Bool dTag; // Discovery tag
    void *value;
    double rank;
    int metaInfo;
    struct Element_ *next;
  } Element;

  typedef struct {
    int size; 
    int capacity; 
    int averageElemLen;
    Bool allowCollisions;
    Element **list;
  } hashTable;

  // Returns True if the element's next entry is non-NULL
  inline Bool hasNext(Element *);

  // Next attribute accessor
  inline Element *getNext(Element *);
  inline int getSize(hashTable *hl);

  Element *initElement(Element *);
  hashTable *inithashTableWithSize(hashTable *hl, const int size);
  hashTable *inithashTable(hashTable *);

  Element *addToHead(Element *sl, void *data);

  // Adds the data as well as it's rank to the head 
  Element *addToHeadWithRank(Element *sl, void *data, const double rank);


  Element *addToTailWithMetaInfo(
    Element *sl, void *data, const int metaInfo
  );

  // Just invokes addToTailWithMetaInfo and passes metaInfo as '0'
  Element *addToTail(Element *sl, void *data); 

  Element **get(hashTable *hl, hashValue hashCode);
  Element *pop(hashTable *hM, const hashValue hashCode);

  void insertElem(hashTable *hl, void *data, const hashValue hashCode);

  // Returns the number of values freed
  long int destroySList(Element *sl);

  // Returns the number of values freed
  long int destroyhashTable(hashTable *hl);

  // Miscellaneous
  hashValue pjwCharHash(const char *srcW);
#endif
