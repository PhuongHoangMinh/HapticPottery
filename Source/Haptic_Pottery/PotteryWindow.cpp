



#include <windows.h>
#include "GLDraw.h"
#include "PotteryHIPModel.h"
#include "PotteryWindow.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    



LRESULT Pottery_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Pottery_OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT Pottery_OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);


extern PotteryHIPModel *g_potteryHIP;


LRESULT CALLBACK PotteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		return Pottery_OnSize(hWnd, wParam, lParam);

	case WM_KEYDOWN:
		return Pottery_OnKeyDown(hWnd, wParam, lParam);

	case WM_KEYUP:
		return Pottery_OnKeyUp(hWnd, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Pottery_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
	{
		ReshapeGL(LOWORD(lParam), HIWORD(lParam));
	}

	return 0;
}

LRESULT Pottery_OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_RIGHT:
		g_potteryHIP->SetHandRotType(ROTATE_RIGHT);

		break;
	case VK_LEFT:
		g_potteryHIP->SetHandRotType(ROTATE_LEFT);

		break;
	case VK_UP:
		g_potteryHIP->SetHandRotType(ROTATE_UP);

		break;
	case VK_DOWN:
		g_potteryHIP->SetHandRotType(ROTATE_DOWN);

		break;
	case VK_PRIOR:
		ZoomOut();

		break;
	case VK_NEXT:
		ZoomIn();

		break;
	case 0x41:    
		g_potteryHIP->m_sphereHIP->IncreaseHIPSize();

		break;
	case 0x5A:    
		g_potteryHIP->m_sphereHIP->DecreaseHIPSize();

		break;
	case 0x53:    
		g_potteryHIP->WheelSpeedUp();

		break;
	case 0x58:    
		g_potteryHIP->WheelSpeedDown();

		break;
	case 0x51:    
		g_potteryHIP->ToggleIsActive();

		break;
	}

	return 0;
}

LRESULT Pottery_OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_RIGHT: case VK_LEFT: case VK_UP: case VK_DOWN:
		g_potteryHIP->SetHandRotType(ROTATE_STOP);

		break;
	}

	return 0;
}
