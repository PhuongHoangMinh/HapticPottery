#ifndef POTTERYHIPMODEL_H__20071102__INCED
#define POTTERYHIPMODEL_H__20071102__INCED

#include <list>
#include "SphereHIP.h"
#include "Pottery.h"
#include "GLModelGeneral.h"
#include "Boundary.h"


#define ROTATE_STOP  0
#define ROTATE_RIGHT 1
#define ROTATE_LEFT  2
#define ROTATE_UP    3
#define ROTATE_DOWN  4



class PotteryHIPModel {
protected:
	BOOL m_isActive;       
	BOOL m_isOutDeform;    
	BOOL m_isProtected;    

	double m_topRadius;    

	hduVector3Dd m_force;    

	hduVector3Dd *m_avFilterForce;    

	double m_elecRotAV;    
	int m_handRotType;     

	double m_rotate[3];      
	double m_scaleFactor;    

	
	int m_combo;
	int m_penalty;

	
	int m_comboCount;
	int m_penaltyCount;

	
	double m_cosX;
	double m_cosY;
	double m_sinX;
	double m_sinY;

#ifdef INTERACTION
	Boundary *m_boundary;    
#endif    

	
	
	
	
	
	hduVector3Dd CollisionDetection(hduVector3Dd position, int *pointCNum, int *lineCNum, double *maxPointP, double *maxLineP, hduVector3Dd *drawPosition);
	
	
	int DeformBoundaryElement(std::list<int> &bList);

	
	hduVector3Dd RotationalForce(hduVector3Dd position, double maxP);
	
	hduVector3Dd AvFilterForce(hduVector3Dd force);
	
	hduVector3Dd lpFilterForce(hduVector3Dd force);

	
	void SetScore(int pointCNum, int lineCNum);

	
	void CalcTopRadius();
	
	void ChangeDeformDirMode(hduVector3Dd position);
	
	hduVector3Dd ProtectPotteryBase(hduVector3Dd position);

	
	void RotatePottery();

	
	hduVector3Dd RotateHIPToPottery(double x, double y, double z) const;
	
	hduVector3Dd RotatePotteryToHIP(double x, double y, double z) const;

	
	void GenerateSupport();

public:
	static const int m_avFilterLength;    

	static const double m_uElecRotAV;         
	static const double m_maxElecRotAV;       
	static const double m_handRotAngularV;    
	static const double m_uTriRotate;         

	static const double m_maxUDRotate;    
	static const double m_minUDRotate;    

	SphereHIP *m_sphereHIP;
	Pottery *m_pottery;
	GLModelGeneral *m_support;

	PotteryHIPModel(int pType, double height, double radius, double scaleFactor);
	PotteryHIPModel(FILE *fp, double scaleFactor);
	~PotteryHIPModel();

	
	BOOL GetIsActive() const { return m_isActive; }
	BOOL GetIsOutDeform() const { return m_isOutDeform; }
	double GetElecRotAV() const { return m_elecRotAV; }
	double GetRotateX() const { return m_rotate[0]; }
	double GetRotateY() const { return m_rotate[1]; }
	double GetRotateZ() const { return m_rotate[2]; }
	hduVector3Dd GetForce() const { return m_force; }
	int GetCombo() const { return m_combo; }
	int GetPenalty() const { return m_penalty; }
	double GetScaleFactor() const { return m_scaleFactor; }

	
	void SetHandRotType(int handRotType);

	
	void ToggleIsActive();

	
	void WheelSpeedUp();
	void WheelSpeedDown();

	
	void UpdatePosition(double x, double y, double z);

	
	void SavePottery(FILE *fp) const;
};


#endif    
