#pragma once
#include "CPatternData.h"
#include "CAlgorithmData.h"
#include "CCameraHandler.h"

enum UserMenu { INSP_START = 1, INSP_STOP };

class CInspectionHandler
{
private:
	CPatternData* m_Patterns;			// 패턴 정보 객체 포인터
	CAlgorithmData* m_Algorithms;		// 검사 알고리즘 정보 객체 포인터
	CCameraHandler* m_CameraHandler;	// 카메라 제어 객체 포인터
	
	int m_nTotalPtnCnt;					// 총 검사 패턴 개수
	int m_nTotalAlgCnt;					// 총 검사 알고리즘 개수

public:
	CInspectionHandler();
	~CInspectionHandler();

	void Initialize();					// 클래스 초기화 및 시작하는 함수
	void ShowMenu() const;				// 검사 메뉴 출력하는 함수
	void StartInspection();				// 검사 시작하는 함수
	bool ReadConfig();					// Config 파일 읽는 함수
};

