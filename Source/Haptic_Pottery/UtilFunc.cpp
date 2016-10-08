



#include "UtilFunc.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


void Normalize(double vector[])
{
	double length;

	length = LENGTH3(vector[0], vector[1], vector[2]);
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}

void Normalize(GLfloat vector[])
{
	GLfloat length;

	length = LENGTH3(vector[0], vector[1], vector[2]);
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}

BOOL IsInRect(int x, int y, RECT *rect)
{
	if (rect->left <= x && rect->right >= x && rect->top <= y && rect->bottom >= y)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
