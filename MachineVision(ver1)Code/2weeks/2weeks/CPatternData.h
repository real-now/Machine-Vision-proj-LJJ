#pragma once

// 클래스 정의
class CPatternData {
private:
	char PName[100]; // 패턴 이름
	int PSpeed; // 패턴 이름에 해당하는 셔터 스피드

public:
	void PNameSet(char* Name); // 패턴 이름 설정
	char* PNameReturn(); // 패턴 이름 반환
	void PSpeedSet(int Speed); // 패턴 촬영 시 필요한 셔터 스피드 설정
	int PSpeedReturn(); // 패턴 촬영 시 필요한 셔터 스피드 반환
};

void CPatternData::PNameSet(char* Name) {
	// 전달받은 패턴 이름을 배열에 저장
	strcpy_s(PName, sizeof(PName), Name);
}
char* CPatternData::PNameReturn() {
	return PName; // 패턴 이름 반환
}
void CPatternData::PSpeedSet(int Speed) {
	// 전달받은 패턴의 셔터 스피드 값을 저장
	PSpeed = Speed;
}
int CPatternData::PSpeedReturn() {
	// 셔터 스피드 값 반환
	return PSpeed;
}