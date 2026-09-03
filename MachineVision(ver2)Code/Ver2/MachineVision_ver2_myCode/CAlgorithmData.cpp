#include "CAlgorithmData.h"
#include <string>
#include <cstdlib>
#include <windows.h>

using namespace std;

CAlgorithmData::CAlgorithmData() 
	: m_bAlgResult(false), m_nPtnCount(0)
{
	// 배열 멤버변수 초기화
	memset(m_szAlgName, 0, sizeof(char) * ALG_MAX_NAME_SIZE);

	for (int i_ptnName = 0; i_ptnName < ALG_MAX_PTN_CNT; i_ptnName++)
	{
		memset(m_szPtnName[i_ptnName], 0, sizeof(char) * ALG_MAX_PTN_NAME_SIZE);
	}
}

CAlgorithmData::~CAlgorithmData()
{
}

void CAlgorithmData::SetAlgName(const char* algName)
{
	strcpy_s(m_szAlgName, ALG_MAX_NAME_SIZE, algName);
}

const char* CAlgorithmData::GetAlgName() const
{
	return m_szAlgName;
}


bool CAlgorithmData::GetAlgResult() const
{
	return m_bAlgResult;
}

void CAlgorithmData::SetAlgPtnInfo(const char* ptnName)
{
	string str = ptnName;
	string delim = ",";

	size_t pos = 0;
	while ((pos = str.find(delim)) != string::npos)
	{
		strcpy_s(m_szPtnName[m_nPtnCount], ALG_MAX_NAME_SIZE, str.substr(0, pos).c_str());
		m_nPtnCount++;
		str.erase(0, pos + delim.length());
	}
	// 마지막 패턴까지 저장
	strcpy_s(m_szPtnName[m_nPtnCount], ALG_MAX_NAME_SIZE, str.c_str());
	m_nPtnCount++;
}

bool CAlgorithmData::CheckAlgPtn(const char* ptnName) const
{
	for (int i_Ptn = 0; i_Ptn < m_nPtnCount; i_Ptn++)
	{
		if (0 == strcmp(m_szPtnName[i_Ptn], ptnName))
			return true;
	}
	
	return false;
}

void CAlgorithmData::RunAlgorithm()
{
	printf("CAlgorithmData::RunAlgorithm(%s) called.\n", m_szAlgName);
	Sleep(1000);

	if ((rand() % 10) >= 8)
		m_bAlgResult = false;	// 1/5 확률로 false
	else
		m_bAlgResult = true;
	printf("CAlgorithmData::RunAlgorithm(%s) finished.\n", m_szAlgName);
}