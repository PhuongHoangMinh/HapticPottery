#ifndef TRIFUNC_H__20071102__INCED
#define TRIFUNC_H__20071102__INCED


#define VALUETOIDX(sv, itv, v) static_cast<int>(((v) - (sv)) / (itv) + 0.5)



void SetCos(double startValue, double interval, int num, double **cosTable);


void SetSin(double startValue, double interval, int num, double **sinTable);


#endif    
