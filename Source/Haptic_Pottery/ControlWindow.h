#ifndef CONTROLWINDOW_H__20071130__INCED
#define CONTROLWINDOW_H__20071130__INCED



LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


void ControlDraw();


void OnOffComboAni(BOOL isOn);
void OnOffPenaltyAni(BOOL isOn);


#endif    
