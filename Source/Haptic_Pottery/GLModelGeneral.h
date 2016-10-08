#ifndef GLMODELGENERAL_H__20071024__INCED
#define GLMODELGENERAL_H__20071024__INCED

#include "GLModel.h"



class GLModelGeneral : public GLModel {
protected:
	GLushort m_numVertices;
	GLushort m_numTriangles;

	GLfloat *m_vertices;
	GLushort *m_triangles;
	GLfloat *m_normals;

public:
	GLModelGeneral(GLushort numVertices, GLushort numTriangles);
	virtual ~GLModelGeneral();

	
	void SetVertices(GLushort i, GLfloat x, GLfloat y, GLfloat z) { m_vertices[i * 3 + 0] = x; m_vertices[i * 3 + 1] = y; m_vertices[i * 3 + 2] = z; }
	void SetTriangles(GLushort i, GLushort v1, GLushort v2, GLushort v3) { m_triangles[i * 3 + 0] = v1; m_triangles[i * 3 + 1] = v2; m_triangles[i * 3 + 2] = v3; }
	void SetNormals(GLushort i, GLfloat x, GLfloat y, GLfloat z) { m_normals[i * 3 + 0] = x; m_normals[i * 3 + 1] = y; m_normals[i * 3 + 2] = z; }

	
	virtual void DrawModel() const;
};


#endif    
