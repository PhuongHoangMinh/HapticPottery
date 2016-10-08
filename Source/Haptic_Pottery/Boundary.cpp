#include "Boundary.h"

#ifdef TESTING
extern FILE *g_debugFp;
#endif    


Boundary::Boundary(int numHE)
{
	m_numHE = numHE;
}

void Boundary::InitVariables()
{
	m_isVColliding = FALSE;
	m_isHColliding = FALSE;
	m_isVBoundaryAll = FALSE;
	m_hSIndex = -1;
	m_hEIndex = -1;
	m_pHSIndex = -1;
	m_pHEIndex = -1;
}

void Boundary::ProcColliding(int hn)
{
	if (!m_isVColliding)
	{
		m_isVColliding = TRUE;
		m_isHColliding = TRUE;
		m_isVBoundaryAll = TRUE;
		m_hSIndex = hn;
	}
	else if (!m_isHColliding)
	{
		m_isHColliding = TRUE;
		m_hSIndex = hn;
	}
}

void Boundary::ProcNColliding(int hn)
{
	if (m_isHColliding)
	{
		m_isHColliding = FALSE;
		m_hEIndex = (hn == 0) ? m_numHE - 1 : hn - 1;
	}
}

void Boundary::ProcLineAndSave(int vn, int validHNMax, std::list<int> &bList)
{
	if (m_hEIndex == -1)
	{
		m_hEIndex = validHNMax;
	}

	if (m_isVBoundaryAll)
	{
		m_isVBoundaryAll = FALSE;
		RangeSave(vn, m_hSIndex, m_hEIndex, bList);
	}
	else if (m_isVColliding)
	{
		if (m_hSIndex == -1)
		{
			int pDist = m_pHEIndex - m_pHSIndex;
			pDist = (pDist < 0) ? pDist + 361 : pDist + 1;

			if (pDist >= 3)
			{
				RangeSave(vn - 1, (m_pHSIndex + 1 >= m_numHE) ? 0 : m_pHSIndex + 1, (m_pHEIndex - 1 < 0) ? m_numHE - 1 : m_pHEIndex - 1, bList);
			}

			InitVariables();
		}
		else
		{
			TwoSave(vn, m_hSIndex, m_hEIndex, bList);
		}
	}
}

void Boundary::RangeSave(int vn, int sIndex, int eIndex, std::list<int> &bList)
{
	int i;

	i = sIndex - 1;
	do
	{
		i = (i == m_numHE - 1) ? 0 : i + 1;

		bList.push_back(PTOLIDX(vn, i, m_numHE));
	} while (i != eIndex);
}

void Boundary::TwoSave(int vn, int sIndex, int eIndex, std::list<int> &bList)
{
	bList.push_back(PTOLIDX(vn, sIndex, m_numHE));
	if (sIndex != eIndex)
	{
		bList.push_back(PTOLIDX(vn, eIndex, m_numHE));
	}
}

void Boundary::ProcLineEnd()
{
	m_isHColliding = FALSE;
	m_pHSIndex = m_hSIndex;
	m_pHEIndex = m_hEIndex;
	m_hSIndex = -1;
	m_hEIndex = -1;
}
