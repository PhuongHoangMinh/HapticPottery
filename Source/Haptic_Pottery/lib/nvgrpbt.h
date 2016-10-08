//
// 그래픽 푸쉬 버튼 커스텀 컨트롤 라이브러리
//
// 제작 : 이재봉
// 소속 : 포항공과대학교 수학과
// 홈페이지 : http://www.jaebong.net/
// 최종 갱신일 : 2006. 8. 19
//
//
//
// - 사용방법 -
//
//
// 1) NVGrpBtRegister() 함수를 호출하여 윈도우 클래스 등록
//    (보통 프로그램 메인 윈도우 클래스를 등록한 직후 호출하는 것이 좋음)
//
//
// 2) 개개 버튼은 CreateWindow 함수로 생성
//    버튼 윈도우 클래스 명은 "NVGrpBt"
//
//    사용 예)
//      hButton = CreateWindow("NVGrpBt", NULL, WS_CHILD, 466, 3, 13, 13, g_hWndMain, (HMENU)BT_OK, g_hInst, NULL);
//
//
// 3) NVGB_INIT 메세지와 wParam, lParam 으로 이미지 로딩 관련 정보를 넘김
//    (CreateWindow 호출 직후 호출하는 것이 좋음)
//
//    wParam 으로 호출 프로그램 인스턴스 핸들
//
//    HIWORD(lParam) 으로 비트맵 ID
//    (비트맵 순서는 처음이 보통 상태, 두번째가 마우스 오버 상태, 세번째가 마우스 다운 상태)
//    (세 개 이미지가 같은 크기로 한 개 파일에 가로 일렬로 배치)
//
//    HIBYTE(LOWORD(lParam)) 으로 버튼 가로 크기 (픽셀 단위)
//    LOBYTE(LOWORD(lParam)) 으로 버튼 세로 크기
//
//    사용 예)
//      SendMessage(hButton, NVGB_INIT, (WPARAM)g_hInst, MAKELPARAM(MAKEWORD(13, 13), IDB_BT_OK));
//
//
// 4) 사용자가 버튼을 클릭하면 (마우스 업)
//    WM_COMMAND 메세지와 LOWORD(wParam)에 버튼 아이디, lParam에 버튼 윈도우 핸들을 보냄
//
//
// 5) 윈도우 클래스를 해제한다
//
//    예) UnregisterClass("NVGrpBt", g_hInst);
//

#ifndef NVGRPBT_H__20060819__INCED
#define NVGRPBT_H__20060819__INCED


#ifdef NVDLLEXPORT
#define NVDLLTYPE __declspec(dllexport)
#else
#define NVDLLTYPE __declspec(dllimport)
#endif



#define NVGB_INIT    (WM_USER + 1)    // 컨트롤 초기화



// 윈도우 클래스 등록
extern "C" NVDLLTYPE void NVGrpBtRegister(HINSTANCE hInst);



#endif    // NVGRPBT_H__20060819__INCED
