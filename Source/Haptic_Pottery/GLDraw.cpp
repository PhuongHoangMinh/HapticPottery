



#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "PotteryHIPModel.h"
#include "GLDraw.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


GLfloat g_translate[3] = {0.0f, -22.0f, -100.0f};    

int g_graphicCallNum = 0;    

extern PotteryHIPModel *g_potteryHIP;


void ReshapeGL(int width, int height)
{
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 10.0f, 500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLInit()
{
	GLfloat light0Ambient[] = {0.33f, 0.33f, 0.33f, 1.0f};
	GLfloat light0Diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light0Specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light0Position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light0Direction[] = {0.0f, 0.0f, -1.0f};

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0Direction);

	glEnable(GL_LIGHT0);
}

void GLDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

	glTranslatef(g_translate[0], g_translate[1], g_translate[2]);

	
	g_potteryHIP->m_sphereHIP->DrawModel();

	
	glRotatef(static_cast<GLfloat>(g_potteryHIP->GetRotateX()), 1.0f, 0.0f, 0.0f);
	glRotatef(static_cast<GLfloat>(g_potteryHIP->GetRotateY()), 0.0f, 1.0f, 0.0f);
	glRotatef(static_cast<GLfloat>(g_potteryHIP->GetRotateZ()), 0.0f, 0.0f, 1.0f);

	
	g_potteryHIP->m_support->DrawModel();

	
	g_potteryHIP->m_pottery->DrawModel();

	glPopMatrix();

	g_graphicCallNum++;
}

void ZoomIn()
{
	if (g_translate[2] < -50.0)
	{
		g_translate[2] += 1.0f;
	}
}

void ZoomOut()
{
	if (g_translate[2] > -200.0)
	{
		g_translate[2] -= 1.0f;
	}
}
