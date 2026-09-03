#pragma once //해당 헤더 파일이 한 번만 포함

//아래 포함된 헤더파일에서 쓰일 함수들
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
using namespace std;

//순서 중요
#include "CAlgorithmData.h"
#include "CCameraHandler.h"
#include "CPatternData.h"

//클래스 정의
class CInspectionHandler {
private:
    CPatternData PatternList[10]; // CPatternData 객체 배열
    CAlgorithmData AlgorithmList[10]; // CAlgorithmData 객체 배열
    CCameraHandler CameraHandler; // CCameraHandler 객체
    int TotalPatternCount; // 총 검사 패턴 개수
    int TotalAlgorithmCount; // 총 검사 알고리즘 개수

    //상대 경로로 파일 불러오기
    const char* iniPath = ".\\config.ini"; 

public:
    void Initialize(); // 클래스 초기화
    bool LoadConfig(); // config 파일 읽고 정보 로드
    void ShowMenu(); // 메뉴 출력
    void RunInspection(); // 검사 수행

};

void CInspectionHandler::Initialize() {
    CameraHandler.ConnectCam(); // 카메라 연결
    if (!LoadConfig()) { // config 파일 찾기 실패 시
        printf("Config 파일 로드 실패\n");
        return;
    }
    while (true) {
        ShowMenu(); // 메뉴 출력
        int sel; // 입력 받은 숫자 저장할 변수
        if (scanf_s("%d", &sel) != 1) { // 입력 받은 정수값 sel에 저장
            while (getchar() != '\n'); // 1, 2외의 숫자 입력 시 초기화
            continue; // 다시 메뉴 선택
        }
        if (sel == 1) { // 1 선택 시 프로그램 시작
            RunInspection();
        }
        else if (sel == 2) { // 2 선택 시 프로그램 종료
            printf("프로그램을 종료합니다.\n");
            CameraHandler.CheckCamOn(); // 카메라 연결 상태 체크
            CameraHandler.DisconnectCam(); // 카메라 연결 해제
            break;
        }
    }
}

// config 파일 읽고 저장
bool CInspectionHandler::LoadConfig() {
    char buffer[256] = { 0 }; // ini 파일에서 읽어 온 문자열 저장

    // 총 패턴 개수 저장 (general 섹션에서 total pattern count 키 값을 읽고 복사)
    GetPrivateProfileStringA("general", "total pattern count", "0", buffer, sizeof(buffer), iniPath);
    TotalPatternCount = atoi(buffer); // 문자열을 정수로 변환
    if (TotalPatternCount > 10) // 최대 10개로 제한
        TotalPatternCount = 10;

    // 총 알고리즘 개수 저장 (general 섹션에서 total algorithm count 키 값을 읽고 복사)
    GetPrivateProfileStringA("general", "total algorithm count", "0", buffer, sizeof(buffer), iniPath);
    TotalAlgorithmCount = atoi(buffer);
    if (TotalAlgorithmCount > 10)
        TotalAlgorithmCount = 10;

    // 패턴 섹션과 키, 값 불러와서 저장
    for (int i = 0; i < TotalPatternCount; ++i) {
        char section[32]; // 패턴 섹션 이름 저장할 변수
        sprintf_s(section, "pattern%02d", i); // 0~총 패턴 개수 만큼

        //해당하는 섹션의 패턴 이름을 읽어서 PatternList[i]의 이름에 저장
        GetPrivateProfileStringA(section, "pattern name", "", buffer, sizeof(buffer), iniPath);
        PatternList[i].PNameSet(buffer);

        //해당하는 섹션의 셔터 스피드 값을 읽어서 정수 변환 후 PatternList[i]의 셔터 스피드에 저장
        GetPrivateProfileStringA(section, "shutter speed", "1", buffer, sizeof(buffer), iniPath);
        PatternList[i].PSpeedSet(atoi(buffer));
    }

    // 알고리즘 섹션과 키, 값 불러와서 저장
    for (int i = 0; i < TotalAlgorithmCount; ++i) {
        char section[32]; // 알고리즘 섹션 이름 저장할 변수
        sprintf_s(section, "algorithm%02d", i); // 0~총 알고리즘 개수 만큼

        // 해당하는 섹션의 알고리즘 이름을 읽어서 AlgorithmList[i]의 이름에 저장
        GetPrivateProfileStringA(section, "algorithm name", "", buffer, sizeof(buffer), iniPath);
        AlgorithmList[i].SetNameAlgorithm(buffer);

        // 해당하는 섹션의 알고리즘에 사용되는 패턴 이름을 읽어서 각 패턴 이름들을 분리 저장
        GetPrivateProfileStringA(section, "pattern", "", buffer, sizeof(buffer), iniPath);
        int pCount = 1; // 알고리즘에 사용 되는 패턴 개수
        // 쉼표가 몇 개인지 세어 패턴 개수 결정 및 이름 분리 후 저장
        for (char* p = buffer; *p; ++p)
            if (*p == ',') pCount++;
        if (pCount > 10) pCount = 10; // 최대 10개 제한
        AlgorithmList[i].PatternSet(buffer, pCount);
    }

    return true; // ini 파싱 후 성공 반환
}

//메뉴 선택창 출력
void CInspectionHandler::ShowMenu() {
    printf("=================\n");
    printf("1. 검사 시작\n");
    printf("2. 프로그램 종료\n");
    printf("=================\n");
    printf("메뉴 입력: ");
}
void CInspectionHandler::RunInspection() {
    printf("Inspection started.\n"); // 검사 시작

    // 검사 시작 시점의 시간 저장
    clock_t StartTime = clock();

    // 총 패턴 개수 만큼 반복 및 검사할 각 패턴을 순서대로 선택
    for (int i = 0; i < TotalPatternCount; ++i) {
        // 현재 패턴 이름 불러오기
        char* CurrentPattern = PatternList[i].PNameReturn();

        // 현재 패턴의 셔터 스피드 값 불러와 설정
        CameraHandler.SetCamParams(PatternList[i].PSpeedReturn());

        // 현재 패턴에 대해 촬영
        CameraHandler.Grab(CurrentPattern);

        // 모든 알고리즘에 대해 반복 및 알고리즘과 현재 패턴 관련 여부 확인
        for (int j = 0; j < TotalAlgorithmCount; ++j) {
            // 검사할 알고리즘에 현재 선택된 패턴 이름이 있는 지 확인
            if(AlgorithmList[j].CheckAlgorithmPattern((char*)CurrentPattern)) {
                // 있으면 현재 패턴 이름을 갱신하고
                AlgorithmList[j].SelectPatternName();
                // 선택된 패턴에 대해 알고리즘 검사 작업을 수행하도록 호출
                AlgorithmList[j].RunAlgorithm(CurrentPattern);

                //패턴 이름과 알고리즘 이름, 검사 결과를 출력
                printf("Ptn[%s] : Alg[%s] %s.\n",
                    CurrentPattern,
                    AlgorithmList[j].ReturnNameAlgorithm(),
                    AlgorithmList[j].AlgorithmResult());
            }
        }
    }

    // 검사 완료 시점의 시간 저장
    clock_t EndTime = clock();
    // 검사 완료 시점에서 시작 시점을 빼서 초 단위로 계산해서 저장
    double TotalTime = (double)(EndTime - StartTime) / CLOCKS_PER_SEC;

    printf("Inspection completed.\n");
    printf("Inspection Time : %.2f\n", TotalTime);
}