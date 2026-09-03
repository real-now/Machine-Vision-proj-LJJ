#include "CInspectionHandler.h"
#include <cstdio>
#include <windows.h>
#include <time.h>
#include <thread>
#include <string>
#include <chrono>

// 생성자
CInspectionHandler::CInspectionHandler()
	:m_Patterns(NULL), m_Algorithms(NULL), m_CameraHandler(NULL), m_nTotalPtnCnt(0), m_nTotalAlgCnt(0),
	m_nTotalFinAlgCnt(0), m_nExpectedJobs(0), m_bFinInspection(false)
{
}

// 소멸자
CInspectionHandler::~CInspectionHandler()
{
	JoinWorkers();		// 남아있는 쓰레드 정리

	if (m_Patterns)
	{
		delete[] m_Patterns;
		m_Patterns = NULL;
	}

	if (m_Algorithms)
	{
		delete[] m_Algorithms;
		m_Algorithms = NULL;
	}

	if (m_CameraHandler)
	{
		if (m_CameraHandler->CheckCamOn())
			m_CameraHandler->DisconnectCam();	// 카메라가 연결되어 있으면 카메라 연결 종료
		delete m_CameraHandler;
		m_CameraHandler = NULL;
	}
}

// 이전 검사에서 생성된 쓰레드를 안전하게 정리
// std::thread는 joinable한 상태에서 대입하거나 소멸하면 프로그램이 종료되므로
// 새 검사를 시작하기 전에 반드시 호출해야 한다.
void CInspectionHandler::JoinWorkers()
{
	if (!m_algThreads.empty())
	{
		// 아직 대기 중인 워커가 있으면 깨워서 종료시키기 위해 센티넬 추가
		std::lock_guard<std::mutex> lk(m_jobMutex);
		for (int k = 0; k < m_nTotalAlgCnt; k++)
			m_AlgJobQ.push(std::string{});		// 빈 문자열 센티넬
	}

	for (size_t i = 0; i < m_algThreads.size(); i++)
	{
		if (m_algThreads[i].joinable())
			m_algThreads[i].join();
	}
	m_algThreads.clear();

	if (m_camThread.joinable())
		m_camThread.join();

	// 남아있는 센티넬 제거
	std::lock_guard<std::mutex> lk(m_jobMutex);
	while (!m_AlgJobQ.empty()) m_AlgJobQ.pop();
}

void CInspectionHandler::Initialize()
{
	bool inspContinue = true;

	if (false == ReadConfig())
	{
		printf("Config 파일을 확인하세요.\n");
		return;
	}

	// 알고리즘 개수를 알게 된 후 크기에 맞춰서 생성
	m_AlgLocks.reset(new std::mutex[m_nTotalAlgCnt]);

	m_CameraHandler = new CCameraHandler;	// 카메라 핸들러 객체 생성
	m_CameraHandler->ConnectCam();			// 카메라 연결	

	while (inspContinue)
	{
		int menuNum;

		ShowMenu();
		scanf_s("%d", &menuNum);

		switch (menuNum)
		{
		case INSP_START: {
			clock_t startTime, endTime;
			double runTime;
			startTime = clock();		// 시작 시간 측정
			StartInspection();

			// 검사가 종료될 때까지 대기 (조건 변수로 대기해 CPU 점유 없음)
			{
				std::unique_lock<std::mutex> finLock(m_FinInspMutex);
				m_FinInspCV.wait(finLock, [this] { return m_bFinInspection.load(); });
			}

			endTime = clock();			// 종료 시간 측정
			runTime = (double)(endTime - startTime);
			printf("Inspection Time : %.2f\n", runTime / CLOCKS_PER_SEC);
			break;
		}
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

int CInspectionHandler::FindAlgIndexByName(const char* name) const {
	// 알고리즘 이름 기반으로 배열 인덱스 찾기
	for (int i = 0; i < m_nTotalAlgCnt; i++) {
		if (0 == strcmp(name, m_Algorithms[i].GetAlgName()))
			return i;
	}
	return -1; // 없으면 -1 출력
}

void CInspectionHandler::StartInspection()
{
	JoinWorkers();		// 이전 검사의 쓰레드를 먼저 정리 (재실행 시 크래시 방지)

	printf("Inspection started.\n");

	// 총 작업 개수(패턴, 알고리즘 매칭 수) 계산
	int expected = 0;
	for (int i_Ptn = 0; i_Ptn < m_nTotalPtnCnt; i_Ptn++) {
		const char* ptn = m_Patterns[i_Ptn].GetPtnName();
		for (int i = 0; i < m_nTotalAlgCnt; i++) {
			if (m_Algorithms[i].CheckAlgPtn(ptn))
				expected++;
		}
	}

	// 검사 상태 초기화
	{
		std::lock_guard<std::mutex> lk(m_TotalFinAlgCntMutex);
		m_nTotalFinAlgCnt = 0;
		m_nExpectedJobs = expected;
	}

	{
		std::lock_guard<std::mutex> finLock(m_FinInspMutex);
		m_bFinInspection = (expected == 0);	// 작업이 0개면 즉시 완료 처리
	}

	if (expected == 0)
	{
		m_FinInspCV.notify_one();
		return;
	}

	// 카메라 스레드 시작 (멤버에 저장)
	m_camThread = std::thread(&CInspectionHandler::CameraWorker, this);

	// 알고리즘 워커 N개 시작 (멤버 벡터에 저장)
	m_algThreads.reserve(m_nTotalAlgCnt);
	for (int i = 0; i < m_nTotalAlgCnt; i++)
		m_algThreads.emplace_back(&CInspectionHandler::AlgWorker, this);
}

void CInspectionHandler::CameraWorker() {
	for (int i_Ptn = 0; i_Ptn < m_nTotalPtnCnt; i_Ptn++)
	{
		// 파라미터 세팅
		m_CameraHandler->SetCamParams(m_Patterns[i_Ptn].GetShutterSpeed());

		// 촬상
		m_CameraHandler->Grab(m_Patterns[i_Ptn].GetPtnName());

		// 이 패턴에 적용되는 알고리즘마다 작업 생성
		const char* ptn = m_Patterns[i_Ptn].GetPtnName();
		for (int i = 0; i < m_nTotalAlgCnt; i++) {
			if (m_Algorithms[i].CheckAlgPtn(ptn)) {
				// "AlgName|PtnName" 형태로 저장
				std::string job = std::string(m_Algorithms[i].GetAlgName()) + "|" + ptn;

				std::lock_guard<std::mutex> lk(m_jobMutex);
				m_AlgJobQ.push(job);
			}
		}
	}
	{ // 워커 종료용 센티넬(상태를 알리는 신호; 종료 토큰)
		std::lock_guard<std::mutex> lk(m_jobMutex);
		// 카메라 작업이 모든 패턴을 끝낸 뒤, 알고리즘 쓰레드들에게 종료 신호
		// 빈 문자열을 센티넬로 사용함
		for (int k = 0; k < m_nTotalAlgCnt; k++)
			m_AlgJobQ.push(std::string{});
	}
}

void CInspectionHandler::AlgWorker() {
	while (true) {
		std::string job;	// 이번에 처리할 작업

		// 10ms 폴링: 비었으면 unlock, 10ms 대기 후 재시도
		m_jobMutex.lock();
		if (m_AlgJobQ.empty()) {
			m_jobMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// 저장한 값 하나 꺼내기
		job = m_AlgJobQ.front();
		m_AlgJobQ.pop();
		m_jobMutex.unlock();

		// 종료 센티넬(빈 문자열) 처리
		// 빈 문자열을 받으면 종료
		if (job.empty())
			break;

		// "AlgName|PtnName"에서 | 문자 찾기
		size_t bar = job.find('|');
		if (bar == std::string::npos) {
			// 형식 오류 확인 시
			continue;
		}
		std::string algName = job.substr(0, bar);
		std::string ptnName = job.substr(bar + 1);

		// 알고리즘 인덱스 찾기
		int idx = FindAlgIndexByName(algName.c_str());
		if (idx < 0) {
			// 이름 못 찾으면 건너뜀
			continue;
		}

		// 동시 실행 보호 (같은 알고리즘 객체를 두 쓰레드가 동시에 쓰지 않도록)
		{
			std::lock_guard<std::mutex> g(m_AlgLocks[idx]);
			m_Algorithms[idx].RunAlgorithm();

			printf("Ptn[%s] : Alg[%s] %s\n",
				ptnName.c_str(),
				m_Algorithms[idx].GetAlgName(),
				m_Algorithms[idx].GetAlgResult() ? "OK" : "NG");
		}

		// 작업 1건 완료 증가 및 완료 판정
		bool bAllDone = false;
		{
			std::lock_guard<std::mutex> lk(m_TotalFinAlgCntMutex);
			m_nTotalFinAlgCnt++;

			if (m_nTotalFinAlgCnt == m_nExpectedJobs)
				bAllDone = true;
		}

		if (bAllDone)
		{
			printf("Inspection completed.\n");
			{
				std::lock_guard<std::mutex> finLock(m_FinInspMutex);
				m_bFinInspection = true;
			}
			m_FinInspCV.notify_one();	// 대기 중인 메인 쓰레드 깨우기
		}
	}
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
		sprintf_s(secName, sizeof(secName), "Pattern%02d", i_Ptn);

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
		sprintf_s(secName, sizeof(secName), "Algorithm%02d", i_Alg);

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