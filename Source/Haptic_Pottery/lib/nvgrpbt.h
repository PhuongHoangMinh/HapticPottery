//
// �׷��� Ǫ�� ��ư Ŀ���� ��Ʈ�� ���̺귯��
//
// ���� : �����
// �Ҽ� : ���װ������б� ���а�
// Ȩ������ : http://www.jaebong.net/
// ���� ������ : 2006. 8. 19
//
//
//
// - ����� -
//
//
// 1) NVGrpBtRegister() �Լ��� ȣ���Ͽ� ������ Ŭ���� ���
//    (���� ���α׷� ���� ������ Ŭ������ ����� ���� ȣ���ϴ� ���� ����)
//
//
// 2) ���� ��ư�� CreateWindow �Լ��� ����
//    ��ư ������ Ŭ���� ���� "NVGrpBt"
//
//    ��� ��)
//      hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD, 466, 3, 13, 13, g_hWndMain, (HMENU)BT_OK, g_hInst, NULL);
//
//
// 3) NVGB_INIT �޼����� wParam, lParam ���� �̹��� �ε� ���� ������ �ѱ�
//    (CreateWindow ȣ�� ���� ȣ���ϴ� ���� ����)
//
//    wParam ���� ȣ�� ���α׷� �ν��Ͻ� �ڵ�
//
//    HIWORD(lParam) ���� ��Ʈ�� ID
//    (��Ʈ�� ������ ó���� ���� ����, �ι�°�� ���콺 ���� ����, ����°�� ���콺 �ٿ� ����)
//    (�� �� �̹����� ���� ũ��� �� �� ���Ͽ� ���� �Ϸķ� ��ġ)
//
//    HIBYTE(LOWORD(lParam)) ���� ��ư ���� ũ�� (�ȼ� ����)
//    LOBYTE(LOWORD(lParam)) ���� ��ư ���� ũ��
//
//    ��� ��)
//      SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(13, 13), IDB_BT_OK));
//
//
// 4) ����ڰ� ��ư�� Ŭ���ϸ� (���콺 ��)
//    WM_COMMAND �޼����� LOWORD(wParam)�� ��ư ���̵�, lParam�� ��ư ������ �ڵ��� ����
//
//
// 5) ������ Ŭ������ �����Ѵ�
//
//    ��) UnregisterClass("NVGrpBt", g_hInst);
//

#ifndef NVGRPBT_H__20060819__INCED
#define NVGRPBT_H__20060819__INCED


#ifdef NVDLLEXPORT
#define NVDLLTYPE __declspec(dllexport)
#else
#define NVDLLTYPE __declspec(dllimport)
#endif



#define NVGB_INIT    (WM_USER + 1)    // ��Ʈ�� �ʱ�ȭ



// ������ Ŭ���� ���
extern "C" NVDLLTYPE void NVGrpBtRegister(HINSTANCE hInst);



#endif    // NVGRPBT_H__20060819__INCED
