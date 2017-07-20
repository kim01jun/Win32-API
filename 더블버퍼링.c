#include <Windows.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
HINSTANCE hInst;;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	hInst = hInstance;
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//출력 형태
	WndClass.lpfnWndProc = WndProc;	//메시지 처리를 위한 함수 이름 전달
	WndClass.cbClsExtra = 0;	//여분의 클래스메모리 (사용 X)
	WndClass.cbWndExtra = 0;	//여분의 윈도우메모리 (사용 X)
	WndClass.hInstance = hInstance;	//메모리상에서 프로그램의 위치 전달
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//아이콘모양
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	//커서모양
	WndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));	//배경색
	WndClass.lpszMenuName = NULL;	//메뉴
	WndClass.lpszClassName = _T("Windows Class Name");	//윈도우 클래스이름
	RegisterClass(&WndClass);	//커널에 등록
	hwnd = CreateWindow(
		_T("Windows Class Name"),	//윈도우 클래스이름
		_T("API Studying"),	//윈도우 타이틀이름
		WS_OVERLAPPEDWINDOW,	//윈도우스타일 (타이틀바의 최소화, 최대화, 닫기) OR기호로 여러개 지정가능
		CW_USEDEFAULT,		//x좌표
		CW_USEDEFAULT,		//y좌표
		800,		//윈도우 가로크기
		500,		//윈도우 세로크기	CW_USEDEFAULT는 커널에 의해 기본값을 지정
		NULL,	// 부모 윈도우 핸들
		NULL,	// 메뉴 핸들
		hInstance,		//응용 프로그램 인스턴스
		NULL		//생성 윈도우 정보
	);
	ShowWindow(hwnd, nCmdShow);	//(나타낼 윈도우 핸들값, 윈도우를 화면에 나타내는 방법으로 상수값 제공 ex)SW_MAXIMIZE)
	UpdateWindow(hwnd);	//윈도우 화면에 기본 출력하기
	while (GetMessage(&msg, NULL, 0, 0))	//메시지큐에서 MSG구조체변수에 저장
	{
		TranslateMessage(&msg);		//변환?
		DispatchMessage(&msg);		//메시지를 처리하는 함수에 메시지를 보냄
	}

	return (int)msg.wParam;
}

void TextPrint(HDC hdc, int x, int y, TCHAR text[])
{
	int i, j;
	SetTextColor(hdc, RGB(255, 255, 255));
	for (i =-1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
			TextOut(hdc, x + i, y + j, text, _tcslen(text));
	SetTextColor(hdc, RGB(0, 0, 0));
	TextOut(hdc, x, y, text, _tcslen(text));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc, memdc1, memdc2;
	PAINTSTRUCT ps;
	static HBITMAP hBit1, hBit2, oldBit1, oldBit2;
	static RECT rectView;
	static int yPos;
	TCHAR str[] = _T("더블버퍼링 연습할게요 ㅎㅎ");

	switch (iMsg)
	{
	case WM_CREATE:	//윈도우가 만들어졌을때
		yPos = -30;
		GetClientRect(hwnd, &rectView);
		SetTimer(hwnd, 1, 10, NULL);
		hBit2 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		break;

	case WM_PAINT:
		GetClientRect(hwnd, &rectView);
		hdc = BeginPaint(hwnd, &ps);

		memdc1 = CreateCompatibleDC(hdc);
		memdc2 = CreateCompatibleDC(memdc1);

		if (hBit1 == NULL)
			hBit1 = CreateCompatibleBitmap(hdc, 800, 500);

		oldBit1 = (HBITMAP)SelectObject(memdc1, hBit1);
		oldBit2 = (HBITMAP)SelectObject(memdc2, hBit2);

		BitBlt(memdc1, 0, 0, 800, 500, memdc2, 0, 0, SRCCOPY);
		SetBkMode(memdc1, TRANSPARENT);		//투명한 배경색 사용 SetBackGroundMode
		TextPrint(memdc1, 320, yPos, str);
		BitBlt(hdc, 0, 0, 800, 500, memdc1, 0, 0, SRCCOPY);

		SelectObject(memdc1, oldBit1);
		SelectObject(memdc2, oldBit2);
		DeleteDC(memdc1);
		DeleteDC(memdc2);
		EndPaint(hwnd, &ps);
		break;

	case WM_TIMER:
		yPos += 1;
		if (yPos > rectView.bottom) yPos = -30;
		InvalidateRgn(hwnd, NULL, FALSE);
		break;

	case WM_DESTROY:	//윈도우가 꺼졌을때
		if (hBit1) DeleteObject(hBit1);
		DeleteObject(hBit2);
		KillTimer(hwnd, 1);
		PostQuitMessage(0);	//GetMessage함수가 0을 반환하게함
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);	//나머지는 커널에서 처리
}