#include "UtilFunc.h"
#include "TriFunc.h"
#include "Boundary.h"
#include "Pottery.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    



const int Pottery::m_defPType = 5;
const double Pottery::m_defHeight = 50.0;
const double Pottery::m_defRadius = 20.0;
const int Pottery::m_maxHeight = 50;
const int Pottery::m_minHeight = 20;
const double Pottery::m_maxRadius = 30.0;
const double Pottery::m_minRadius = 3.0;
const int Pottery::m_maxInitRadius = 20;
const int Pottery::m_minInitRadius = 10;
const double Pottery::m_lVFactor = 0.5;
const double Pottery::m_itFactor = 3.5;
const double Pottery::m_itCutoff = 0.0002;


double *g_triCosPotEven;
double *g_triSinPotEven;
double *g_triCosPotOdd;
double *g_triSinPotOdd;


Pottery::Pottery(int pType, double height, double radius)
{
	int i, j;
	double h;

	m_pType = pType;

	m_height = height;

	Pottery::GetPIProp(m_pType, &m_pIProp);

	m_potteryElement = new PE[m_pIProp.numVE * m_pIProp.numHE]; //Dynamic memory allocation

	m_uHeight = m_height / static_cast<double>(m_pIProp.numVE - 1);
	m_uAngle = 360.0 / static_cast<double>(m_pIProp.numHE);

#ifdef TESTING
	fprintf(g_debugFp, "uHeight : %f, uAngle : %f\n", m_uHeight, m_uAngle);
#endif    

	SetCos(0.0, m_uAngle, m_pIProp.numHE, &g_triCosPotEven);
	SetSin(0.0, m_uAngle, m_pIProp.numHE, &g_triSinPotEven);
	SetCos(m_uAngle / 2.0, static_cast<double>(m_uAngle), m_pIProp.numHE, &g_triCosPotOdd);
	SetSin(m_uAngle / 2.0, static_cast<double>(m_uAngle), m_pIProp.numHE, &g_triSinPotOdd);

	m_viscosity = m_pIProp.initVisco;

	for (i = 0; i < m_pIProp.numVE; i++)
	{
		h = static_cast<double>(i) * m_uHeight;

		for (j = 0; j < m_pIProp.numHE; j++)
		{
			m_potteryElement[GETIDX(i, j)].r = radius;
			m_potteryElement[GETIDX(i, j)].h = h;

			if (!ISODD(i))    
			{
				m_potteryElement[GETIDX(i, j)].a = static_cast<double>(j) * m_uAngle;
			}
			else    
			{
				m_potteryElement[GETIDX(i, j)].a = static_cast<double>(j) * m_uAngle + m_uAngle / 2.0f;
			}

			CalculatePosition(i, j);
		}
	}

	for (i = 0; i < m_pIProp.numVE; i++)
	{
		for (j = 0; j < m_pIProp.numHE; j++)
		{
			CalculateNormal(i, j);
		}
	}

	SetAmbient(0.51f, 0.59f, 0.51f, 1.0f);
	SetDiffuse(0.5f, 0.5f, 0.5f, 1.0f);
	SetSpecular(0.05f, 0.05f, 0.05f, 1.0f);
	SetEmission(0.0f, 0.0f, 0.0f, 1.0f);
	SetShininess(30);
}

Pottery::Pottery(FILE *fp)
{
	double temp[3];
	int ti;
	int i, j;

	fseek(fp, 37, SEEK_SET);

	fscanf(fp, "%d/", &m_pType);

	fscanf(fp, "%lf/", &m_height);

	Pottery::GetPIProp(m_pType, &m_pIProp);

	m_potteryElement = new PE[m_pIProp.numVE * m_pIProp.numHE];

	m_uHeight = m_height / static_cast<double>(m_pIProp.numVE - 1);
	m_uAngle = 360.0 / static_cast<double>(m_pIProp.numHE);

	SetCos(0.0, m_uAngle, m_pIProp.numHE, &g_triCosPotEven);
	SetSin(0.0, m_uAngle, m_pIProp.numHE, &g_triSinPotEven);
	SetCos(m_uAngle / 2.0, static_cast<double>(m_uAngle), m_pIProp.numHE, &g_triCosPotOdd);
	SetSin(m_uAngle / 2.0, static_cast<double>(m_uAngle), m_pIProp.numHE, &g_triSinPotOdd);

	fscanf(fp, "%lf/", &m_viscosity);

	for (i = 0; i < m_pIProp.numVE; i++)
	{
		for (j = 0; j < m_pIProp.numHE; j++)
		{
			fscanf(fp, "(%lf, %lf, %lf)/", &m_potteryElement[GETIDX(i, j)].r, &m_potteryElement[GETIDX(i, j)].a, &m_potteryElement[GETIDX(i, j)].h);

			CalculatePosition(i, j);
		}
	}

	for (i = 0; i < m_pIProp.numVE; i++)
	{
		for (j = 0; j < m_pIProp.numHE; j++)
		{
			CalculateNormal(i, j);
		}
	}

	fscanf(fp, "(%lf, %lf, %lf)/", &temp[0], &temp[1], &temp[2]);
	SetAmbient(static_cast<GLfloat>(temp[0]), static_cast<GLfloat>(temp[1]), static_cast<GLfloat>(temp[2]), 1.0f);
	fscanf(fp, "(%lf, %lf, %lf)/", &temp[0], &temp[1], &temp[2]);
	SetDiffuse(static_cast<GLfloat>(temp[0]), static_cast<GLfloat>(temp[1]), static_cast<GLfloat>(temp[2]), 1.0f);
	fscanf(fp, "(%lf, %lf, %lf)/", &temp[0], &temp[1], &temp[2]);
	SetSpecular(static_cast<GLfloat>(temp[0]), static_cast<GLfloat>(temp[1]), static_cast<GLfloat>(temp[2]), 1.0f);
	fscanf(fp, "(%lf, %lf, %lf)/", &temp[0], &temp[1], &temp[2]);
	SetEmission(static_cast<GLfloat>(temp[0]), static_cast<GLfloat>(temp[1]), static_cast<GLfloat>(temp[2]), 1.0f);
	fscanf(fp, "%d/", &ti);
	SetShininess(ti);
}

Pottery::~Pottery()
{
	delete[] m_potteryElement;

	delete[] g_triCosPotEven;
	delete[] g_triSinPotEven;
	delete[] g_triCosPotOdd;
	delete[] g_triSinPotOdd;
}

void Pottery::IncreaseViscosity()
{
	if (m_viscosity < m_pIProp.maxVisco)
	{
		m_viscosity += m_pIProp.uVisco;
	}
}

void Pottery::DecreaseViscosity()
{
	if (m_viscosity > m_pIProp.minVisco)
	{
		m_viscosity -= m_pIProp.uVisco;
	}
}

void Pottery::IncreaseShininess()
{
	if (m_shininess < 100)
	{
		m_shininess += 10;
	}
}

void Pottery::DecreaseShininess()
{
	if (m_shininess > 0)
	{
		m_shininess -= 10;
	}
}

void Pottery::GetPIProp(int pType, PIPROP *pIProp)
{
	switch (pType)
	{
	case 1:
		pIProp->numVE = 100;
		pIProp->numHE = 180;
		pIProp->numE = 18000;
		pIProp->pResFac = 0.028;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 20;
		pIProp->rotPenalty = 50;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 6;

		break;
	case 2:
		pIProp->numVE = 200;
		pIProp->numHE = 180;
		pIProp->numE = 36000;
		pIProp->pResFac = 0.014;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 30;
		pIProp->rotPenalty = 70;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 10;

		break;
	case 3:
		pIProp->numVE = 200;
		pIProp->numHE = 360;
		pIProp->numE = 72000;
		pIProp->pResFac = 0.007;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 60;
		pIProp->rotPenalty = 150;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 14;

		break;
	case 4:
		pIProp->numVE = 400;
		pIProp->numHE = 180;
		pIProp->numE = 72000;
		pIProp->pResFac = 0.007;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 80;
		pIProp->rotPenalty = 200;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 14;

		break;
	case 5:
		pIProp->numVE = 400;
		pIProp->numHE = 360;
		pIProp->numE = 144000;
		pIProp->pResFac = 0.0035;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 150;
		pIProp->rotPenalty = 300;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 20;

		break;
	case 6:
		pIProp->numVE = 400;
		pIProp->numHE = 720;
		pIProp->numE = 288000;
		pIProp->pResFac = 0.00175;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 200;
		pIProp->rotPenalty = 400;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 24;

		break;
	case 7:
		pIProp->numVE = 800;
		pIProp->numHE = 360;
		pIProp->numE = 288000;
		pIProp->pResFac = 0.00175;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 170;
		pIProp->rotPenalty = 360;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 24;

		break;
	case 8:
		pIProp->numVE = 800;
		pIProp->numHE = 720;
		pIProp->numE = 576000;
		pIProp->pResFac = 0.000875;
		pIProp->lResFac = 0.6;
		pIProp->rFric = 0.35;
		pIProp->initVisco = 120.0;
		pIProp->maxVisco = 300.0;
		pIProp->minVisco = 60.0;
		pIProp->uVisco = 20.0;
		pIProp->rotCombo = 220;
		pIProp->rotPenalty = 500;
		pIProp->stopCombo = 0;
		pIProp->stopPenalty = 30;

		break;
	}
}

void Pottery::DeformElement(int vn, int hn, double dr, BOOL isLineCol)
{
	int idx = GETIDX(vn, hn);

	if (isLineCol)
	{
		m_potteryElement[idx].dLen = dr / (m_viscosity * Pottery::m_lVFactor);
	}
	else
	{
		m_potteryElement[idx].dLen = dr / m_viscosity;
	}
	m_potteryElement[idx].r -= m_potteryElement[idx].dLen;

	if (m_potteryElement[idx].r < Pottery::m_minRadius)
	{
		m_potteryElement[idx].r = Pottery::m_minRadius;
		m_potteryElement[idx].dLen = 0.0;
	}
	else if (m_potteryElement[idx].r > Pottery::m_maxRadius)
	{
		m_potteryElement[idx].r = Pottery::m_maxRadius;
		m_potteryElement[idx].dLen = 0.0;
	}
}

int Pottery::DeformInteraction(int idx)
{
	double cX;       
	int step = 1;    
	int v, h;        
	double dLen;     
	int i;

	cX = DTOR(m_uAngle) * m_potteryElement[idx].r / 2;
	v = LTOPIDXVN(idx, m_pIProp.numHE);
	h = LTOPIDXHN(idx, m_pIProp.numHE);

	if (!ISODD(v))    
	{
		do
		{
			dLen = m_potteryElement[idx].dLen / (SQUARE(cX * m_itFactor * step) + 1);

			if (!ISODD(step))    
			{
				for (i = h - step / 2; i <= h + step / 2; i++)
				{
					ItDeformTwo(idx, GETIDX(v + step, i), GETIDX(v - step, i), dLen);
				}

				for (i = v - step + 1; i <= v + step - 1; i += 2)
				{
					ItDeformTwo(idx, GETIDX(i, h - step / 2 - 1), GETIDX(i, h + step / 2), dLen);
				}
			}
			else    
			{
				for (i = h - step / 2 - 1; i <= h + step / 2; i++)
				{
					ItDeformTwo(idx, GETIDX(v + step, i), GETIDX(v - step, i), dLen);
				}

				for (i = v - step + 1; i <= v + step - 1; i += 2)
				{
					ItDeformTwo(idx, GETIDX(i, h - step / 2 - 1), GETIDX(i, h + step / 2 + 1), dLen);
				}
			}

			step++;
		} while (abs(dLen) >= Pottery::m_itCutoff);
	}
	else    
	{
		do
		{
			dLen = m_potteryElement[idx].dLen / (SQUARE(cX * m_itFactor * step) + 1);

			if (!ISODD(step))    
			{
				for (i = h - step / 2; i <= h + step / 2; i++)
				{
					ItDeformTwo(idx, GETIDX(v + step, i), GETIDX(v - step, i), dLen);
				}

				for (i = v - step + 1; i <= v + step - 1; i += 2)
				{
					ItDeformTwo(idx, GETIDX(i, h - step / 2), GETIDX(i, h + step / 2 + 1), dLen);
				}
			}
			else    
			{
				for (i = h - step / 2; i <= h + step / 2 + 1; i++)
				{
					ItDeformTwo(idx, GETIDX(v + step, i), GETIDX(v - step, i), dLen);
				}

				for (i = v - step + 1; i <= v + step - 1; i += 2)
				{
					ItDeformTwo(idx, GETIDX(i, h - step / 2 - 1), GETIDX(i, h + step / 2 + 1), dLen);
				}
			}

			step++;
		} while (abs(dLen) >= Pottery::m_itCutoff);
	}

	return step - 1;
}

void Pottery::ResetPosNormal(int maxV, int minV, int maxH, int minH)
{
	int vn, hn;

	for (vn = minV; vn <= maxV; vn++)
	{
		hn = minH - 1;
		do
		{
			hn++;
			if (hn >= m_pIProp.numHE)
			{
				hn = 0;
			}

			CalculatePosition(vn, hn);

			CalculateNormal(vn, hn);
		} while (hn != maxH);
	}
}

void Pottery::ApplyMaterials() const
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, static_cast<GLfloat>(m_shininess));

	glMaterialfv(GL_BACK, GL_AMBIENT, m_backAmbient);
	glMaterialfv(GL_BACK, GL_DIFFUSE, m_backDiffuse);
	glMaterialfv(GL_BACK, GL_SPECULAR, m_backSpecular);
	glMaterialfv(GL_BACK, GL_EMISSION, m_backEmission);
	glMaterialf(GL_BACK, GL_SHININESS, static_cast<GLfloat>(m_backShininess));
}

void Pottery::DrawModel() const
{
	int i, j;
	int idx1, idx2, idx3, idx4;

	glPushMatrix();

	ApplyTransRot();

	ApplyColor();
	ApplyMaterials();

	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_pIProp.numVE - 1; i++)
	{
		for (j = 0; j < m_pIProp.numHE; j++)
		{
			idx1 = GETIDX(i, j);
			idx2 = GETIDX_M(i, j - 1);
			if (!ISODD(i))    
			{
				idx3 = GETIDX_M(i + 1, j - 1);
				idx4 = GETIDX(i + 1, j);
			}
			else    
			{
				idx3 = GETIDX(i + 1, j);
				idx4 = GETIDX_P(i + 1, j + 1);
			}

			glNormal3fv(m_potteryElement[idx1].normal1);
			glVertex3fv(m_potteryElement[idx1].position);
			glVertex3fv(m_potteryElement[idx2].position);
			glVertex3fv(m_potteryElement[idx3].position);

			glNormal3fv(m_potteryElement[idx1].normal2);
			glVertex3fv(m_potteryElement[idx1].position);
			glVertex3fv(m_potteryElement[idx3].position);
			glVertex3fv(m_potteryElement[idx4].position);
		}
	}
	glEnd();

	glPopMatrix();
}

void Pottery::CalculatePosition(int vn, int hn)
{
	int idx = GETIDX(vn, hn);

	m_potteryElement[idx].position[1] = static_cast<GLfloat>(m_potteryElement[idx].h);
	if (!ISODD(vn))    
	{
		m_potteryElement[idx].position[0] = static_cast<GLfloat>(m_potteryElement[idx].r * g_triCosPotEven[hn]);
		m_potteryElement[idx].position[2] = static_cast<GLfloat>(m_potteryElement[idx].r * g_triSinPotEven[hn]);
	}
	else    
	{
		m_potteryElement[idx].position[0] = static_cast<GLfloat>(m_potteryElement[idx].r * g_triCosPotOdd[hn]);
		m_potteryElement[idx].position[2] = static_cast<GLfloat>(m_potteryElement[idx].r * g_triSinPotOdd[hn]);
	}
}

void Pottery::CalculateNormal(int vn, int hn)
{
	int idx = GETIDX(vn, hn);

	if (vn == m_pIProp.numVE - 1)    
	{
		if (!ISODD(vn))    
		{
			CrossProductNormal(idx, GETIDX_M(vn - 1, hn - 1), GETIDX_M(vn, hn - 1), m_potteryElement[idx].normal1);
			CrossProductNormal(idx, GETIDX(vn - 1, hn), GETIDX_M(vn - 1, hn - 1), m_potteryElement[idx].normal2);
		}
		else    
		{
			CrossProductNormal(idx, GETIDX(vn - 1, hn), GETIDX_M(vn, hn - 1), m_potteryElement[idx].normal1);
			CrossProductNormal(idx, GETIDX_P(vn - 1, hn + 1), GETIDX(vn - 1, hn), m_potteryElement[idx].normal2);
		}
	}
	else if (!ISODD(vn))    
	{
		CrossProductNormal(idx, GETIDX_M(vn, hn - 1), GETIDX_M(vn + 1, hn - 1), m_potteryElement[idx].normal1);
		CrossProductNormal(idx, GETIDX_M(vn + 1, hn - 1), GETIDX(vn + 1, hn), m_potteryElement[idx].normal2);
	}
	else    
	{
		CrossProductNormal(idx, GETIDX_M(vn, hn - 1), GETIDX(vn + 1, hn), m_potteryElement[idx].normal1);
		CrossProductNormal(idx, GETIDX(vn + 1, hn), GETIDX_P(vn + 1, hn + 1), m_potteryElement[idx].normal2);
	}
}

void Pottery::ItDeformTwo(int idx, int idx1, int idx2, double dLen)
{
	double nR;    

	if (idx1 >= 0 && idx1 < m_pIProp.numE)
	{
		nR = m_potteryElement[idx1].r - m_potteryElement[idx].r;
		if ((dLen > 0 && nR > dLen) || (dLen < 0 && nR < dLen))
		{
			m_potteryElement[idx1].r -= dLen;
		}
	}

	if (idx2 >= 0 && idx2 < m_pIProp.numE)
	{
		nR = m_potteryElement[idx2].r - m_potteryElement[idx].r;
		if ((dLen > 0 && nR > dLen) || (dLen < 0 && nR < dLen))
		{
			m_potteryElement[idx2].r -= dLen;
		}
	}
}

void Pottery::CrossProductNormal(int v1, int v2, int v3, GLfloat *normal) const
{
	GLfloat u1[3], u2[3];

	u1[0] = m_potteryElement[v2].position[0] - m_potteryElement[v1].position[0];
	u1[1] = m_potteryElement[v2].position[1] - m_potteryElement[v1].position[1];
	u1[2] = m_potteryElement[v2].position[2] - m_potteryElement[v1].position[2];
	u2[0] = m_potteryElement[v3].position[0] - m_potteryElement[v1].position[0];
	u2[1] = m_potteryElement[v3].position[1] - m_potteryElement[v1].position[1];
	u2[2] = m_potteryElement[v3].position[2] - m_potteryElement[v1].position[2];

	normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
	normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
	normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

	Normalize(normal);
}
