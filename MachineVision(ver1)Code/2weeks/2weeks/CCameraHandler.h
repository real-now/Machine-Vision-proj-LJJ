#pragma once

// 클래스 정의
class CCameraHandler {
private:
	int ShutterSpeed; // 카메라 셔터 스피드
	bool ConnectState; // 카메라 연결 상태

public:
	void ConnectCam(); // 카메라 연결
	void SetCamParams(int speed); // 카메라 셔터 스피드 설정
	void Grab(char* Pattern); // 촬상
	void DisconnectCam(); // 카메라 연결 해제
	void CheckCamOn(); // 카메라 연결 상태 확인
};

void CCameraHandler::ConnectCam() { // 호출 시 카메라 연결
	cout << "CCameraHandler::ConnectCam called." << endl;
	ConnectState = true; // 카메라 연결 상태
	Sleep(1000); // 1000ms = 1초 동안 대기
	cout << "CCameraHandler::ConnectCam finished." << endl;
}
void CCameraHandler::SetCamParams(int speed) { // 전달받은 셔터 스피드로
	ShutterSpeed = speed; // 현재 셔터 스피드 설정
	cout << "CCameraHandler::SetCamParams called(" << ShutterSpeed << "s)." << endl;
	Sleep(1000);
	cout << "CCameraHandler::SetCamParams finished(" << ShutterSpeed << "s)." << endl;
}
void CCameraHandler::Grab(char* Pattern) { // 전달받은 패턴 이름으로
	// 현재 패턴 이름 출력을 위해 사용
	cout << "CCameraHandler::Grab called(" << Pattern << ")." << endl;
	Sleep(ShutterSpeed * 1000); // 현재 패턴에 해당하는 셔터 스피드 만큼 지연
	cout << "CCameraHandler::Grab finished(" << Pattern << ")." << endl;
}
void CCameraHandler::DisconnectCam() { // 호출 시 카메라 연결 해제
	cout << "CCameraHandler::DisconnectCam called." << endl;
	ConnectState = false; //카메라 연결 해제 상태
	Sleep(1000);
}
void CCameraHandler::CheckCamOn() { // 현재 카메라 연결 상태 확인
	cout << "CCameraHandler::CheckCamOn called." << endl;
}

