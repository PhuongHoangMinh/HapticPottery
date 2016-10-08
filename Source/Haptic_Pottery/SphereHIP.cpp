#include <windows.h>
#include <gl/glu.h>
#include "SphereHIP.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


const double SphereHIP::m_initHIPRadius = 3.5;
const double SphereHIP::m_maxHIPRadius = 4.0;
const double SphereHIP::m_minHIPRadius = 3.0;
const double SphereHIP::m_uHIPRadius = 0.1;


SphereHIP::SphereHIP(GLfloat x, GLfloat y, GLfloat z)
	: GLModel(x, y, z)
{
	SET3ARRAY(m_drawPosition, x, y, z)
	m_radius = SphereHIP::m_initHIPRadius;

	SetAmbient(0.6f, 0.4f, 0.4f, 1.0f);
	SetDiffuse(0.5f, 0.2f, 0.2f, 1.0f);
	SetSpecular(0.1f, 0.0f, 0.0f, 1.0f);
	SetEmission(0.0f, 0.0f, 0.0f, 1.0f);
	SetShininess(0);
}

void SphereHIP::IncreaseHIPSize()
{
	if (m_radius < SphereHIP::m_maxHIPRadius)
	{
		m_radius += SphereHIP::m_uHIPRadius;
	}
}

void SphereHIP::DecreaseHIPSize()
{
	if (m_radius > SphereHIP::m_minHIPRadius)
	{
		m_radius -= SphereHIP::m_uHIPRadius;
	}
}

void SphereHIP::DrawModel() const
{
	GLUquadricObj* quadObj = gluNewQuadric();

	glPushMatrix();

	glTranslatef(m_drawPosition[0], m_drawPosition[1], m_drawPosition[2]);

	ApplyColor();
	ApplyMaterials();

	gluSphere(quadObj, m_radius, 20, 20);

	glPopMatrix();

	gluDeleteQuadric(quadObj);
}
