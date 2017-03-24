#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#define SafeRelease(pObject) if (pObject != NULL) { pObject -> Release(); pObject = NULL; }
#define ITEMS_NUM 10

static LPDIRECTINPUT8 g_pIDirectInput = NULL;
extern HWND hWnd;

class CGameEngine_Input {

private:

	LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;
	LPDIRECTINPUTDEVICE8 m_pMouseDevice;
	char m_KeyBuffer[256];
	DIDEVICEOBJECTDATA m_MouseData[ITEMS_NUM];
	int MouseX , MouseY; //菲公 X , Y 畒夹

public:
	
	long m_lMouseMoveX,m_lMouseMoveY;
	CGameEngine_Input();
	~CGameEngine_Input();
	bool CreateInput(HINSTANCE, HWND, int iMin = -100, int iMax = 100, int iDeadZone = 20);
	bool ReadKeyboard();
	bool ReadMouse();
	bool IsKeyPressed(int key); // 耞ノめ龄ㄧ计
	inline long GetMouseMoveX() { return m_lMouseMoveX; }
	inline long GetMouseMoveY() { return m_lMouseMoveY; }
	int GetMouseX(HWND hWnd);
	int GetMouseY(HWND hWnd);
	bool IsLButtonPressed(); // 耞菲公オ龄
	bool IsLButtonUp(); // 耞菲公オ龄秨
	bool IsRButtonPressed(); // 耞菲公龄
	bool IsMButtonPressed(); // 耞菲公い丁簎近
	void InputRelease();

private:
	
	bool CreateDirectInput(HINSTANCE);
	bool CreateKeyboard(HWND);
	bool CreateMouse(HWND);
};