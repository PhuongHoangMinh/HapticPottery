



#include "UtilFunc.h"
#include "TriFunc.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


void SetCos(double startValue, double interval, int num, double **cosTable)
{
	int i;

	(*cosTable) = new double[num];

	for (i = 0; i < num; i++)
	{
		(*cosTable)[i] = cos(DTOR(static_cast<double>(i) * interval + startValue));
	}
}

void SetSin(double startValue, double interval, int num, double **sinTable)
{
	int i;

	(*sinTable) = new double[num];

	for (i = 0; i < num; i++)
	{
		(*sinTable)[i] = sin(DTOR(static_cast<double>(i) * interval + startValue));
	}
}
