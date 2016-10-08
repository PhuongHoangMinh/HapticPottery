#include "GLModelGeneral.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


GLModelGeneral::GLModelGeneral(GLushort numVertices, GLushort numTriangles)
{
	m_numVertices = numVertices;
	m_numTriangles = numTriangles;

	m_vertices = new GLfloat[numVertices * 3];
	m_triangles = new GLushort[numTriangles * 3];
	m_normals = new GLfloat[numTriangles * 3];
}

GLModelGeneral::~GLModelGeneral()
{
	delete[] m_vertices;
	delete[] m_triangles;
	delete[] m_normals;
}

void GLModelGeneral::DrawModel() const
{
	int i;

    glPushMatrix();

	ApplyTransRot();

	ApplyColor();
	ApplyMaterials();

	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_numTriangles; i++)
	{
		glNormal3fv(&m_normals[i * 3]);
		glVertex3fv(&m_vertices[m_triangles[i * 3 + 0] * 3]);
		glVertex3fv(&m_vertices[m_triangles[i * 3 + 1] * 3]);
		glVertex3fv(&m_vertices[m_triangles[i * 3 + 2] * 3]);
	}
	glEnd();

	glPopMatrix();
}
