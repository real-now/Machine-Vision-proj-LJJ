#pragma once
#include "CPatternData.h"
#include "CAlgorithmData.h"
#include "CCameraHandler.h"
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>

enum UserMenu { INSP_START = 1, INSP_STOP };

class CInspectionHandler
{
private:
	std::queue<std::string> m_AlgJobQ;			// 알고리즘 작업 큐 (알고리즘 이름|패턴 이름 형식으로 저장)
	std::mutex m_jobMutex;						// 큐 전용 뮤텍스
	std::unique_ptr<std::mutex[]> m_AlgLocks;	// 알고리즘 쓰레드 전용 뮤텍스
	std::mutex m_TotalFinAlgCntMutex;			// 검사를 완료한 총 알고리즘 개수 Mutex

	std::mutex m_FinInspMutex;					// 검사 종료 통지용 Mutex
	std::condition_variable m_FinInspCV;		// 검사 종료 통지용 조건 변수

	// 소멸자에서 join하기 위해 멤버로 만듦
	std::thread m_camThread;                 // 카메라 스레드
	std::vector<std::thread> m_algThreads;   // 알고리즘 워커 스레드들

	CPatternData* m_Patterns;			// 패턴 정보 객체 포인터
	CAlgorithmData* m_Algorithms;		// 검사 알고리즘 정보 객체 포인터
	CCameraHandler* m_CameraHandler;	// 카메라 제어 객체 포인터

	int m_nTotalPtnCnt;					// 총 검사 패턴 개수
	int m_nTotalAlgCnt;					// 총 검사 알고리즘 개수
	int m_nTotalFinAlgCnt;				// (알고리즘 쓰레드) 검사를 완료한 총 알고리즘 개수
	int m_nExpectedJobs;				// 총 알고리즘 작업 개수
	std::atomic<bool> m_bFinInspection;	// 검사 종료 여부

	int FindAlgIndexByName(const char* name) const;	// 이름으로 알고리즘 인덱스 찾기
	void JoinWorkers();								// 이전 검사에서 생성된 쓰레드 정리

public:
	CInspectionHandler();
	~CInspectionHandler();

	void Initialize();					// 클래스 초기화 및 시작하는 함수
	void ShowMenu() const;				// 검사 메뉴 출력하는 함수
	void StartInspection();				// 검사 시작하는 함수
	bool ReadConfig();					// Config 파일 읽는 함수
	void CameraWorker();				// 카메라 쓰레드가 실행할 함수
	void AlgWorker();					// 알고리즘 쓰레드가 실행할 함수
};