#include "CInspectionHandler.h"
#include <cstdio>
#include <windows.h>
#include <time.h>

// 생성자
CInspectionHandler::CInspectionHandler()
	:m_Patterns(NULL), m_Algorithms(NULL), m_CameraHandler(NULL), m_nTotalPtnCnt(0), m_nTotalAlgCnt(0)
{
}

// 소멸자
CInspectionHandler::~CInspectionHandler()
{
	if (m_Patterns)
		delete[] m_Patterns;

	if (m_Algorithms)
		delete[] m_Algorithms;

	if (m_CameraHandler)
	{
		if (m_CameraHandler->CheckCamOn())
			m_CameraHandler->DisconnectCam();	// 카메라가 연결되어 있으면 카메라 연결 종료
		delete[] m_CameraHandler;
	}		
}

void CInspectionHandler::Initialize()
{
	bool inspContinue = true;
		
	if (false == ReadConfig())
	{
		printf("Config 파일을 확인하세요.\n");
		return;
	}
	
	m_CameraHandler = new CCameraHandler;	// 카메라 핸들러 객체 생성
	m_CameraHandler->ConnectCam();			// 카메라 연결	

	while (inspContinue)
	{
		int menuNum;

		ShowMenu();		
		scanf_s("%d", &menuNum);

		switch (menuNum)
		{
		case INSP_START:
			clock_t startTime, endTime;
			double runTime;
			startTime = clock();		// 시작 시간 측정
			StartInspection();
			endTime = clock();			// 종료 시간 측정
			runTime = (double)(endTime - startTime);
			printf("Inspection Time : %.2f\n", runTime / CLOCKS_PER_SEC);
			break;
		case INSP_STOP:
			inspContinue = false;
			printf("프로그램을 종료합니다.\n");
			break;
		default:
			printf("잘못 선택하셨습니다.\n");
		}
	}
}

void CInspectionHandler::ShowMenu() const
{
	printf("=================\n");
	printf("1. 검사 시작\n");
	printf("2. 프로그램 종료\n");
	printf("=================\n");
	printf("메뉴 입력: ");
}

void CInspectionHandler::StartInspection()
{
	printf("Inspection started.\n");

	for (int i_Ptn = 0; i_Ptn < m_nTotalPtnCnt; i_Ptn++)
	{
		// 1. 파라미터 세팅
		m_CameraHandler->SetCamParams(m_Patterns[i_Ptn].GetShutterSpeed());

		// 2. 촬상
		m_CameraHandler->Grab(m_Patterns[i_Ptn].GetPtnName());
				
		// 3. 알고리즘 수행
		for (int i_Alg = 0; i_Alg < m_nTotalAlgCnt; i_Alg++)
		{
			if (m_Algorithms[i_Alg].CheckAlgPtn(m_Patterns[i_Ptn].GetPtnName()))	// 검사하는 알고리즘이라면
			{
				m_Algorithms[i_Alg].RunAlgorithm();	// 알고리즘 수행

				if (m_Algorithms[i_Alg].GetAlgResult())
				{
					printf("Ptn[%s] : Alg[%s] OK.\n",
						m_Patterns[i_Ptn].GetPtnName(), m_Algorithms[i_Alg].GetAlgName());
				}
				else
				{
					printf("Ptn[%s] : Alg[%s] NG.\n",
						m_Patterns[i_Ptn].GetPtnName(), m_Algorithms[i_Alg].GetAlgName());
				}
			}
		}
	}
	printf("Inspection completed.\n");
}

bool CInspectionHandler::ReadConfig()
{	
	char secName[1024];
	char buf[1024];

	// Pattern Load
	m_nTotalPtnCnt = GetPrivateProfileIntA("general", "total pattern count", 0, ".\\config.ini");

	if (0 == m_nTotalPtnCnt)
		return false;

	m_Patterns = new CPatternData[m_nTotalPtnCnt];

	for (int i_Ptn = 0; i_Ptn < m_nTotalPtnCnt; i_Ptn++)
	{
		sprintf(secName, "Pattern%02d", i_Ptn);
		
		GetPrivateProfileStringA(secName, "pattern name", "", buf, sizeof(buf), ".\\config.ini");	// 패턴 이름 읽기
		
		if (0 == strcmp(buf, ""))
			return false;

		m_Patterns[i_Ptn].SetPtnName(buf);	// 패턴 이름 설정

		m_Patterns[i_Ptn].SetShutterSpeed(GetPrivateProfileIntA(secName, "shutter speed", 0, ".\\config.ini")); // 셔터 스피드 읽기 및 설정
	}


	// Algorithm Load
	m_nTotalAlgCnt = GetPrivateProfileIntA("general", "total algorithm count", 0, ".\\config.ini");
	
	if (0 == m_nTotalAlgCnt) 
		return false;

	m_Algorithms = new CAlgorithmData[m_nTotalAlgCnt];

	for (int i_Alg = 0; i_Alg < m_nTotalAlgCnt; i_Alg++)
	{
		sprintf(secName, "Algorithm%02d", i_Alg);

		GetPrivateProfileStringA(secName, "algorithm name", "", buf, sizeof(buf), ".\\config.ini");	// 알고리즘 이름 읽기

		if (0 == strcmp(buf, ""))
			return false;

		m_Algorithms[i_Alg].SetAlgName(buf);	// 알고리즘 이름 설정

		GetPrivateProfileStringA(secName, "pattern", "", buf, sizeof(buf), ".\\config.ini");	// 알고리즘이 사용되는 패턴 읽기

		if (0 == strcmp(buf, ""))
			return false;

		m_Algorithms[i_Alg].SetAlgPtnInfo(buf);	// 알고리즘이 사용되는 패턴 설정
	}

	return true;
}