



#include "HDProc.h"
#include "PotteryHIPModel.h"

#ifdef TESTING
#include <stdio.h>
extern FILE *g_debugFp;
#endif    


int g_hapticCallNum = 0;    

PotteryHIPModel *g_potteryHIP;    

extern HWND g_hMainWnd;


int HDInit()
{
	hdEnable(HD_FORCE_OUTPUT);
	hdEnable(HD_MAX_FORCE_CLAMPING);

	hdStartScheduler();
	if (HD_DEVICE_ERROR(hdGetError()))
	{
		MessageBox(g_hMainWnd, "Haptic Device Error!", "Error", MB_OK);

		return 0;
	}

	return 1;
}

void GeneratePotteryHIP(int pType, double height, double radius)
{
	HDdouble maxWorkspace[6];
	HDdouble minDim;

	hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
	minDim = maxWorkspace[3] - maxWorkspace[0];
	if (minDim > maxWorkspace[4] - maxWorkspace[1])
	{
		minDim = maxWorkspace[4] - maxWorkspace[1];
	}
	if (minDim > maxWorkspace[5] - maxWorkspace[2])
	{
		minDim = maxWorkspace[5] - maxWorkspace[2];
	}

	g_potteryHIP = new PotteryHIPModel(pType, height, radius, 150.0 / minDim);
}

void DeletePotteryHIP()
{
	delete g_potteryHIP;
}

HDCallbackCode HDCALLBACK GetHIPPosition(void *data)
{
	hduVector3Dd center;
	PotteryHIPModel *potteryHIP = static_cast<PotteryHIPModel*>(data);

	HHD hHD = hdGetCurrentDevice();

	hdBeginFrame(hHD);

	hdGetDoublev(HD_CURRENT_POSITION, center);

	potteryHIP->UpdatePosition(center[0], center[1], center[2]);

	hdSetDoublev(HD_CURRENT_FORCE, potteryHIP->GetForce());

	hdEndFrame(hHD);
	    

	g_hapticCallNum++;

	return HD_CALLBACK_CONTINUE;
}

void HapticProc()
{
	hdScheduleAsynchronous(GetHIPPosition, g_potteryHIP, HD_DEFAULT_SCHEDULER_PRIORITY);
}
