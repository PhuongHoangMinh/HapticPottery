#include <stdio.h>
#include "UtilFunc.h"
#include "TriFunc.h"
#include "PotteryHIPModel.h"
#include "ControlWindow.h"

#ifdef TESTING
extern FILE *g_debugFp;
#endif    

#ifdef FORCE
extern FILE *g_maxPPoint;
extern FILE *g_maxPLine;
extern FILE *g_averPPoint;
extern FILE *g_averPLine;
extern FILE *g_forcePoint;
extern FILE *g_forceLine;
extern FILE *g_forceVis;
extern FILE *g_forceRot;
extern FILE *g_colPoint;
extern FILE *g_colLine;
extern FILE *g_totalForce;
extern FILE *g_filteredForce;
extern FILE *g_lFilteredForce;
#endif    



const int PotteryHIPModel::m_avFilterLength = 10;
const double PotteryHIPModel::m_uElecRotAV = 90.0;
const double PotteryHIPModel::m_maxElecRotAV = 360.0;
const double PotteryHIPModel::m_handRotAngularV = 45.0;
const double PotteryHIPModel::m_uTriRotate = 45.0 / 1000.0;
const double PotteryHIPModel::m_maxUDRotate = 80.0;
const double PotteryHIPModel::m_minUDRotate = -50.0;


double g_triRotSV;     
double g_triRotItv;    
double *g_triCosRot;
double *g_triSinRot;

extern double *g_triCosPotEven;
extern double *g_triSinPotEven;
extern double *g_triCosPotOdd;
extern double *g_triSinPotOdd;

extern hduVector3Dd g_iForce;
extern int g_iPointCNum;
extern int g_iLineCNum;


PotteryHIPModel::PotteryHIPModel(int pType, double height, double radius, double scaleFactor)
{
	m_sphereHIP = new SphereHIP(0.0, 0.0, 0.0);

	m_pottery = new Pottery(pType, height, radius);

	GenerateSupport();

#ifdef TESTING
	fprintf(g_debugFp, "scaleFactor : %f\n", scaleFactor);
#endif    

	m_isActive = FALSE;
	m_isOutDeform = TRUE;
	m_isProtected = FALSE;

	CalcTopRadius();

	m_avFilterForce = new hduVector3Dd[PotteryHIPModel::m_avFilterLength];

	m_elecRotAV = 0.0;
	m_handRotType = ROTATE_STOP;

	SET3ARRAY(m_rotate, 0.0, 0.0, 0.0)
	m_scaleFactor = scaleFactor;

	m_combo = 0;
	m_penalty = 0;
	m_comboCount = 1;
	m_penaltyCount = 1;

	m_cosX = 1.0;
	m_cosY = 1.0;
	m_sinX = 0.0;
	m_sinY = 0.0;

	g_triRotSV = PotteryHIPModel::m_uTriRotate - 180.0;
	g_triRotItv = PotteryHIPModel::m_uTriRotate;
	SetCos(g_triRotSV, g_triRotItv, static_cast<int>(360.0 / PotteryHIPModel::m_uTriRotate), &g_triCosRot);
	SetSin(g_triRotSV, g_triRotItv, static_cast<int>(360.0 / PotteryHIPModel::m_uTriRotate), &g_triSinRot);

#ifdef INTERACTION
	m_boundary = new Boundary(m_pottery->GetNumHE());
#endif    
}

PotteryHIPModel::PotteryHIPModel(FILE *fp, double scaleFactor)
{

	m_sphereHIP = new SphereHIP(0.0, 0.0, 0.0);

	m_pottery = new Pottery(fp);

	GenerateSupport();

	m_isActive = FALSE;
	m_isOutDeform = TRUE;
	m_isProtected = FALSE;

	CalcTopRadius();

	m_avFilterForce = new hduVector3Dd[PotteryHIPModel::m_avFilterLength];

	m_elecRotAV = 0.0;
	m_handRotType = ROTATE_STOP;

	m_rotate[0] = 0.0;
	m_rotate[1] = 0.0;
	m_rotate[2] = 0.0;
	m_scaleFactor = scaleFactor;

	fscanf(fp, "%d/%d/", &m_combo, &m_penalty);
	m_comboCount = 1;
	m_penaltyCount = 1;

	m_cosX = 1.0;
	m_cosY = 1.0;
	m_sinX = 0.0;
	m_sinY = 0.0;

	g_triRotSV = PotteryHIPModel::m_uTriRotate - 180.0;
	g_triRotItv = PotteryHIPModel::m_uTriRotate;
	SetCos(g_triRotSV, g_triRotItv, static_cast<int>(360.0 / PotteryHIPModel::m_uTriRotate), &g_triCosRot);
	SetSin(g_triRotSV, g_triRotItv, static_cast<int>(360.0 / PotteryHIPModel::m_uTriRotate), &g_triSinRot);

#ifdef INTERACTION
	m_boundary = new Boundary(m_pottery->GetNumHE());
#endif    
}

PotteryHIPModel::~PotteryHIPModel()
{
	delete m_sphereHIP;
	delete m_pottery;
	delete m_support;

	delete[] m_avFilterForce;

	delete[] g_triCosRot;
	delete[] g_triSinRot;

#ifdef INTERACTION
	delete m_boundary;
#endif    
}

void PotteryHIPModel::SetHandRotType(int handRotType)
{
	if (m_elecRotAV == 0.0 || handRotType == ROTATE_UP || handRotType == ROTATE_DOWN || handRotType == ROTATE_STOP)    
	{
		m_handRotType = handRotType;
	}
}

void PotteryHIPModel::ToggleIsActive()
{
	m_isActive = m_isActive ? FALSE : TRUE;
}

void PotteryHIPModel::WheelSpeedUp()
{
	m_handRotType = ROTATE_STOP;    

	m_elecRotAV += PotteryHIPModel::m_uElecRotAV;
	if (m_elecRotAV > PotteryHIPModel::m_maxElecRotAV)
	{
		m_elecRotAV = PotteryHIPModel::m_maxElecRotAV;
	}
}

void PotteryHIPModel::WheelSpeedDown()
{
	m_elecRotAV -= PotteryHIPModel::m_uElecRotAV;
	if (m_elecRotAV < 0.0)
	{
		m_elecRotAV = 0.0;
	}
}

void PotteryHIPModel::UpdatePosition(double x, double y, double z)
{
	hduVector3Dd convPosition;                 
	int pointCNum = 0, lineCNum = 0;           
	double maxPointP = 0.0, maxLineP = 0.0;    
	hduVector3Dd force;                        
	hduVector3Dd drawPosition;                 
	GLfloat scaledPosition[3] = {              
		static_cast<GLfloat>(m_scaleFactor * x), static_cast<GLfloat>(m_scaleFactor * y), static_cast<GLfloat>(m_scaleFactor * z)};
#ifdef FORCE
	hduVector3Dd vForce;     
	hduVector3Dd fForce;     
	hduVector3Dd lfForce;    
	hduVector3Dd avForce;    
#endif    

#ifdef FORCE
	
	static double robotHIP[3] = {0.0, 14.3, 39.0};
	static double dp = 0.0076;

	m_isActive = TRUE;

	if (robotHIP[2] < 32)
	{
		dp -= (robotHIP[2] - 22) * 0.0000008;
	}

	robotHIP[2] -= dp;
	x = robotHIP[0];
	y = robotHIP[1];
	z = robotHIP[2];

	scaledPosition[0] = static_cast<GLfloat>(m_scaleFactor * x);
	scaledPosition[1] = static_cast<GLfloat>(m_scaleFactor * y);
	scaledPosition[2] = static_cast<GLfloat>(m_scaleFactor * z);
#endif    

#ifdef SPEED
	
	static BOOL isY = FALSE;
	static BOOL isR = FALSE;
	static double robotHIP[3] = {0.0, 14.3, 39.0};
	static double dp = 0.0076;
	static double dp2 = 0.006;

	m_isActive = TRUE;

	if (!isY && dp < 0.0)
	{
		isY = TRUE;
	}

	if (isY && robotHIP[2] > 30)
	{
		isR = TRUE;
		WheelSpeedUp();
		WheelSpeedUp();
		WheelSpeedUp();
		WheelSpeedUp();
		dp = 0.0005;
		dp2 = 0.0027;
	}
	else if (isY && !isR)
	{
		dp -= 0.0000003;
	}
	else if (robotHIP[2] < 32 && !isR)
	{
		dp -= (robotHIP[2] - 22) * 0.0000008;
	}

	if (isR && robotHIP[1] > 54)
	{
		dp = -0.001;
		dp2 = 0.0;
	}

	robotHIP[2] -= dp;
	if (isY)
	{
		robotHIP[1] += dp2;
	}
	x = robotHIP[0];
	y = robotHIP[1];
	z = robotHIP[2];

	scaledPosition[0] = static_cast<GLfloat>(m_scaleFactor * x);
	scaledPosition[1] = static_cast<GLfloat>(m_scaleFactor * y);
	scaledPosition[2] = static_cast<GLfloat>(m_scaleFactor * z);
#endif    

	drawPosition.set(scaledPosition[0], scaledPosition[1], scaledPosition[2]);    

	m_sphereHIP->SetPosition(scaledPosition[0], scaledPosition[1], scaledPosition[2]);    

	RotatePottery();

	convPosition = m_scaleFactor * RotateHIPToPottery(x, y, z);

	ChangeDeformDirMode(convPosition);

	force = ProtectPotteryBase(convPosition);

	if (!(m_isProtected && m_isOutDeform))    
	{
#ifdef FORCE
		vForce = CollisionDetection(convPosition, &pointCNum, &lineCNum, &maxPointP, &maxLineP, &drawPosition);
#else
		force += CollisionDetection(convPosition, &pointCNum, &lineCNum, &maxPointP, &maxLineP, &drawPosition);
#endif    

		if (pointCNum + lineCNum > 0)    
		{
#ifdef FORCE
			force += vForce;

			fprintf(g_colPoint, "%d\n", pointCNum);
			fprintf(g_colLine, "%d\n", lineCNum);
			fprintf(g_forceVis, "%f\n", vForce.magnitude());

			fForce = RotationalForce(convPosition, maxPointP + maxLineP);

			fprintf(g_forceRot, "%f\n", fForce.magnitude());

			force += fForce;

			fprintf(g_totalForce, "%f\n", force.magnitude());
#else
			force += RotationalForce(convPosition, maxPointP + maxLineP);
#endif    
		}
	}

	
	m_sphereHIP->SetDrawPosition(static_cast<GLfloat>(drawPosition[0]), static_cast<GLfloat>(drawPosition[1]), static_cast<GLfloat>(drawPosition[2]));

#ifdef FORCE
	lfForce = lpFilterForce(force);
	avForce = AvFilterForce(force);

	force = lfForce;
#else
	force = lpFilterForce(force);
#endif    

#ifdef FORCE
	if (pointCNum + lineCNum > 0)
	{
		fprintf(g_filteredForce, "%f\n", avForce.magnitude(), avForce[0], avForce[1], avForce[2]);
		fprintf(g_lFilteredForce, "%f\n", lfForce.magnitude(), lfForce[0], lfForce[1], lfForce[2]);
	}
#endif    

	m_force = RotatePotteryToHIP(force[0], force[1], force[2]);

	SetScore(pointCNum, lineCNum);

	g_iForce = m_force;
	g_iPointCNum = pointCNum;
	g_iLineCNum = lineCNum;
}

void PotteryHIPModel::SavePottery(FILE *fp) const
{
	double temp[3];
	int i, j;

	fprintf(fp, "Virtual Pottery Data Format 20071201/");
	fprintf(fp, "%d/", m_pottery->GetPType());
	fprintf(fp, "%lf/", m_pottery->GetHeight());
	fprintf(fp, "%f/", m_pottery->GetViscosity());
	for (i = 0; i < m_pottery->GetNumVE(); i++)
	{
		for (j = 0; j < m_pottery->GetNumHE(); j++)
		{
			fprintf(fp, "(%f, %f, %f)/", m_pottery->GetElementRadius(i, j), m_pottery->GetElementAngle(i, j), m_pottery->GetElementHeight(i));
		}
	}

	m_pottery->GetAmbient(temp);
	fprintf(fp, "(%f, %f, %f)/", temp[0], temp[1], temp[2]);
	m_pottery->GetDiffuse(temp);
	fprintf(fp, "(%f, %f, %f)/", temp[0], temp[1], temp[2]);
	m_pottery->GetSpecular(temp);
	fprintf(fp, "(%f, %f, %f)/", temp[0], temp[1], temp[2]);
	m_pottery->GetEmission(temp);
	fprintf(fp, "(%f, %f, %f)/", temp[0], temp[1], temp[2]);
	fprintf(fp, "%d/", m_pottery->GetShininess());

	fprintf(fp, "%d/%d/", m_combo, m_penalty);
}

hduVector3Dd PotteryHIPModel::CollisionDetection(hduVector3Dd position, int *pointCNum, int *lineCNum, double *maxPointP, double *maxLineP, hduVector3Dd *drawPosition)
{
	int validVNMin, validVNMax;     
	int validHNMin, validHNMax;     
	double distHIP;                 
	double angleHIP, aWidthHIP;     
	double a, b, c, r;              
	double angle;                   
	double t;                       
	double e1[3], e2[3];            
	double p, dLen;                 
	hduVector3Dd lForce, pForce;    
	hduVector3Dd lForceDirec;       
	hduVector3Dd cDirec;            
	hduVector3Dd tForce;            
#ifdef TESTING
	int validENum;                                 
	LARGE_INTEGER sCounter, eCounter;              
#endif    
#ifdef FORCE
	hduVector3Dd lfForce;                          
	double totalPointP = 0.0, totalLineP = 0.0;    
#endif    
#ifdef INTERACTION
	std::list<int> bElements;    
	int maxStep;                 
#endif    
	int i, j;

#ifdef TESTING
	QueryPerformanceCounter(&sCounter);
#endif    

	distHIP = LENGTH2(position[0], position[2]);

	if (m_isOutDeform)
	{
		cDirec.set(position[0], 0.0, position[2]);
	}
	else
	{
		cDirec.set(-position[0], 0.0, -position[2]);
	}
	cDirec.normalize();

	angleHIP = RTOD(acos(position[0] / distHIP));
	if (position[2] < 0)
	{
		angleHIP = 360.0 - angleHIP;
	}

	aWidthHIP = RTOD(m_sphereHIP->GetRadius() / distHIP);
	if (aWidthHIP > 180.0)    
	{
		aWidthHIP = 180.0;
	}

	validVNMax = static_cast<int>((position[1] + m_sphereHIP->GetRadius()) / m_pottery->GetUHeight());
	validVNMin = static_cast<int>((position[1] - m_sphereHIP->GetRadius()) / m_pottery->GetUHeight()) + 1;
	if (validVNMax < 0 || validVNMin >= m_pottery->GetNumVE())
	{
		return tForce;
	}
	validVNMax = (validVNMax >= m_pottery->GetNumVE()) ? m_pottery->GetNumVE() - 1 : validVNMax;
	validVNMin = (validVNMin < 0) ? 0 : validVNMin;

	validHNMax = static_cast<int>((angleHIP + aWidthHIP) / m_pottery->GetUAngle());
	validHNMax = (validHNMax >= m_pottery->GetNumHE()) ? validHNMax - m_pottery->GetNumHE() : validHNMax;
	validHNMin = static_cast<int>((angleHIP - aWidthHIP) / m_pottery->GetUAngle()) + 1;
	validHNMin = (validHNMin < 0) ? validHNMin + m_pottery->GetNumHE() : validHNMin;

#ifdef INTERACTION
	m_boundary->InitVariables();
#endif    

	for (i = validVNMin; i <= validVNMax; i++)
	{
		c = SQUARE(position[0]) + SQUARE(position[2]) - SQUARE(m_sphereHIP->GetRadius()) + SQUARE(m_pottery->GetElementHeight(i) - position[1]);

		j = validHNMin - 1;
		do
		{
			j++;
			if (j >= m_pottery->GetNumHE())
			{
				j = 0;
			}

			e2[1] = e1[1] = m_pottery->GetElementHeight(i);
			angle = m_pottery->GetElementAngle(i, j);

			
			if ((angle > 60.0 && angle < 120.0) || (angle > 240.0 && angle < 300.0))    
			{
				if (!ISODD(i))    
				{
					t = g_triCosPotEven[j] / g_triSinPotEven[j];
				}
				else    
				{
					t = g_triCosPotOdd[j] / g_triSinPotOdd[j];
				}

				a = 1 + SQUARE(t);
				b = position[2] + t * position[0];

				r = SQUARE(b) - a * c;
				if (r < 0)    
				{
					continue;
				}
				r = sqrt(r);

				if (angle > 180 && angle < 360)
				{
					e1[2] = (b + r) / a;
					e2[2] = (b - r) / a;
				}
				else
				{
					e1[2] = (b - r) / a;
					e2[2] = (b + r) / a;
				}

				e1[0] = t * e1[2];
				e2[0] = t * e2[2];
			}
			else
			{
				if (!ISODD(i))    
				{
					t = g_triSinPotEven[j] / g_triCosPotEven[j];
				}
				else    
				{
					t = g_triSinPotOdd[j] / g_triCosPotOdd[j];
				}
				a = 1 + SQUARE(t);
				b = position[0] + t * position[2];

				r = SQUARE(b) - a * c;
				if (r < 0)    
				{
					continue;
				}
				r = sqrt(r);

				if (angle > 90 && angle < 270)
				{
					e1[0] = (b + r) / a;
					e2[0] = (b - r) / a;
				}
				else
				{
					e1[0] = (b - r) / a;
					e2[0] = (b + r) / a;
				}

				e1[2] = t * e1[0];
				e2[2] = t * e2[0];
			}

			if ((m_isOutDeform && distHIP <= m_pottery->GetElementRadius(i, j)) ||
				(!m_isOutDeform && distHIP >= m_pottery->GetElementRadius(i, j)))    
			{
				p = m_sphereHIP->GetRadius() - LENGTH3(position[0] - (e1[0] + e2[0]) / 2.0, position[1] - (e1[1] + e2[1]) / 2.0, position[2] - (e1[2] + e2[2]) / 2.0);
				if (m_isOutDeform)
				{
					dLen = m_pottery->GetElementRadius(i, j) - LENGTH2(e1[0], e1[2]);
				}
				else
				{
					dLen = m_pottery->GetElementRadius(i, j) - LENGTH2(e2[0], e2[2]);
				}

				
				if (m_isActive)
				{
					m_pottery->DeformElement(i, j, dLen, TRUE);

#ifdef INTERACTION
					m_boundary->ProcColliding(j);
#endif    
				}

				if (*maxLineP < p)
				{
					*maxLineP = p;
				}
#ifdef FORCE
				totalLineP += p;
#endif    

				lForce.set(2 * position[0] - e1[0] - e2[0], 2 * position[1] - e1[1] - e2[1], 2 * position[2] - e1[2] - e2[2]);
				lForceDirec += lForce;

				(*lineCNum)++;
			}
			else    
			{
				if (m_isOutDeform)
				{
					dLen = m_pottery->GetElementRadius(i, j) - LENGTH2(e1[0], e1[2]);
					p = dLen;
				}
				else
				{
					dLen = m_pottery->GetElementRadius(i, j) - LENGTH2(e2[0], e2[2]);
					p = -dLen;
				}

				if (p > 0)    
				{
					if (m_isActive)
					{
						m_pottery->DeformElement(i, j, dLen, FALSE);
#ifdef INTERACTION
						m_boundary->ProcColliding(j);
#endif    
					}

					if (*maxPointP < p)
					{
						*maxPointP = p;
					}

#ifdef FORCE
					totalPointP += p;
#endif    

					if (m_isOutDeform)
					{
						pForce.set(position[0] - e1[0], position[1] - e1[1], position[2] - e1[2]);
					}
					else
					{
						pForce.set(position[0] - e2[0], position[1] - e2[1], position[2] - e2[2]);
					}
					Normalize(pForce);
					pForce *= m_pottery->GetPResFac() * p;

					tForce += pForce;

					(*pointCNum)++;
				}
#ifdef INTERACTION
				else if (m_isActive)
				{
					m_boundary->ProcNColliding(j);
				}
#endif    
			}
		} while (j != validHNMax);

#ifdef INTERACTION
		m_boundary->ProcLineAndSave(i, validHNMax, bElements);
		m_boundary->ProcLineEnd();
#endif    
	}

#ifdef INTERACTION
	maxStep = DeformBoundaryElement(bElements);

	int vMax = validVNMax + maxStep;
	int vMin = validVNMin - maxStep - 1;
	int hMax = validHNMax + maxStep / 2 + 2;
	int hMin = validHNMin - maxStep / 2 - 2;
#else
	int vMax = validVNMax;
	int vMin = validVNMin - 1;
	int hMax = validHNMax + 1;
	int hMin = validHNMin - 1;
#endif    

	lForceDirec.normalize();

	if ((*pointCNum) > 0 || (*lineCNum) > 0)
	{
		m_pottery->ResetPosNormal((vMax >= m_pottery->GetNumVE()) ? m_pottery->GetNumVE() - 1 : vMax, (vMin < 0) ? 0 : vMin,
			(hMax >= m_pottery->GetNumHE()) ? hMax - m_pottery->GetNumHE() : hMax, (hMin < 0) ? hMin + m_pottery->GetNumHE() : hMin);

#ifdef FORCE
		fprintf(g_maxPPoint, "%f\n", *maxPointP);
		fprintf(g_maxPLine, "%f\n", *maxLineP);
		fprintf(g_averPPoint, "%f\n", totalPointP / static_cast<double>(*pointCNum));
		fprintf(g_averPLine, "%f\n", totalLineP / static_cast<double>(*lineCNum));

		fprintf(g_forcePoint, "%f\n", tForce.magnitude());
#endif    

		if (validVNMax == m_pottery->GetNumVE() - 1)
		{
			CalcTopRadius();
		}

		if ((*lineCNum) > 0)
		{
#ifdef FORCE
			lfForce = m_pottery->GetLResFac() * (*maxLineP) * lForceDirec;

			fprintf(g_forceLine, "%f\n", lfForce.magnitude());

			tForce += lfForce;
#else
			tForce += m_pottery->GetLResFac() * (*maxLineP) * lForceDirec;
#endif     
		}
#ifdef FORCE
		else
		{
			fprintf(g_forceLine, "%f\n", 0.0);
		}
#endif     
	}

	*drawPosition = position + (*maxPointP) * cDirec + (*maxLineP) * lForceDirec;
	*drawPosition = RotatePotteryToHIP((*drawPosition)[0], (*drawPosition)[1], (*drawPosition)[2]);

#ifdef TESTING
	QueryPerformanceCounter(&eCounter);

	if (angleHIP + aWidthHIP > 360.0 || angleHIP - aWidthHIP < 0.0)
	{
		validENum = (validHNMax + m_pottery->GetNumHE() - validHNMin) * (validVNMax - validVNMin);
	}
	else
	{
		validENum = (validHNMax - validHNMin) * (validVNMax - validVNMin);
	}

	if ((*pointCNum) > 0 || (*lineCNum) > 0)
	{
#ifdef INTERACTION
		fprintf(g_debugFp, "Max Step : %d, ", maxStep);
#endif    
		fprintf(g_debugFp, "Ran Num : %d, Col Num : %d(%d/%d), H : %u, L : %u\n",
			validENum, *pointCNum + *lineCNum, *pointCNum, *lineCNum, eCounter.HighPart - sCounter.HighPart, eCounter.LowPart - sCounter.LowPart);
	}
#endif    

	return tForce;
}

int PotteryHIPModel::DeformBoundaryElement(std::list<int> &bList)
{
	std::list<int>::iterator it;
	int maxStep = 0;    
	int step;

	for (it = bList.begin(); it != bList.end(); it++)
	{
		step = m_pottery->DeformInteraction(static_cast<int>(*it));

		if (step > maxStep)
		{
			maxStep = step;
		}
	}

	return maxStep;
}

hduVector3Dd PotteryHIPModel::RotationalForce(hduVector3Dd position, double maxP)
{
	hduVector3Dd force;

	if (m_handRotType == ROTATE_RIGHT || m_handRotType == ROTATE_LEFT || m_elecRotAV > 0.0)
	{
		force.set(position[2], 0, -position[0]);
		force.normalize();

		force *= m_pottery->GetRotateFriction() * maxP;

		if (m_handRotType == ROTATE_LEFT)
		{
			force = -force;
		}
	}

	return force;
}

hduVector3Dd PotteryHIPModel::AvFilterForce(hduVector3Dd force)
{
	hduVector3Dd tForce;
	int i;

	if (force.isZero(0.0000001) || m_avFilterForce[0].isZero(0.0000001))
	{
		for (i = 0; i < PotteryHIPModel::m_avFilterLength - 1; i++)
		{
			m_avFilterForce[i] = force;
			tForce += m_avFilterForce[i];
		}
	}
	else
	{
		for (i = 0; i < PotteryHIPModel::m_avFilterLength - 1; i++)
		{
			m_avFilterForce[i] = m_avFilterForce[i + 1];
			tForce += m_avFilterForce[i];
		}
	}
	m_avFilterForce[PotteryHIPModel::m_avFilterLength - 1] = force;
	tForce += m_avFilterForce[PotteryHIPModel::m_avFilterLength - 1];

	return tForce / static_cast<double>(PotteryHIPModel::m_avFilterLength);
}

hduVector3Dd PotteryHIPModel::lpFilterForce(hduVector3Dd force)
{
	static hduVector3Dd xv[2], yv[2];

	xv[0] = xv[1];
	xv[1] = force / 3.282051595e+01;
	yv[0] = yv[1];
	yv[1] = (xv[0] + xv[1]) + (0.9390625058 * yv[0]);
    
	return yv[1];



}

void PotteryHIPModel::SetScore(int pointCNum, int lineCNum)
{
	if (!m_isActive || (pointCNum == 0 && lineCNum == 0))    
	{
		m_comboCount = 1;
		m_penaltyCount = 1;
		OnOffComboAni(FALSE);
		OnOffPenaltyAni(FALSE);

		return;
	}

	if (m_handRotType != ROTATE_STOP || m_elecRotAV > 0.0)    
	{
		if (lineCNum <= m_pottery->GetRotCombo())
		{
			m_comboCount++;
		}
		else
		{
			m_comboCount = 1;
			OnOffComboAni(FALSE);
		}

		if (lineCNum >= m_pottery->GetRotPenalty())
		{
			m_penaltyCount++;
		}
		else
		{
			m_penaltyCount = 1;
			OnOffPenaltyAni(FALSE);
		}
	}
	else    
	{
		if (lineCNum <= m_pottery->GetStopCombo())
		{
			m_comboCount++;
		}
		else
		{
			m_comboCount = 1;
			OnOffComboAni(FALSE);
		}

		if (lineCNum >= m_pottery->GetStopPenalty())
		{
			m_penaltyCount++;
		}
		else
		{
			m_penaltyCount = 1;
			OnOffPenaltyAni(FALSE);
		}
	}

	if (m_comboCount % 2000 == 0)    
	{
		m_combo += m_comboCount / 2000;

		OnOffComboAni(TRUE);
	}
	else if (m_penaltyCount % 200 == 2)    
	{
		if (m_handRotType != ROTATE_STOP || m_elecRotAV > 0.0)    
		{
			m_penalty += 10;
		}
		else
		{
			m_penalty += 1;
		}

		OnOffPenaltyAni(TRUE);
	}
}

void PotteryHIPModel::CalcTopRadius()
{
	int i;

	m_topRadius = m_pottery->GetElementRadius(m_pottery->GetNumVE() - 1, 0);
	for (i = 1; i < m_pottery->GetNumHE(); i++)
	{
		if (m_pottery->GetElementRadius(m_pottery->GetNumVE() - 1, i) < m_topRadius)
		{
			m_topRadius = m_pottery->GetElementRadius(m_pottery->GetNumVE() - 1, i);
		}
	}
}

void PotteryHIPModel::ChangeDeformDirMode(hduVector3Dd position)
{
	static double pPosition[3] = {position[0], position[1], position[2]};

	if (position[1] - m_sphereHIP->GetRadius() < m_pottery->GetHeight() && pPosition[1] - m_sphereHIP->GetRadius() > m_pottery->GetHeight() && SQLENGTH2(position[0], position[2]) < SQUARE(m_topRadius))
	{
		m_isOutDeform = FALSE;
	}
	else if (!m_isOutDeform && position[1] - m_sphereHIP->GetRadius() > m_pottery->GetHeight())
	{
		m_isOutDeform = TRUE;
	}

	pPosition[0] = position[0];
	pPosition[1] = position[1];
	pPosition[2] = position[2];
}

hduVector3Dd PotteryHIPModel::ProtectPotteryBase(hduVector3Dd position)
{
	static double pPosition[3] = {position[0], position[1], position[2]};
	hduVector3Dd force;

	if (m_isOutDeform)
	{
		if (position[1] + m_sphereHIP->GetRadius() > 0 && pPosition[1] + m_sphereHIP->GetRadius() < 0 && SQLENGTH2(position[0], position[2]) <= 400.0)
		{
			m_isProtected = TRUE;
		}
		else if (m_isProtected && (position[1] + m_sphereHIP->GetRadius() < 0 || SQLENGTH2(position[0], position[2]) > 400.0))
		{
			m_isProtected = FALSE;
		}

		if (m_isProtected)
		{
			force.set(0.0, -1.0, 0.0);
			force *= position[1] + m_sphereHIP->GetRadius();
		}
	}
	else
	{
		if (position[1] - m_sphereHIP->GetRadius() < 0 && pPosition[1] - m_sphereHIP->GetRadius() > 0)
		{
			m_isProtected = TRUE;
		}
		else if (m_isProtected && position[1] - m_sphereHIP->GetRadius() > 0)
		{
			m_isProtected = FALSE;
		}

		if (m_isProtected)
		{
			force.set(0.0, 1.0, 0.0);
			force *= m_sphereHIP->GetRadius() - position[1];
		}
	}

	pPosition[0] = position[0];
	pPosition[1] = position[1];
	pPosition[2] = position[2];

	return force;
}

void PotteryHIPModel::RotatePottery()
{
	switch (m_handRotType)
	{
	case ROTATE_RIGHT:
		m_rotate[1] += PotteryHIPModel::m_handRotAngularV / 1000.0;

		break;
	case ROTATE_LEFT:
		m_rotate[1] -= PotteryHIPModel::m_handRotAngularV / 1000.0;

		break;
	case ROTATE_UP:
		m_rotate[0] -= PotteryHIPModel::m_handRotAngularV / 1000.0;

		break;
	case ROTATE_DOWN:
		m_rotate[0] += PotteryHIPModel::m_handRotAngularV / 1000.0;

		break;
	case ROTATE_STOP:
		if (m_elecRotAV == 0.0)    
		{
			return;
		}

		break;
	}

	m_rotate[1] += m_elecRotAV / 1000.0;

	if (m_rotate[1] > 180.0)
	{
		m_rotate[1] -= 360.0;
	}
	else if (m_rotate[1] <= -180.0)
	{
		m_rotate[1] += 360.0;
	}

	if (m_rotate[0] > PotteryHIPModel::m_maxUDRotate)
	{
		m_rotate[0] = PotteryHIPModel::m_maxUDRotate;
	}
	else if (m_rotate[0] < PotteryHIPModel::m_minUDRotate)
	{
		m_rotate[0] = PotteryHIPModel::m_minUDRotate;
	}

	m_cosY = g_triCosRot[VALUETOIDX(g_triRotSV, g_triRotItv, m_rotate[1])];
	m_sinY = g_triSinRot[VALUETOIDX(g_triRotSV, g_triRotItv, m_rotate[1])];
	m_cosX = g_triCosRot[VALUETOIDX(g_triRotSV, g_triRotItv, m_rotate[0])];
	m_sinX = g_triSinRot[VALUETOIDX(g_triRotSV, g_triRotItv, m_rotate[0])];
}

hduVector3Dd PotteryHIPModel::RotateHIPToPottery(double x, double y, double z) const
{
	hduVector3Dd cVec;

	cVec.set(m_cosY * x + m_sinX * m_sinY * y - m_cosX * m_sinY * z,
		m_cosX * y + m_sinX * z,
		m_sinY * x - m_sinX * m_cosY * y + m_cosX * m_cosY * z);

	return cVec;
}

hduVector3Dd PotteryHIPModel::RotatePotteryToHIP(double x, double y, double z) const
{
	hduVector3Dd cVec;

	cVec.set(m_cosY * x + m_sinY * z,
		m_sinX * m_sinY * x + m_cosX * y - m_sinX * m_cosY * z,
		m_sinX * y - m_cosX * m_sinY * x + m_cosX * m_cosY * z);

	return cVec;
}

void PotteryHIPModel::GenerateSupport()
{
	m_support = new GLModelGeneral(8, 12);
	m_support->SetVertices(0, -20.0f, -5.0f, 20.0f);
	m_support->SetVertices(1, 20.0f, -5.0f, 20.0f);
	m_support->SetVertices(2, 20.0f, 0.0f, 20.0f);
	m_support->SetVertices(3, -20.0f, 0.0f, 20.0f);
	m_support->SetVertices(4, -20.0f, -5.0f, -20.0f);
	m_support->SetVertices(5, 20.0f, -5.0f, -20.0f);
	m_support->SetVertices(6, 20.0f, 0.0f, -20.0f);
	m_support->SetVertices(7, -20.0f, 0.0f, -20.0f);
	m_support->SetTriangles(0, 0, 1, 3);
	m_support->SetTriangles(1, 1, 2, 3);
	m_support->SetTriangles(2, 1, 5, 2);
	m_support->SetTriangles(3, 5, 6, 2);
	m_support->SetTriangles(4, 5, 4, 6);
	m_support->SetTriangles(5, 4, 7, 6);
	m_support->SetTriangles(6, 4, 0, 7);
	m_support->SetTriangles(7, 0, 3, 7);
	m_support->SetTriangles(8, 0, 1, 4);
	m_support->SetTriangles(9, 1, 5, 4);
	m_support->SetTriangles(10, 3, 2, 7);
	m_support->SetTriangles(11, 2, 6, 7);
	m_support->SetNormals(0, 0.0f, 0.0f, 1.0f);
	m_support->SetNormals(1, 0.0f, 0.0f, 1.0f);
	m_support->SetNormals(2, 1.0f, 0.0f, 0.0f);
	m_support->SetNormals(3, 1.0f, 0.0f, 0.0f);
	m_support->SetNormals(4, 0.0f, 0.0f, -1.0f);
	m_support->SetNormals(5, 0.0f, 0.0f, -1.0f);
	m_support->SetNormals(6, -1.0f, 0.0f, 0.0f);
	m_support->SetNormals(7, -1.0f, 0.0f, 0.0f);
	m_support->SetNormals(8, 0.0f, 1.0f, 0.0f);
	m_support->SetNormals(9, 0.0f, 1.0f, 0.0f);
	m_support->SetNormals(10, 0.0f, -1.0f, 0.0f);
	m_support->SetNormals(11, 0.0f, -1.0f, 0.0f);
	m_support->SetPosition(0.0f, 0.0f, 0.0f);
	m_support->SetRotateX(0.0f);
	m_support->SetRotateY(0.0f);
	m_support->SetRotateZ(0.0f);
	m_support->SetAmbient(0.2f, 0.6f, 0.4f, 1.0f);
	m_support->SetDiffuse(0.5f, 0.5f, 0.5f, 1.0f);
	m_support->SetSpecular(0.0f, 0.0f, 0.0f, 1.0f);
	m_support->SetEmission(0.0f, 0.0f, 0.0f, 1.0f);
	m_support->SetShininess(0);
}
