// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _PARCEL_H
#define _PARCEL_H
  typedef enum Bit_ {
    Clear=0, Set=1
  } Bit;

  typedef struct Parcel_ {
    double cost;
    void *data;
    Bit dataAllocStyle;	// Clear to indicate static allocation
			// Set to indicate dynamic allocation
  } Parcel;

  inline Parcel *allocParcel();
  Parcel *initParcel(Parcel *);
  Parcel *destroyParcel(Parcel *);

  Parcel *setDataCost(Parcel *p, const double cost, void *data, Bit dataAllocStyle);
  Parcel *createParcel(const double cost, void *data, Bit dataAllocStyle);
  void printParcel(const Parcel *p);
#endif
