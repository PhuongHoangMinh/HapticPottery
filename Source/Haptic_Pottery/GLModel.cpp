#include "GLModel.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


GLModel::GLModel()
{
	SET3ARRAY(m_position, 0.0f, 0.0f, 0.0f)
	m_rotateX = 0.0f;
	m_rotateY = 0.0f;
	m_rotateZ = 0.0f;

	SET4ARRAY(m_color, 0.5f, 0.5f, 0.5f, 1.0f)

	SET4ARRAY(m_ambient, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_diffuse, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_specular, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_emission, 0.0f, 0.0f, 0.0f, 0.0f)
	m_shininess = 30;
}

GLModel::GLModel(GLfloat x, GLfloat y, GLfloat z)
{
	SET3ARRAY(m_position, x, y, z);
	m_rotateX = 0.0f;
	m_rotateY = 0.0f;
	m_rotateZ = 0.0f;

	SET4ARRAY(m_color, 0.5f, 0.5f, 0.5f, 1.0f)

	SET4ARRAY(m_ambient, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_diffuse, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_specular, 0.5f, 0.5f, 0.5f, 1.0f)
	SET4ARRAY(m_emission, 0.0f, 0.0f, 0.0f, 0.0f)
	m_shininess = 30;
}

void GLModel::Translate(GLfloat dx, GLfloat dy, GLfloat dz)
{
	m_position[0] += dx;
	m_position[1] += dy;
	m_position[2] += dz;
}

void GLModel::ApplyTransRot() const
{
	glTranslatef(m_position[0], m_position[1], m_position[2]);
	glRotatef(m_rotateX, 1.0f, 0.0f, 0.0f);
	glRotatef(m_rotateY, 0.0f, 1.0f, 0.0f);
	glRotatef(m_rotateZ, 0.0f, 0.0f, 1.0f);
}

void GLModel::ApplyColor() const
{
	glColor4fv(m_color);
}

void GLModel::ApplyMaterials() const
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, static_cast<GLfloat>(m_shininess));
}
