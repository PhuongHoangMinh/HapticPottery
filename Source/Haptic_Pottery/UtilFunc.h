#ifndef UTILFUNC_H__20071120__INCED
#define UTILFUNC_H__20071120__INCED

#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#define DTOR(a) ((a) * 0.017453292519943)     
#define RTOD(a) ((a) * 57.295779513082321)    

#define ISODD(a) ((a) & 0x00000001)    

#define COLORFTOI(f) (static_cast<int>(255.0 * (f)))       
#define COLORITOF(i) (static_cast<GLfloat>(i) / 255.0f)    

#define SQUARE(v) ((v) * (v))
#define LENGTH2(v1, v2) sqrt((v1) * (v1) + (v2) * (v2))
#define LENGTH3(v1, v2, v3) sqrt((v1) * (v1) + (v2) * (v2) + (v3) * (v3))
#define SQLENGTH2(v1, v2) ((v1) * (v1) + (v2) * (v2))
#define SQLENGTH3(v1, v2, v3) ((v1) * (v1) + (v2) * (v2) + (v3) * (v3))


#define SET4ARRAYFAC(an1, an2, f) an1[0] = an2[0] * (f); an1[1] = an2[1] * (f); an1[2] = an2[2] * (f); an1[3] = an2[3] * (f);


#define SET3ARRAY(an, a1, a2, a3) an[0] = a1; an[1] = a2; an[2] = a3;
#define SET4ARRAY(an, a1, a2, a3, a4) an[0] = a1; an[1] = a2; an[2] = a3; an[3] = a4;
#define SET3ATOA(an1, an2) an1[0] = an2[0]; an1[1] = an2[1]; an1[2] = an2[2];


void Normalize(double vector[]);
void Normalize(GLfloat vector[]);


BOOL IsInRect(int x, int y, RECT *rect);


#endif    
