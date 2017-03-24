#include "GameEngine_Input.h"

CGameEngine_Input::CGameEngine_Input() {
	m_pKeyboardDevice = NULL;
	m_lMouseMoveX = 0;
	m_lMouseMoveY = 0;
}

bool CGameEngine_Input::CreateInput(HINSTANCE hInst, HWND hWnd,
	int iMin, int iMax,int iDeadZone) {
		CreateDirectInput(hInst);
		CreateKeyboard(hWnd);
		CreateMouse(hWnd);
		return true;
}

bool CGameEngine_Input::CreateDirectInput(HINSTANCE hInst) {
	// 創建IDirectInput接口對象
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&g_pIDirectInput, NULL))) {
			MessageBox(NULL, "建立IDirectInput8接口對象失敗.", "警告", MB_OK | MB_ICONINFORMATION);
			return false;
	}
	return true;
}

bool CGameEngine_Input::CreateKeyboard(HWND hWnd) {
	// 創建鍵盤輸入設備
	if (FAILED(g_pIDirectInput -> CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL))) {
		MessageBox(NULL, "建立鍵盤輸入設備對象失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 設備鍵盤輸入設備的協調級別
	if (FAILED(m_pKeyboardDevice -> SetCooperativeLevel(hWnd,
		DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
			MessageBox(NULL, "設備鍵盤協調級別失敗.", "警告", MB_OK | MB_ICONINFORMATION);
			InputRelease();
			return false;
	}
	
	// 設備鍵盤輸入設備的數據格式
	if (FAILED(m_pKeyboardDevice -> SetDataFormat(&c_dfDIKeyboard))) {
		MessageBox(NULL, "設備鍵盤的數據讀取數據格式失敗.", "警告", MB_OK|MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 獲取鍵盤輸入設備的訪問權
	if (FAILED(m_pKeyboardDevice -> Acquire())) {
		MessageBox(NULL, "取得鍵盤輸入設備的訪問權失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 初始化鍵盤緩衝區
	ZeroMemory(m_KeyBuffer, sizeof(char)*256);
	return true;
}

// 判斷用戶按鍵函數
bool CGameEngine_Input::ReadKeyboard() { //key = DIK_RIGHT,DIK_LEFT,DIK_A,.....
	if (DIERR_INPUTLOST == m_pKeyboardDevice -> GetDeviceState(
		sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer)) {

			m_pKeyboardDevice -> Acquire(); // 重新獲取鍵盤的使用權
			
			if (FAILED(m_pKeyboardDevice -> GetDeviceState(
				sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer))) {
					return false;
			}
	}
	return true;
}

bool CGameEngine_Input::CreateMouse(HWND hWnd) {
	// 創建滑鼠輸入設備
	if (FAILED(g_pIDirectInput -> CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL))) {
		MessageBox(NULL, "建立滑鼠輸入設備對象失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 設置滑鼠設備的數據格式
	if (FAILED(m_pMouseDevice -> SetDataFormat(&c_dfDIMouse))) {
		MessageBox(NULL, "設置滑鼠設備的數據格式失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 設置滑鼠設備的協調級別
	if (FAILED(m_pMouseDevice -> SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
		MessageBox(NULL, "設置滑鼠設備的協調級別失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// 設置滑鼠設備的屬性(使用緩衝區讀數據)
	DIPROPDWORD dipROPWORD;
	dipROPWORD.diph.dwSize = sizeof(DIPROPDWORD);
	dipROPWORD.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipROPWORD.diph.dwObj = 0;
	dipROPWORD.diph.dwHow = DIPH_DEVICE;
	dipROPWORD.dwData = ITEMS_NUM; // #define ITEMS_NUM 10
	if (FAILED(m_pMouseDevice -> SetProperty(DIPROP_BUFFERSIZE, &dipROPWORD.diph))) {
		MessageBox(NULL, "設置滑鼠設備的屬性失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	//獲取滑鼠設備的訪問權
	if (FAILED(m_pMouseDevice -> Acquire())) {
		MessageBox(NULL, "獲取滑鼠設備的訪問權失敗.", "警告", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	return true;
}

//判斷鍵盤的按下狀況
bool CGameEngine_Input::IsKeyPressed(int key) {
	HRESULT hr;
	hr = m_pKeyboardDevice -> GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer);
	if (hr == DIERR_INPUTLOST) {
		m_pKeyboardDevice -> Acquire();
		hr = m_pKeyboardDevice -> GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer);
		if (FAILED(hr)) {
			MessageBox(NULL, "取得鍵盤按鍵狀態失敗", "警告", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}
	if (m_KeyBuffer[key] & 0x80) {
		return true;
	}
	return false;
}

//捕捉鼠標按鍵和移動數據
bool CGameEngine_Input::ReadMouse() {
	DWORD dwReadNum = 1;
	int i;
	//每次讀取數據前，一定要使鼠標緩衝區歸零
	ZeroMemory(m_MouseData,sizeof(DIDEVICEOBJECTDATA)*ITEMS_NUM);
	//循環讀取鼠標數據
	for (i=0; i < ITEMS_NUM; i++) {
		if (DIERR_INPUTLOST == m_pMouseDevice -> GetDeviceData(
			sizeof(DIDEVICEOBJECTDATA), &m_MouseData[i], &dwReadNum,0)) {
				m_pMouseDevice -> Acquire();
				if (FAILED(m_pMouseDevice -> GetDeviceData(
					sizeof(DIDEVICEOBJECTDATA), &m_MouseData[i], &dwReadNum,0))) {
						return false;
				}
		}
		if (m_MouseData[i].dwOfs == DIMOFS_X) {
			m_lMouseMoveX += m_MouseData[i].dwData;
		}
		if (m_MouseData[i].dwOfs == DIMOFS_Y) {
			m_lMouseMoveY += m_MouseData[i].dwData;
		}
	}
	return true;
}

//判斷鼠標左鍵按下
bool CGameEngine_Input::IsLButtonPressed() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON0) && (m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//判斷鼠標左鍵按下
bool CGameEngine_Input::IsLButtonUp() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON0) && !(m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//判斷鼠標右鍵按下
bool CGameEngine_Input::IsRButtonPressed() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON1) && (m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//判斷鼠標滾輪按下
bool CGameEngine_Input::IsMButtonPressed() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON2) && (m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

CGameEngine_Input::~CGameEngine_Input() {

}

void CGameEngine_Input::InputRelease() {
	if (m_pKeyboardDevice) {
		m_pKeyboardDevice -> Unacquire();
	}
	if (m_pMouseDevice) {
		m_pMouseDevice -> Unacquire();
	}
	
	SafeRelease(m_pKeyboardDevice);
	SafeRelease(m_pMouseDevice);
	SafeRelease(g_pIDirectInput);
}

int CGameEngine_Input::GetMouseX(HWND hWnd) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd,&point);
	return point.x;
}

int CGameEngine_Input::GetMouseY(HWND hWnd) {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd,&point);
	return point.y;
}