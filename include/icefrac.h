#ifndef ICEFRAC_H
#define ICEFRAC_H

typedef struct icefrac *Icefrac;

extern Icefrac initicefrac(const Config *);
extern void freeicefrac(Icefrac, Bool);
extern void interpolate_icefrac(Icefrac, int, Real);
extern Real geticefrac(const Icefrac, int);
extern Bool readicefrac(Icefrac, const Cell *, int, int, const Config *);

#endif
