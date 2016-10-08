



#include <windows.h>
#include <stdio.h>
#include "./lib/nvgrpbt.h"
#include "HDProc.h"
#include "PotteryHIPModel.h"
#include "ControlWindow.h"
#include "NewDlg.h"
#include "UtilFunc.h"
#include "resource.h"

#ifdef TESTING
extern FILE *g_debugFp;
#endif    


#define BT_NEW           101
#define BT_SAVE          102
#define BT_LOAD          103
#define BT_VISCOSITYUP   104
#define BT_VISCOSITYDOWN 105
#define BT_SHININESSUP   106
#define BT_SHININESSDOWN 107

#define DRAWMSEC 50    



void BufferDraw(int hapticFrame, int graphicFrame);



void GetColorDialog(HWND hWnd, COLORREF *color);


void PlayComboAni();
void PlayPenaltyAni();


LRESULT Control_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Control_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Control_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Control_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);


void OnBtNew(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnBtSave(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnBtLoad(HWND hWnd, WPARAM wParam, LPARAM lParam);





RECT g_selectAmbient = {184, 358, 214, 374};
RECT g_selectDiffuse = {184, 378, 214, 394};
RECT g_selectSpecular = {184, 398, 214, 414};

HDC g_hControlDC;    

HBITMAP g_bufferBitmap;    
HBITMAP g_oldBufferBitmap;
HDC g_bufferDC;            
RECT g_bufferRect;         


HBITMAP g_bmComboPenalty;
HBITMAP g_bmStatusTop;
HBITMAP g_bmStatusLeft;
HBITMAP g_bmHIPTop;
HBITMAP g_bmHIPLeft;
HBITMAP g_bmPotteryTop;
HBITMAP g_bmPotteryLeft;
HBITMAP g_bmKeyboard;
HBITMAP g_bmBottom;
HBITMAP g_bmAniCombo;
HBITMAP g_bmAniPenalty;

HFONT g_defFont;    
HFONT g_oldFont;


BOOL g_isPlayAniCombo = FALSE;
BOOL g_isPlayAniPenalty = FALSE;



hduVector3Dd g_iForce;
int g_iPointCNum;
int g_iLineCNum;

extern HINSTANCE g_hInst;

extern HWND g_hMainWnd;

extern PotteryHIPModel *g_potteryHIP;

extern int g_hapticCallNum;
extern int g_graphicCallNum;


void ControlDraw()
{
	static DWORD drawTickCount = GetTickCount() + DRAWMSEC;    
	static DWORD frameTickCount = GetTickCount();              
	static int hapticFrame = 0, graphicFrame = 0;
	DWORD curTickCount;                                        

	curTickCount = GetTickCount();

	if (curTickCount - frameTickCount >= 1000)
	{
		hapticFrame = g_hapticCallNum;
		g_hapticCallNum = 0;

		graphicFrame = g_graphicCallNum;
		g_graphicCallNum = 0;

		frameTickCount = curTickCount;
	}

	if (curTickCount - drawTickCount >= DRAWMSEC)
	{
		BufferDraw(hapticFrame, graphicFrame);

		BitBlt(g_hControlDC, 4, 12, g_bufferRect.right, g_bufferRect.bottom, g_bufferDC, 0, 0, SRCCOPY);

		drawTickCount = curTickCount;
	}
}

void OnOffComboAni(BOOL isOn)
{
	g_isPlayAniCombo = isOn;
}

void OnOffPenaltyAni(BOOL isOn)
{
	g_isPlayAniPenalty = isOn;
}

void BufferDraw(int hapticFrame, int graphicFrame)
{
	HBITMAP oldBitmap;
	HPEN oldPen;
	HBRUSH brushAmbient, brushDiffuse, brushSpecular;
	HBRUSH oldBrush;
	HDC memDC;
	double td[3];
	int ti[3];
	char str[256];

	memDC = CreateCompatibleDC(g_bufferDC);

	FillRect(g_bufferDC, &g_bufferRect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

	oldBitmap = static_cast<HBITMAP>(SelectObject(memDC, g_bmComboPenalty));
	BitBlt(g_bufferDC, 0, 0, 52, 49, memDC, 0, 0, SRCCOPY);

	SetTextColor(g_bufferDC, RGB(32, 255, 32));
	sprintf(str, "%d", g_potteryHIP->GetCombo());
	TextOut(g_bufferDC, 70, 4, str, lstrlen(str));

	SetTextColor(g_bufferDC, RGB(255, 32, 32));
	sprintf(str, "%d", g_potteryHIP->GetPenalty());
	TextOut(g_bufferDC, 70, 30, str, lstrlen(str));

	SelectObject(memDC, g_bmStatusTop);
	BitBlt(g_bufferDC, 0, 58, 292, 22, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmStatusLeft);
	BitBlt(g_bufferDC, 0, 80, 1, 109, memDC, 0, 0, SRCCOPY);
	BitBlt(g_bufferDC, 291, 80, 1, 109, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmHIPTop);
	BitBlt(g_bufferDC, 0, 197, 292, 22, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmHIPLeft);
	BitBlt(g_bufferDC, 0, 219, 1, 49, memDC, 0, 0, SRCCOPY);
	BitBlt(g_bufferDC, 291, 219, 1, 49, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmPotteryTop);
	BitBlt(g_bufferDC, 0, 276, 292, 22, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmPotteryLeft);
	BitBlt(g_bufferDC, 0, 298, 1, 129, memDC, 0, 0, SRCCOPY);
	BitBlt(g_bufferDC, 291, 298, 1, 129, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmBottom);
	BitBlt(g_bufferDC, 0, 189, 292, 1, memDC, 0, 0, SRCCOPY);
	BitBlt(g_bufferDC, 0, 268, 292, 1, memDC, 0, 0, SRCCOPY);
	BitBlt(g_bufferDC, 0, 427, 292, 1, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, g_bmKeyboard);
	BitBlt(g_bufferDC, 0, 435, 292, 152, memDC, 0, 0, SRCCOPY);

	SetTextColor(g_bufferDC, RGB(255, 255, 255));

	if (g_potteryHIP->GetIsActive())
	{
		if (g_potteryHIP->GetIsOutDeform())
		{
			TextOut(g_bufferDC, 12, 86, "Mode : Deformation (Contract)", 29);
		}
		else
		{
			TextOut(g_bufferDC, 12, 86, "Mode : Deformation (Expand)", 27);
		}
	}
	else
	{
		if (g_potteryHIP->GetIsOutDeform())
		{
			TextOut(g_bufferDC, 12, 86, "Mode : Touch (Outside)", 22);
		}
		else
		{
			TextOut(g_bufferDC, 12, 86, "Mode : Touch (Inside)", 21);
		}
	}
	sprintf(str, "Graphic & Haptic roop : %d | %d times/sec", graphicFrame, hapticFrame);
	TextOut(g_bufferDC, 12, 106, str, lstrlen(str));

	sprintf(str, "Potter's Wheel Speed : %.2f rot/sec", g_potteryHIP->GetElecRotAV() / 360.0);
	TextOut(g_bufferDC, 12, 126, str, lstrlen(str));

	sprintf(str, "Force : %.3fN (%.3f %.3f, %.3f)", g_iForce.magnitude(), g_iForce[0], g_iForce[1], g_iForce[2]);
	TextOut(g_bufferDC, 12, 146, str, lstrlen(str));

	sprintf(str, "Collision : %d elements (%d/%d)", g_iPointCNum + g_iLineCNum, g_iPointCNum, g_iLineCNum);
	TextOut(g_bufferDC, 12, 166, str, lstrlen(str));

	g_potteryHIP->m_sphereHIP->GetPosition(td);
	sprintf(str, "Position : (%.3f, %.3f, %.3f)", td[0], td[1], td[2]);
	TextOut(g_bufferDC, 12, 225, str, lstrlen(str));

	sprintf(str, "Radius : %.1f", g_potteryHIP->m_sphereHIP->GetRadius());
	TextOut(g_bufferDC, 12, 245, str, lstrlen(str));

	sprintf(str, "Resolution : %d (%d X %d)", g_potteryHIP->m_pottery->GetNumVE() * g_potteryHIP->m_pottery->GetNumHE(), g_potteryHIP->m_pottery->GetNumVE(), g_potteryHIP->m_pottery->GetNumHE());
	TextOut(g_bufferDC, 12, 304, str, lstrlen(str));

	sprintf(str, "Viscosity : %d", static_cast<int>(g_potteryHIP->m_pottery->GetViscosity()));
	TextOut(g_bufferDC, 12, 324, str, lstrlen(str));

	oldPen = static_cast<HPEN>(SelectObject(g_bufferDC, GetStockObject(WHITE_PEN)));

	g_potteryHIP->m_pottery->GetAmbient(ti);
	brushAmbient = CreateSolidBrush(RGB(ti[0], ti[1], ti[2]));
	oldBrush = static_cast<HBRUSH>(SelectObject(g_bufferDC, brushAmbient));
	Rectangle(g_bufferDC, 180, 346, 210, 362);
	sprintf(str, "Ambient : (%d, %d, %d)", ti[0], ti[1], ti[2]);
	TextOut(g_bufferDC, 12, 344, str, lstrlen(str));

	g_potteryHIP->m_pottery->GetDiffuse(ti);
	brushDiffuse = CreateSolidBrush(RGB(ti[0], ti[1], ti[2]));
	SelectObject(g_bufferDC, brushDiffuse);
	Rectangle(g_bufferDC, 180, 366, 210, 382);
	sprintf(str, "Diffuse : (%d, %d, %d)", ti[0], ti[1], ti[2]);
	TextOut(g_bufferDC, 12, 364, str, lstrlen(str));

	g_potteryHIP->m_pottery->GetSpecular(ti);
	brushSpecular = CreateSolidBrush(RGB(ti[0], ti[1], ti[2]));
	SelectObject(g_bufferDC, brushSpecular);
	Rectangle(g_bufferDC, 180, 386, 210, 402);
	sprintf(str, "Specular : (%d, %d, %d)", ti[0], ti[1], ti[2]);
	TextOut(g_bufferDC, 12, 384, str, lstrlen(str));

	SelectObject(g_bufferDC, oldPen);
	SelectObject(g_bufferDC, oldBrush);
	DeleteObject(brushAmbient);
	DeleteObject(brushDiffuse);
	DeleteObject(brushSpecular);

	sprintf(str, "Shininess : %d", g_potteryHIP->m_pottery->GetShininess());
	TextOut(g_bufferDC, 12, 404, str, lstrlen(str));

	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);

	PlayComboAni();
	PlayPenaltyAni();
}

void GetColorDialog(HWND hWnd, COLORREF *color)
{
	CHOOSECOLOR cColor;
	static COLORREF crTemp[16] = {
		RGB(170, 202, 170), RGB(150, 178, 150), RGB(130, 150, 130), RGB(159, 178, 203),
		RGB(140, 154, 173), RGB(109, 119, 134), RGB(143, 143, 178), RGB(112, 112, 134),
		RGB(174, 141, 141), RGB(148, 116, 116), RGB(117, 94, 94), RGB(127, 96, 66),
		RGB(153, 153, 153), RGB(127, 127, 127), RGB(67, 67, 67), RGB(12, 12, 12)
	};

	memset(&cColor, 0, sizeof(CHOOSECOLOR));
	cColor.lStructSize = sizeof(CHOOSECOLOR);
	cColor.hwndOwner = hWnd;
	cColor.lpCustColors = crTemp;
	cColor.Flags = CC_FULLOPEN;
	if (ChooseColor(&cColor))
	{
		*color = cColor.rgbResult;
	}
}

void PlayComboAni()
{
	static int frameNum = -1;
	HBITMAP oldBitmap;
	HDC memDC;

	if (g_isPlayAniCombo || frameNum != 0)
	{
		frameNum++;
		if (frameNum >= 10)
		{
			frameNum = 0;
		}
	}

	memDC = CreateCompatibleDC(g_bufferDC);
	oldBitmap = static_cast<HBITMAP>(SelectObject(memDC, g_bmAniCombo));

	BitBlt(g_bufferDC, 170, 4, 19, 17, memDC, frameNum * 19, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);
}

void PlayPenaltyAni()
{
	static int frameNum = -1;
	HBITMAP oldBitmap;
	HDC memDC;

	if (g_isPlayAniPenalty || frameNum != 0)
	{
		frameNum++;
		if (frameNum >= 10)
		{
			frameNum = 0;
		}
	}

	memDC = CreateCompatibleDC(g_bufferDC);
	oldBitmap = static_cast<HBITMAP>(SelectObject(memDC, g_bmAniPenalty));

	BitBlt(g_bufferDC, 170, 28, 18, 16, memDC, frameNum * 18, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);
}

LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return Control_OnCreate(hWnd, wParam, lParam);

	case WM_LBUTTONDOWN:
		return Control_OnLButtonDown(hWnd, wParam, lParam);

	case WM_COMMAND:
		return Control_OnCommand(hWnd, wParam, lParam);

	case WM_DESTROY:
		return Control_OnDestroy(hWnd, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Control_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hButton;

	g_hControlDC = GetDC(hWnd);

	g_bufferRect.top = 0;
	g_bufferRect.right = 292;
	g_bufferRect.bottom = 587;
	g_bufferRect.left = 0;
	g_bufferBitmap = CreateCompatibleBitmap(g_hControlDC, g_bufferRect.right, g_bufferRect.bottom);
	g_bufferDC = CreateCompatibleDC(g_hControlDC);
	g_oldBufferBitmap = static_cast<HBITMAP>(SelectObject(g_bufferDC, g_bufferBitmap));
	g_defFont = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, "Arial");
	g_oldFont = static_cast<HFONT>(SelectObject(g_bufferDC, g_defFont));
	SetBkColor(g_bufferDC, RGB(0, 0, 0));

	g_bmComboPenalty = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_COMBOPENALTY));
	g_bmStatusTop = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STATUSTOP));
	g_bmStatusLeft = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_STATUSLEFT));
	g_bmHIPTop = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_HIPTOP));
	g_bmHIPLeft = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_HIPLEFT));
	g_bmPotteryTop = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_POTTERYTOP));
	g_bmPotteryLeft = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_POTTERYLEFT));
	g_bmKeyboard = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_KEYBOARD));
	g_bmBottom = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BOTTOM));
	g_bmAniCombo = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ANICOMBO));
	g_bmAniPenalty = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_ANIPENALTY));

	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 252, 6, 44, 26, hWnd, reinterpret_cast<HMENU>(BT_NEW), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(26, 44), IDB_NEW));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 252, 34, 44, 26, hWnd, reinterpret_cast<HMENU>(BT_SAVE), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(26, 44), IDB_SAVE));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 252, 62, 44, 26, hWnd, reinterpret_cast<HMENU>(BT_LOAD), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(26, 44), IDB_LOAD));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 279, 334, 16, 10, hWnd, reinterpret_cast<HMENU>(BT_VISCOSITYUP), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(10, 16), IDB_UP));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 279, 344, 16, 10, hWnd, reinterpret_cast<HMENU>(BT_VISCOSITYDOWN), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(10, 16), IDB_DOWN));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 279, 414, 16, 10, hWnd, reinterpret_cast<HMENU>(BT_SHININESSUP), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(10, 16), IDB_UP));
	hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD | WS_VISIBLE, 279, 424, 16, 10, hWnd, reinterpret_cast<HMENU>(BT_SHININESSDOWN), g_hInst, NULL);
	SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(10, 16), IDB_DOWN));

	return 0;
}

LRESULT Control_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COLORREF color;

	if (IsInRect(LOWORD(lParam), HIWORD(lParam), &g_selectAmbient))    
	{
		color = g_potteryHIP->m_pottery->GetAmbient();
		GetColorDialog(hWnd, &color);
		g_potteryHIP->m_pottery->SetAmbient(COLORITOF(GetRValue(color)), COLORITOF(GetGValue(color)), COLORITOF(GetBValue(color)), 1.0f);
	}
	else if (IsInRect(LOWORD(lParam), HIWORD(lParam), &g_selectDiffuse))    
	{
		color = g_potteryHIP->m_pottery->GetDiffuse();
		GetColorDialog(hWnd, &color);
		g_potteryHIP->m_pottery->SetDiffuse(COLORITOF(GetRValue(color)), COLORITOF(GetGValue(color)), COLORITOF(GetBValue(color)), 1.0f);
	}
	else if (IsInRect(LOWORD(lParam), HIWORD(lParam), &g_selectSpecular))    
	{
		color = g_potteryHIP->m_pottery->GetSpecular();
		GetColorDialog(hWnd, &color);
		g_potteryHIP->m_pottery->SetSpecular(COLORITOF(GetRValue(color)), COLORITOF(GetGValue(color)), COLORITOF(GetBValue(color)), 1.0f);
	}

	return 0;
}

LRESULT Control_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case BT_NEW:
		OnBtNew(hWnd, wParam, lParam);

		break;
	case BT_SAVE:
		OnBtSave(hWnd, wParam, lParam);

		break;
	case BT_LOAD:
		OnBtLoad(hWnd, wParam, lParam);

		break;
	case BT_VISCOSITYUP:
		g_potteryHIP->m_pottery->IncreaseViscosity();

		break;
	case BT_VISCOSITYDOWN:
		g_potteryHIP->m_pottery->DecreaseViscosity();

		break;
	case BT_SHININESSUP:
		g_potteryHIP->m_pottery->IncreaseShininess();

		break;
	case BT_SHININESSDOWN:
		g_potteryHIP->m_pottery->DecreaseShininess();

		break;
	}

	return 0;
}

LRESULT Control_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SelectObject(g_bufferDC, g_oldFont);
	DeleteObject(g_defFont);
	SelectObject(g_bufferDC, g_oldBufferBitmap);
    DeleteObject(g_bufferBitmap);
	DeleteDC(g_bufferDC);

	DeleteObject(g_bmComboPenalty);
	DeleteObject(g_bmStatusTop);
	DeleteObject(g_bmStatusLeft);
	DeleteObject(g_bmHIPTop);
	DeleteObject(g_bmHIPLeft);
	DeleteObject(g_bmPotteryTop);
	DeleteObject(g_bmPotteryLeft);
	DeleteObject(g_bmKeyboard);
	DeleteObject(g_bmBottom);
	DeleteObject(g_bmAniCombo);
	DeleteObject(g_bmAniPenalty);

	ReleaseDC(hWnd, g_hControlDC);

	return 0;
}

	int pType;
	int height;
	int radius;
	int maxHeight;
	int minHeight;
	int maxRadius;
	int minRadius;

void OnBtNew(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	NEWDLGPARAM ndParam;

	ndParam.pType = g_potteryHIP->m_pottery->GetPType();
	ndParam.height = static_cast<int>(Pottery::m_defHeight);
	ndParam.radius = static_cast<int>(Pottery::m_defRadius);

	if (GetNewPotteryType(g_hInst, hWnd, &ndParam))
	{
		hdStopScheduler();
		if (HD_DEVICE_ERROR(hdGetError()))
		{
			MessageBox(g_hMainWnd, "Haptic Device Error!", "Error", MB_OK);

			SendMessage(g_hMainWnd, WM_QUIT, 0, 0);

			return;
		}

		delete g_potteryHIP;

		GeneratePotteryHIP(ndParam.pType, static_cast<double>(ndParam.height), static_cast<double>(ndParam.radius));

		hdStartScheduler();
		if (HD_DEVICE_ERROR(hdGetError()))
		{
			MessageBox(g_hMainWnd, "Haptic Device Error!", "Error", MB_OK);

			SendMessage(g_hMainWnd, WM_QUIT, 0, 0);

			return;
		}
	}
}

void OnBtSave(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR fileName[MAX_PATH] = "";
	FILE *fp;

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "virtual pottery data(*.vpd)\0*.vpd\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "vpd";

	if (GetSaveFileName(&ofn))
	{
		if (fp = fopen(fileName, "w"))
		{
			g_potteryHIP->SavePottery(fp);

			fclose(fp);
		}
		else
		{
			MessageBox(g_hMainWnd, "Cannot Create File!", "Error", MB_OK);
		}
	}
}

void OnBtLoad(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	TCHAR fileName[MAX_PATH] = "";
	double scaleFactor;
	FILE *fp;

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "virtual pottery data(*.vpd)\0*.vpd\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "vpd";

	if (GetOpenFileName(&ofn))
	{
		if (fp = fopen(fileName, "r"))
		{
			hdStopScheduler();
			if (HD_DEVICE_ERROR(hdGetError()))
			{
				MessageBox(g_hMainWnd, "Haptic Device Error!", "Error", MB_OK);

				SendMessage(g_hMainWnd, WM_QUIT, 0, 0);

				fclose(fp);

				return;
			}

			scaleFactor = g_potteryHIP->GetScaleFactor();

			delete g_potteryHIP;

			g_potteryHIP = new PotteryHIPModel(fp, scaleFactor);

			fclose(fp);

			hdStartScheduler();
			if (HD_DEVICE_ERROR(hdGetError()))
			{
				MessageBox(g_hMainWnd, "Haptic Device Error!", "Error", MB_OK);

				SendMessage(g_hMainWnd, WM_QUIT, 0, 0);

				return;
			}
		}
		else
		{
			MessageBox(g_hMainWnd, "Cannot Open File!", "Error", MB_OK);
		}
	}
}
