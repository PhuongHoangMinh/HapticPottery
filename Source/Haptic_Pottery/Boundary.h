#ifndef BOUNDARY_H__20080307__INCED
#define BOUNDARY_H__20080307__INCED

#include <windows.h>
#include <list>


#define PTOLIDX(vn, hn, numHE) ((vn) * (numHE) + (hn))

#define LTOPIDXVN(idx, numHE) ((idx) / (numHE))
#define LTOPIDXHN(idx, numHE) ((idx) % (numHE))



class Boundary {
public:
	Boundary(int numHE);

	
	void InitVariables();
	
	void ProcColliding(int hn);
	
	void ProcNColliding(int hn);
	
	void ProcLineAndSave(int vn, int validHNMax, std::list<int> &bList);
	
	void RangeSave(int vn, int sIndex, int eIndex, std::list<int> &bList);
	
	void TwoSave(int vn, int sIndex, int eIndex, std::list<int> &bList);
	
	void ProcLineEnd();

protected:
	int m_numHE;    

	BOOL m_isVColliding, m_isHColliding;    
	BOOL m_isVBoundaryAll;                  
	int m_hSIndex, m_hEIndex;               
	int m_pHSIndex, m_pHEIndex;             
};


#endif    
