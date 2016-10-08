#ifndef GLMODEL_H__20071024__INCED
#define GLMODEL_H__20071024__INCED

#include <windows.h>
#include <gl/gl.h>
#include "UtilFunc.h"



class GLModel {
protected:
	GLfloat m_position[3];
	GLfloat m_rotateX;
	GLfloat m_rotateY;
	GLfloat m_rotateZ;

	GLfloat m_color[4];

	GLfloat m_ambient[4];
	GLfloat m_diffuse[4];
	GLfloat m_specular[4];
	GLfloat m_emission[4];
	int m_shininess;

public:
	GLModel();
	GLModel(GLfloat x, GLfloat y, GLfloat z);

	
	GLfloat GetRotateX() const { return m_rotateX; }
	GLfloat GetRotateY() const { return m_rotateY; }
	GLfloat GetRotateZ() const { return m_rotateZ; }
	void GetAmbient(double color[]) const { SET3ATOA(color, m_ambient) }
	void GetDiffuse(double color[]) const { SET3ATOA(color, m_diffuse) }
	void GetSpecular(double color[]) const { SET3ATOA(color, m_specular) }
	void GetEmission(double color[]) const { SET3ATOA(color, m_emission) }
	void GetAmbient(int color[]) const { color[0] = COLORFTOI(m_ambient[0]); color[1] = COLORFTOI(m_ambient[1]); color[2] = COLORFTOI(m_ambient[2]); }
	void GetDiffuse(int color[]) const { color[0] = COLORFTOI(m_diffuse[0]); color[1] = COLORFTOI(m_diffuse[1]); color[2] = COLORFTOI(m_diffuse[2]); }
	void GetSpecular(int color[]) const { color[0] = COLORFTOI(m_specular[0]); color[1] = COLORFTOI(m_specular[1]); color[2] = COLORFTOI(m_specular[2]); }
	void GetEmission(int color[]) const { color[0] = COLORFTOI(m_emission[0]); color[1] = COLORFTOI(m_emission[1]); color[2] = COLORFTOI(m_emission[2]); }
	COLORREF GetAmbient() const { return RGB(COLORFTOI(m_ambient[0]), COLORFTOI(m_ambient[1]), COLORFTOI(m_ambient[2])); }
	COLORREF GetDiffuse() const { return RGB(COLORFTOI(m_diffuse[0]), COLORFTOI(m_diffuse[1]), COLORFTOI(m_diffuse[2])); }
	COLORREF GetSpecular() const { return RGB(COLORFTOI(m_specular[0]), COLORFTOI(m_specular[1]), COLORFTOI(m_specular[2])); }
	COLORREF GetEmission() const { return RGB(COLORFTOI(m_emission[0]), COLORFTOI(m_emission[1]), COLORFTOI(m_emission[2])); }
	int GetShininess() const { return m_shininess; }

	
	void SetPosition(GLfloat x, GLfloat y, GLfloat z) { SET3ARRAY(m_position, x, y, x) }
	void SetRotateX(GLfloat angle) { m_rotateX = angle; }
	void SetRotateY(GLfloat angle) { m_rotateY = angle; }
	void SetRotateZ(GLfloat angle) { m_rotateZ = angle; }
	void SetColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_color, r, g, b, a) }
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_ambient, r, g, b, a) }
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_diffuse, r, g, b, a) }
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_specular, r, g, b, a) }
	void SetEmission(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_emission, r, g, b, a) }
	void SetShininess(int s) { m_shininess = s; }

	
	void RotateX(GLfloat da) { m_rotateX += da; }
	void RotateY(GLfloat da) { m_rotateY += da; }
	void RotateZ(GLfloat da) { m_rotateZ += da; }

	
	void Translate(GLfloat dx, GLfloat dy, GLfloat dz);

	
	void ApplyTransRot() const;

	
	void ApplyColor() const;
	
	void ApplyMaterials() const;

	
	virtual void DrawModel() const = 0;
};


#endif    
