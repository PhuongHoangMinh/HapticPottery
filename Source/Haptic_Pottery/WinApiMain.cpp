











#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <HD/hd.h>
#include <commctrl.h>
#include "./lib/nvgrpbt.h"
#include "GLDraw.h"
#include "HDProc.h"
#include "ControlWindow.h"
#include "PotteryWindow.h"
#include "PotteryHIPModel.h"
#include "NewDlg.h"
#include "resource.h"

#ifdef TESTING
#include <stdio.h>
FILE *g_debugFp;    
#endif    

#ifdef FORCE
FILE *g_maxPPoint;
FILE *g_maxPLine;
FILE *g_averPPoint;
FILE *g_averPLine;
FILE *g_forcePoint;
FILE *g_forceLine;
FILE *g_forceVis;
FILE *g_forceRot;
FILE *g_colPoint;
FILE *g_colLine;
FILE *g_totalForce;
FILE *g_filteredForce;
FILE *g_lFilteredForce;
#endif    


#define MW_TITLE "Virtual Pottery"


#define WC_MAIN "VirtualPottery"
#define WC_CONTROL "VPControl"
#define WC_POTTERY "VPPottery"

#define DR_WINDOW(W) DestroyWindow(W); W = 0;                   
#define DR_DC(W, D) ReleaseDC(W, D); D = 0;                     
#define DR_RC(R) wglDeleteContext(R); R = 0;                    
#define DR_HD(H) hdDisableDevice(H); H = HD_INVALID_HANDLE;     



void MyRegisterClass(HINSTANCE hInstance);


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


void ReleaseResources();


void MyUnRegisterClass();


LRESULT Main_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Main_OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam);


HINSTANCE g_hInst;     

HWND g_hMainWnd;       
HWND g_hControlWnd;    
HWND g_hPotteryWnd;    

HDC g_hPotteryDC;      
HGLRC g_hRC;           
HHD g_hHD;             


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	BOOL isMessagePumpActive = TRUE;
	MSG msg;
	NEWDLGPARAM ndParam;

#ifdef TESTING
	LARGE_INTEGER qPFreq;    

	g_debugFp = fopen("./TESTING.txt", "w");

	if (QueryPerformanceFrequency(&qPFreq))
	{
		fprintf(g_debugFp, "Start Freq H : %u, L : %u\n",  qPFreq.HighPart, qPFreq.LowPart);
	}
	else
	{
		fprintf(g_debugFp, "QueryPerformanceCounter unavailable\n");
	}
#endif    

#ifdef FORCE
	g_maxPPoint = fopen("./maxPPoint.txt", "w");
	g_maxPLine = fopen("./maxPLine.txt", "w");
	g_averPPoint = fopen("./averPPoint.txt", "w");
	g_averPLine = fopen("./averPLine.txt", "w");
	g_forcePoint = fopen("./forcePoint.txt", "w");
	g_forceLine = fopen("./forceLine.txt", "w");
	g_forceVis = fopen("./forceVis.txt", "w");
	g_forceRot = fopen("./forceRot.txt", "w");
	g_colPoint = fopen("./colPoint.txt", "w");
	g_colLine = fopen("./colLine.txt", "w");
	g_totalForce = fopen("./totalForce.txt", "w");
	g_filteredForce = fopen("./filteredForce.txt", "w");
	g_lFilteredForce = fopen("./lFilteredForce.txt", "w");
#endif    

	ndParam.pType = static_cast<int>(Pottery::m_defPType);
	ndParam.height = static_cast<int>(Pottery::m_defHeight);
	ndParam.radius = static_cast<int>(Pottery::m_defRadius);

	InitCommonControls();

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (GetNewPotteryType(g_hInst, g_hMainWnd, &ndParam))
	{
		GeneratePotteryHIP(ndParam.pType, static_cast<double>(ndParam.height), static_cast<double>(ndParam.radius));

		if (HDInit())
		{
			GLInit();

			HapticProc();

			while (isMessagePumpActive)
			{
				if (PeekMessage(&msg, g_hMainWnd, 0, 0, PM_REMOVE))
				{
					if (msg.message != WM_QUIT)
					{
						DispatchMessage(&msg);
					}
					else
					{
						isMessagePumpActive = FALSE;
					}
				}
				else
				{
					ControlDraw();

					GLDraw();
					SwapBuffers(g_hPotteryDC);
				}
			}

			hdStopScheduler();
		}

		DeletePotteryHIP();
	}

	ReleaseResources();

	MyUnRegisterClass();

#ifdef FORCE
	fclose(g_maxPPoint);
	fclose(g_maxPLine);
	fclose(g_averPPoint);
	fclose(g_averPLine);
	fclose(g_forcePoint);
	fclose(g_forceLine);
	fclose(g_forceVis);
	fclose(g_forceRot);
	fclose(g_colPoint);
	fclose(g_colLine);
	fclose(g_totalForce);
	fclose(g_filteredForce);
	fclose(g_lFilteredForce);
#endif    

#ifdef TESTING
	fclose(g_debugFp);
#endif    

	return static_cast<int>(msg.wParam);
}

void MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, reinterpret_cast<LPCTSTR>(IDI_MAIN));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WC_MAIN;
	wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_MAIN_S));
	RegisterClassEx(&wcex);

	
	wcex.style |= CS_OWNDC;
	wcex.lpfnWndProc = ControlWndProc;
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszClassName = WC_CONTROL;
	RegisterClassEx(&wcex);

	
	wcex.lpfnWndProc = PotteryWndProc;
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszClassName = WC_POTTERY;
	RegisterClassEx(&wcex);

	NVGrpBtRegister(hInstance);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	GLuint pixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
			16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	g_hInst = hInstance;

	g_hMainWnd = CreateWindowEx(WS_EX_APPWINDOW, WC_MAIN, MW_TITLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInst, NULL);
	if (!g_hMainWnd)
	{
		MessageBox(g_hMainWnd, "Fail to Create Main Window!", "Error", MB_OK);

		return FALSE;
	}

	g_hControlWnd = CreateWindowEx(WS_EX_APPWINDOW, WC_CONTROL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		0, 0, 10, 10, g_hMainWnd, NULL, g_hInst, NULL);
	if (!g_hControlWnd)
	{
		MessageBox(g_hMainWnd, "Fail to Create Control Window!", "Error", MB_OK);

		return FALSE;
	}
	
	g_hPotteryWnd = CreateWindowEx(WS_EX_APPWINDOW, WC_POTTERY, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		0, 0, 10, 10, g_hMainWnd, NULL, g_hInst, NULL);
	if (!g_hPotteryWnd)
	{
		MessageBox(g_hMainWnd, "Fail to Create Pottery Window!", "Error", MB_OK);

		return FALSE;
	}

	g_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(hdGetError()))
	{
		MessageBox(g_hMainWnd, "Failed to initialize haptic device!", "Error", MB_OK);

		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	g_hPotteryDC = GetDC(g_hPotteryWnd);
	if (!g_hPotteryDC)
	{
		MessageBox(g_hMainWnd, "Fail to Create Main Window DC!", "Error", MB_OK);

		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	pixelFormat = ChoosePixelFormat(g_hPotteryDC, &pfd);
	if (!pixelFormat)
	{
		MessageBox(g_hMainWnd, "Fail to Choose Pixel Format!", "Error", MB_OK);

		DR_DC(g_hPotteryWnd, g_hPotteryDC)
		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	if (!SetPixelFormat(g_hPotteryDC, pixelFormat, &pfd))
	{
		MessageBox(g_hMainWnd, "Fail to Set Pixel Format!", "Error", MB_OK);

		DR_DC(g_hPotteryWnd, g_hPotteryDC)
		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	g_hRC = wglCreateContext(g_hPotteryDC);
	if (!g_hRC)
	{
		MessageBox(g_hMainWnd, "Fail to Create Rendering Context!", "Error", MB_OK);

		DR_DC(g_hPotteryWnd, g_hPotteryDC)
		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	if (!wglMakeCurrent(g_hPotteryDC, g_hRC))
	{
		MessageBox(g_hMainWnd, "Fail to Set Rendering Context!", "Error", MB_OK);

		DR_RC(g_hRC)
		DR_DC(g_hPotteryWnd, g_hPotteryDC)
		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)

		return FALSE;
	}

	ShowWindow(g_hMainWnd, nCmdShow);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		return Main_OnSize(hWnd, wParam, lParam);

	case WM_GETMINMAXINFO:
		return Main_OnGetMinMaxInfo(hWnd, wParam, lParam);

	case WM_KEYDOWN: case WM_KEYUP:
		return SendMessage(g_hPotteryWnd, message, wParam, lParam);

	case WM_CLOSE:
		PostMessage(hWnd, WM_QUIT, 0, 0);

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void ReleaseResources()
{
	if (g_hPotteryWnd)
	{
		if (g_hPotteryDC)
		{
			wglMakeCurrent(g_hPotteryDC, 0);
			
			if (g_hRC)
			{
				DR_RC(g_hRC)
			}

			DR_DC(g_hPotteryWnd, g_hPotteryDC)
		}

		DR_WINDOW(g_hPotteryWnd)
		DR_WINDOW(g_hControlWnd)
		DR_WINDOW(g_hMainWnd)
	}

	if (g_hHD != HD_INVALID_HANDLE)
	{
		DR_HD(g_hHD)
	}
}

void MyUnRegisterClass()
{
	UnregisterClass("NVGrpBt", g_hInst);

	UnregisterClass(WC_POTTERY, g_hInst);
	UnregisterClass(WC_CONTROL, g_hInst);
	UnregisterClass(WC_MAIN, g_hInst);
}

LRESULT Main_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rt;

	if (wParam != SIZE_MINIMIZED)
	{
		GetClientRect(hWnd, &rt);
		MoveWindow(g_hControlWnd, 0, 0, 300, rt.bottom, TRUE);
		MoveWindow(g_hPotteryWnd, 300, 0, rt.right - 300, rt.bottom, TRUE);
	}

	return 0;
}

LRESULT Main_OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO mmi = reinterpret_cast<LPMINMAXINFO>(lParam);

	mmi->ptMinTrackSize.x = 700;
	mmi->ptMinTrackSize.y = 640;

	return 0;
}
