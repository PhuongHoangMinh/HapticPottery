#ifndef NEWDLG_H__20071201__INCED
#define NEWDLG_H__20071201__INCED



typedef struct {
	int pType;
	int height;
	int radius;
} NEWDLGPARAM;



BOOL CALLBACK NewDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);




BOOL GetNewPotteryType(HINSTANCE hInst, HWND hWnd, NEWDLGPARAM *ndParam);


#endif    
