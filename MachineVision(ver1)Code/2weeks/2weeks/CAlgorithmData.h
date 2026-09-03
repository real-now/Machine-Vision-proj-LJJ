#pragma once

// 클래스 정의
class CAlgorithmData {
private:
	char AlgorithmName[100]; // 알고리즘 이름
	int PatternCount; // 알고리즘이 사용되는 패턴 개수
	char PatternName[10][50]; // 알고리즘이 사용되는 패턴 이름들
	bool AlgorithmResultFlag; // 알고리즘 검사 결과
	char SelectPName[50]; // 현재 검사 중인 패턴 이름

public:
	void SetNameAlgorithm(char* AlName); // 알고리즘 이름 설정
	char* ReturnNameAlgorithm(); // 알고리즘 이름 반환
	void PatternSet(char* Pattern, int PCount); // 알고리즘이 사용되는 패턴 개수와 이름 저장
	void SelectPatternName(); // 현재 검사할 패턴 이름 설정
	char* ReturnSelectPName(); // 현재 검사 중인 패턴 이름 반환
	bool CheckAlgorithmPattern(char* PName); // 알고리즘이 사용되는 패턴인지 검사
	const char* AlgorithmResult(); // 알고리즘 검사 결과 반환
	void RunAlgorithm(char* PatternName); // 알고리즘 수행
};

void CAlgorithmData::SetNameAlgorithm(char* AlName) {
	// 전달받은 알고리즘 이름을 배열에 저장
	strcpy_s(AlgorithmName, sizeof(AlgorithmName), AlName);
}
char* CAlgorithmData::ReturnNameAlgorithm() {
	// 알고리즘 이름 반환
	return AlgorithmName;
}

// 전달받은 Pattern 포인터는 쉼표로 구분된 패턴 이름들이 연속된 문자열
void CAlgorithmData::PatternSet(char* Pattern, int PCount) {
	// 최대 10개 패턴 제한
	PatternCount = (PCount > 10) ? 10 : PCount;
	int index = 0; // 2차 배열의 가로줄 번호
	int pos = 0; // 2차 배열의 세로줄 번호
	int len = (int)strlen(Pattern); // 전달받은 패턴의 총 크기

	// 쉼표로 구분된 패턴 이름을 나누기 위한 부분
	for (int i = 0; i <= len && index < PatternCount; i++) {
		// 쉼표이거나 널문자인 경우 다음 패턴 이름 저장
		if (Pattern[i] == ',' || Pattern[i] == '\0') {
			PatternName[index][pos] = '\0'; // 문자열의 끝인 널문자 삽입
			index++; // 다음 가로줄
			pos = 0; // 세로줄 위치 초기화
		}
		// 그 외의 경우 문자 저장
		else {
			PatternName[index][pos++] = Pattern[i];
		}
	}
}

// 이 함수를 호출 할 때마다 PatternName 배열에 저장된 패턴 이름을 하나씩 SelectPName에 저장
void CAlgorithmData::SelectPatternName() {
	static int P_index = 0;  // 현재 선택된 패턴 인덱스

	if (PatternCount == 0) {
		SelectPName[0] = '\0';  // 패턴이 없으면 빈 문자열
		return;
	}

	// 인덱스가 범위를 벗어나면 초기화
	if (P_index >= PatternCount) {
		P_index = 0;
	}

	// 현재 인덱스 위치에 있는 패턴 이름을 현재 선택한 패턴 이름에 복사
	strcpy_s(SelectPName, sizeof(SelectPName), PatternName[P_index++]);
}
char* CAlgorithmData::ReturnSelectPName() {
	return SelectPName; // 선택된 패턴 이름 반환
}

// 전달받은 패턴 이름이 현재 알고리즘에 맞는지 확인
bool CAlgorithmData::CheckAlgorithmPattern(char* PName) {
	for (int i = 0; i < PatternCount; ++i) {
		if (strcmp(PatternName[i], PName) == 0) {
			return true;	// 이 알고리즘은 PName 패턴을 사용
		}
	}
	return false;	// 사용하지 않음
}

// 알고리즘 검사 결과를 문자열로 반환
const char* CAlgorithmData::AlgorithmResult() {
	return AlgorithmResultFlag ? "OK" : "NG";
}

// 실제 알고리즘 수행을 시뮬레이션
void CAlgorithmData::RunAlgorithm(char* PatternName) {
	printf("CAlgorithmData::RunAlgorithm(%s,%s) called.\n", PatternName, AlgorithmName);
	Sleep(1000);
	AlgorithmResultFlag = (rand() % 2 == 0) ? false : true; // 검사 결과는 랜덤으로 저장
	printf("CAlgorithmData::RunAlgorithm(%s,%s) finished.\n", PatternName, AlgorithmName);
}