#ifndef SPHEREHIP_H__20071025__INCED
#define SPHEREHIP_H__20071025__INCED

#include <HDU/hduVector.h>
#include "GLModel.h"



class SphereHIP : public GLModel {
protected:
	GLfloat m_drawPosition[3];    
	double m_radius;

public:
	static const double m_initHIPRadius;
	static const double m_maxHIPRadius;
	static const double m_minHIPRadius;
	static const double m_uHIPRadius;    

	SphereHIP(GLfloat x, GLfloat y, GLfloat z);

	
	double GetRadius() const { return m_radius; }
	void GetPosition(double pos[]) const { pos[0] = static_cast<double>(m_position[0]); pos[1] = static_cast<double>(m_position[1]); pos[2] = static_cast<double>(m_position[2]); }

	
	void SetDrawPosition(GLfloat x, GLfloat y, GLfloat z) { SET3ARRAY(m_drawPosition, x, y, z) }

	
	void IncreaseHIPSize();
	void DecreaseHIPSize();

	
	virtual void DrawModel() const;
};


#endif    
