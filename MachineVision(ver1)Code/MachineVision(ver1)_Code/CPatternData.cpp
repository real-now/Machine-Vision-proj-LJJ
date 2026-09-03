#include "CPatternData.h"
#include <cstring>

CPatternData::CPatternData(): m_nShutterSpeed(0) 
{
	// 배열 멤버변수 초기화
	memset(m_szPtnName, 0, sizeof(char) * MAX_PTN_NAME_SIZE);
}

CPatternData::~CPatternData() 
{	
}

void CPatternData::SetPtnName(const char* ptnName)
{
	strcpy(m_szPtnName, ptnName);
}

const char* CPatternData::GetPtnName() const
{
	return m_szPtnName;
}

void CPatternData::SetShutterSpeed(int shutterSpeed)
{
	m_nShutterSpeed = shutterSpeed;
}

int CPatternData::GetShutterSpeed() const
{
	return m_nShutterSpeed;
}