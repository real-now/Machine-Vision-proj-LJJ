#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void PrintIniFile(const char* iniPath) {
    char sectionNames[4096] = { 0 };
    // 모든 섹션 이름 읽기
    DWORD sectionsLen = GetPrivateProfileSectionNamesA(
        sectionNames, sizeof(sectionNames), iniPath);

    // 섹션 이름은 '\0'으로 구분, 마지막은 '\0\0'
    char* section = sectionNames;
    while (*section) {
        cout << "[" << section << "]" << endl;

        // 각 섹션의 모든 키=값 읽기
        char keyValues[4096] = { 0 };
        GetPrivateProfileSectionA(section, keyValues, 
            sizeof(keyValues), iniPath);

        char* keyValue = keyValues;
        while (*keyValue) {
            cout << keyValue << endl;
            keyValue += strlen(keyValue) + 1;
        }
        cout << endl;
        section += strlen(section) + 1;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8); //유니코드(한국어)를 출력하기 위해 사용

    const char* iniPath = ".\\test1.ini"; //상대 경로로 실행
    PrintIniFile(iniPath); //해당 ini파일 실행

    system("pause"); //exe 파일에서 확인하기 위한 시스템 정지

    return 0;
}