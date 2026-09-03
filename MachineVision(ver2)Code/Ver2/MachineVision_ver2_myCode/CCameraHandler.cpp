#include "CCameraHandler.h"
#include <cstdio>
#include <windows.h>

CCameraHandler::CCameraHandler() : m_nShutterSpeed(0), m_bCamOn(false) {}

void CCameraHandler::ConnectCam()
{
	printf("CCameraHandler::ConnectCam called.\n");
	Sleep(1000);
	m_bCamOn = true;
	printf("CCameraHandler::ConnectCam finished.\n");
}

void CCameraHandler::SetCamParams(int shutterSpeed)
{	
	printf("CCameraHandler::SetCamParams called(%ds).\n", shutterSpeed);
	Sleep(1000);
	m_nShutterSpeed = shutterSpeed;
	printf("CCameraHandler::SetCamParams finished(%ds).\n", shutterSpeed);
}

void CCameraHandler::Grab(const char* ptnName)
{
	printf("CCameraHandler::Grab called(%s).\n", ptnName);
	Sleep(m_nShutterSpeed * 1000);
	printf("CCameraHandler::Grab finished(%s).\n", ptnName);
}

void CCameraHandler::DisconnectCam()
{
	printf("CCameraHandler::DisconnectCam called.\n");
	Sleep(1000);
	m_bCamOn = false;
	printf("CCameraHandler::DisconnectCam finished.\n");
}

bool CCameraHandler::CheckCamOn() const
{
	printf("CCameraHandler::CheckCamOn called.\n");
	return m_bCamOn;
}
