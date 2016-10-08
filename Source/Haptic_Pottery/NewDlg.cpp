



#include <windows.h>
#include <commctrl.h>
#include "Pottery.h"
#include "NewDlg.h"
#include "resource.h"


BOOL CALLBACK NewDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static NEWDLGPARAM *ndParam;

	switch (message)
	{
	case WM_INITDIALOG:
		ndParam = reinterpret_cast<NEWDLGPARAM*>(lParam);

		switch (ndParam->pType)
		{
		case 1:
			CheckDlgButton(hDlg, IDC_RADIOTYPE1, BST_CHECKED);

			break;
		case 2:
			CheckDlgButton(hDlg, IDC_RADIOTYPE2, BST_CHECKED);

			break;
		case 3:
			CheckDlgButton(hDlg, IDC_RADIOTYPE3, BST_CHECKED);

			break;
		case 4:
			CheckDlgButton(hDlg, IDC_RADIOTYPE4, BST_CHECKED);

			break;
		case 5:
			CheckDlgButton(hDlg, IDC_RADIOTYPE5, BST_CHECKED);

			break;
		case 6:
			CheckDlgButton(hDlg, IDC_RADIOTYPE6, BST_CHECKED);

			break;
		case 7:
			CheckDlgButton(hDlg, IDC_RADIOTYPE7, BST_CHECKED);

			break;
		case 8:
			CheckDlgButton(hDlg, IDC_RADIOTYPE8, BST_CHECKED);

			break;
		}

		SendMessage(GetDlgItem(hDlg, IDC_SPINHEIGHT), UDM_SETRANGE, NULL, MAKELPARAM(Pottery::m_maxHeight, Pottery::m_minHeight));
		SendMessage(GetDlgItem(hDlg, IDC_SPINRADIUS), UDM_SETRANGE, NULL, MAKELPARAM(Pottery::m_maxInitRadius, Pottery::m_minInitRadius));

		SetDlgItemInt(hDlg, IDC_EDITHEIGHT, ndParam->height, TRUE);
		SetDlgItemInt(hDlg, IDC_EDITRADIUS, ndParam->radius, TRUE);

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE1))
			{
				ndParam->pType = 1;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE2))
			{
				ndParam->pType = 2;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE3))
			{
				ndParam->pType = 3;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE4))
			{
				ndParam->pType = 4;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE5))
			{
				ndParam->pType = 5;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE6))
			{
				ndParam->pType = 6;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE7))
			{
				ndParam->pType = 7;
			}
			else if (IsDlgButtonChecked(hDlg, IDC_RADIOTYPE8))
			{
				ndParam->pType = 8;
			}

			ndParam->height = GetDlgItemInt(hDlg, IDC_EDITHEIGHT, NULL, TRUE);
			ndParam->radius = GetDlgItemInt(hDlg, IDC_EDITRADIUS, NULL, TRUE);

			EndDialog(hDlg, IDOK);

			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);

			return TRUE;
		}

		break;
	}

	return FALSE;
}

BOOL GetNewPotteryType(HINSTANCE hInst, HWND hWnd, NEWDLGPARAM *ndParam)
{
	NEWDLGPARAM dlgPType = *ndParam;

	if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_NEW), hWnd, NewDlgProc, (LPARAM)&dlgPType) == IDOK)
	{
		*ndParam = dlgPType;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
