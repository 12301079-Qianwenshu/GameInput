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
	// �Ы�IDirectInput���f��H
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&g_pIDirectInput, NULL))) {
			MessageBox(NULL, "�إ�IDirectInput8���f��H����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
			return false;
	}
	return true;
}

bool CGameEngine_Input::CreateKeyboard(HWND hWnd) {
	// �Ы���L��J�]��
	if (FAILED(g_pIDirectInput -> CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL))) {
		MessageBox(NULL, "�إ���L��J�]�ƹ�H����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// �]����L��J�]�ƪ���կŧO
	if (FAILED(m_pKeyboardDevice -> SetCooperativeLevel(hWnd,
		DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
			MessageBox(NULL, "�]����L��կŧO����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
			InputRelease();
			return false;
	}
	
	// �]����L��J�]�ƪ��ƾڮ榡
	if (FAILED(m_pKeyboardDevice -> SetDataFormat(&c_dfDIKeyboard))) {
		MessageBox(NULL, "�]����L���ƾ�Ū���ƾڮ榡����.", "ĵ�i", MB_OK|MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// �����L��J�]�ƪ��X���v
	if (FAILED(m_pKeyboardDevice -> Acquire())) {
		MessageBox(NULL, "���o��L��J�]�ƪ��X���v����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// ��l����L�w�İ�
	ZeroMemory(m_KeyBuffer, sizeof(char)*256);
	return true;
}

// �P�_�Τ������
bool CGameEngine_Input::ReadKeyboard() { //key = DIK_RIGHT,DIK_LEFT,DIK_A,.....
	if (DIERR_INPUTLOST == m_pKeyboardDevice -> GetDeviceState(
		sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer)) {

			m_pKeyboardDevice -> Acquire(); // ���s�����L���ϥ��v
			
			if (FAILED(m_pKeyboardDevice -> GetDeviceState(
				sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer))) {
					return false;
			}
	}
	return true;
}

bool CGameEngine_Input::CreateMouse(HWND hWnd) {
	// �Ыطƹ���J�]��
	if (FAILED(g_pIDirectInput -> CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL))) {
		MessageBox(NULL, "�إ߷ƹ���J�]�ƹ�H����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// �]�m�ƹ��]�ƪ��ƾڮ榡
	if (FAILED(m_pMouseDevice -> SetDataFormat(&c_dfDIMouse))) {
		MessageBox(NULL, "�]�m�ƹ��]�ƪ��ƾڮ榡����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// �]�m�ƹ��]�ƪ���կŧO
	if (FAILED(m_pMouseDevice -> SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
		MessageBox(NULL, "�]�m�ƹ��]�ƪ���կŧO����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	// �]�m�ƹ��]�ƪ��ݩ�(�ϥνw�İ�Ū�ƾ�)
	DIPROPDWORD dipROPWORD;
	dipROPWORD.diph.dwSize = sizeof(DIPROPDWORD);
	dipROPWORD.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipROPWORD.diph.dwObj = 0;
	dipROPWORD.diph.dwHow = DIPH_DEVICE;
	dipROPWORD.dwData = ITEMS_NUM; // #define ITEMS_NUM 10
	if (FAILED(m_pMouseDevice -> SetProperty(DIPROP_BUFFERSIZE, &dipROPWORD.diph))) {
		MessageBox(NULL, "�]�m�ƹ��]�ƪ��ݩʥ���.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	
	//����ƹ��]�ƪ��X���v
	if (FAILED(m_pMouseDevice -> Acquire())) {
		MessageBox(NULL, "����ƹ��]�ƪ��X���v����.", "ĵ�i", MB_OK | MB_ICONINFORMATION);
		InputRelease();
		return false;
	}
	return true;
}

//�P�_��L�����U���p
bool CGameEngine_Input::IsKeyPressed(int key) {
	HRESULT hr;
	hr = m_pKeyboardDevice -> GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer);
	if (hr == DIERR_INPUTLOST) {
		m_pKeyboardDevice -> Acquire();
		hr = m_pKeyboardDevice -> GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer);
		if (FAILED(hr)) {
			MessageBox(NULL, "���o��L���䪬�A����", "ĵ�i", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}
	if (m_KeyBuffer[key] & 0x80) {
		return true;
	}
	return false;
}

//�������Ы���M���ʼƾ�
bool CGameEngine_Input::ReadMouse() {
	DWORD dwReadNum = 1;
	int i;
	//�C��Ū���ƾګe�A�@�w�n�Ϲ��нw�İ��k�s
	ZeroMemory(m_MouseData,sizeof(DIDEVICEOBJECTDATA)*ITEMS_NUM);
	//�`��Ū�����мƾ�
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

//�P�_���Х�����U
bool CGameEngine_Input::IsLButtonPressed() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON0) && (m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//�P�_���Х�����U
bool CGameEngine_Input::IsLButtonUp() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON0) && !(m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//�P�_���Хk����U
bool CGameEngine_Input::IsRButtonPressed() {
	for (int i=0; i < ITEMS_NUM; i++) {
		if ((m_MouseData[i].dwOfs == DIMOFS_BUTTON1) && (m_MouseData[i].dwData & 0x80)) {
			return true;
		}
	}
	return false;
}

//�P�_���кu�����U
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