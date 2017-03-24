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
	int MouseX , MouseY; //�ƹ��� X , Y �y��

public:
	
	long m_lMouseMoveX,m_lMouseMoveY;
	CGameEngine_Input();
	~CGameEngine_Input();
	bool CreateInput(HINSTANCE, HWND, int iMin = -100, int iMax = 100, int iDeadZone = 20);
	bool ReadKeyboard();
	bool ReadMouse();
	bool IsKeyPressed(int key); // �P�_�Τ������
	inline long GetMouseMoveX() { return m_lMouseMoveX; }
	inline long GetMouseMoveY() { return m_lMouseMoveY; }
	int GetMouseX(HWND hWnd);
	int GetMouseY(HWND hWnd);
	bool IsLButtonPressed(); // �P�_�ƹ�������U
	bool IsLButtonUp(); // �P�_�ƹ������}
	bool IsRButtonPressed(); // �P�_�ƹ��k����U
	bool IsMButtonPressed(); // �P�_�ƹ������u�����U
	void InputRelease();

private:
	
	bool CreateDirectInput(HINSTANCE);
	bool CreateKeyboard(HWND);
	bool CreateMouse(HWND);
};