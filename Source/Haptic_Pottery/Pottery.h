#ifndef POTTERY_H__20071027__INCED
#define POTTERY_H__20071027__INCED

#include <stdio.h>
#include "GLModel.h"



#define GETIDX(vn, hn) ((vn) * m_pIProp.numHE + (hn))
#define GETHIDX_P(hn) (((hn) >= m_pIProp.numHE) ? (hn) - m_pIProp.numHE : (hn))
#define GETHIDX_M(hn) (((hn) < 0) ? (hn) + m_pIProp.numHE : (hn))
#define GETIDX_P(vn, hn) ((vn) * m_pIProp.numHE + (((hn) >= m_pIProp.numHE) ? (hn) - m_pIProp.numHE : (hn)))
#define GETIDX_M(vn, hn) ((vn) * m_pIProp.numHE + (((hn) < 0) ? (hn) + m_pIProp.numHE : (hn)))


#define PIPROP_NUM 8



typedef struct {
	double r;       
	double a;       
	double h;       
	double dLen;    
	GLfloat position[3];
	GLfloat normal1[3];
	GLfloat normal2[3];
} PE;


typedef struct {
	int numVE;           
	int numHE;           
	int numE;            
	double pResFac;      
	double lResFac;      
	double rFric;        
	double initVisco;    
	double maxVisco;     
	double minVisco;     
	double uVisco;       
	int rotCombo;        
	int rotPenalty;      
	int stopCombo;       
	int stopPenalty;     
} PIPROP;


class Pottery : public GLModel {
protected:
	PE *m_potteryElement;

	int m_pType;    

	double m_height;    

	double m_uHeight;    
	double m_uAngle;     

	double m_viscosity;    

	PIPROP m_pIProp;    

	
	GLfloat m_backAmbient[4];
	GLfloat m_backDiffuse[4];
	GLfloat m_backSpecular[4];
	GLfloat m_backEmission[4];
	int m_backShininess;

	
	void CalculatePosition(int vn, int hn);
	
	void CalculateNormal(int vn, int hn);

	
	void ItDeformTwo(int idx, int idx1, int idx2, double dLen);

	
	void CrossProductNormal(int v1, int v2, int v3, GLfloat *normal) const;

public:
	static const int m_defPType;         
	static const double m_defHeight;     
	static const double m_defRadius;     
	static const int m_maxHeight;        
	static const int m_minHeight;        
	static const double m_maxRadius;     
	static const double m_minRadius;     
	static const int m_maxInitRadius;    
	static const int m_minInitRadius;    
	static const double m_lVFactor;      
	static const double m_itFactor;      
	static const double m_itCutoff;      

	Pottery(int pType, double height, double radius);
	Pottery(FILE *fp);
	~Pottery();

	
	int GetPType() const { return m_pType; }
	double GetHeight() const { return m_height; };
	double GetUHeight() const { return m_uHeight; }
	double GetUAngle() const { return m_uAngle; }
	double GetViscosity() const { return m_viscosity; }
	double GetPResFac() const { return m_pIProp.pResFac; }
	double GetLResFac() const { return m_pIProp.lResFac; }
	double GetRotateFriction() const { return m_pIProp.rFric; }
	int GetNumVE() const { return m_pIProp.numVE; }
	int GetNumHE() const { return m_pIProp.numHE; }
	int GetRotCombo() const { return m_pIProp.rotCombo; }
	int GetRotPenalty() const { return m_pIProp.rotPenalty; }
	int GetStopCombo() const { return m_pIProp.stopCombo; }
	int GetStopPenalty() const { return m_pIProp.stopPenalty; }
	double GetElementRadius(int vn, int hn) const { return m_potteryElement[GETIDX(vn, hn)].r; }
	double GetElementAngle(int vn, int hn) const { return m_potteryElement[GETIDX(vn, hn)].a; }
	double GetElementHeight(int vn) const { return m_potteryElement[GETIDX(vn, 0)].h; }

	
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_ambient, r, g, b, a) SET4ARRAYFAC(m_backAmbient, m_ambient, 0.8f) }
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_diffuse, r, g, b, a) SET4ARRAYFAC(m_backDiffuse, m_diffuse, 0.8f) }
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_specular, r, g, b, a) SET4ARRAYFAC(m_backSpecular, m_specular, 0.8f) }
	void SetEmission(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { SET4ARRAY(m_emission, r, g, b, a) SET4ARRAYFAC(m_backEmission, m_emission, 0.8f) }
	void SetShininess(int s) { m_shininess = s; m_backShininess = static_cast<int>(m_shininess * 0.8);}

	
	void IncreaseViscosity();
	void DecreaseViscosity();

	
	void IncreaseShininess();
	void DecreaseShininess();

	
	static void GetPIProp(int pType, PIPROP *pIProp);

	
	void DeformElement(int vn, int hn, double dr, BOOL isLineCol);

	
	
	int DeformInteraction(int idx);

	
	void ResetPosNormal(int maxV, int minV, int maxH, int minH);

	
	void ApplyMaterials() const;

	
	virtual void DrawModel() const;
};


#endif    
