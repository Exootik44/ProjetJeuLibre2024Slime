#pragma once

class InputManager
{
public:
	// Touche apuyee en continu
	static bool GetKeyIsPressed(char key);
	
	// Touche relache
	static bool GetKeyIsReleased(char key);

	// Touche appuyee une fois
	static bool GetKeyDown(char key);

	static bool GetKeyDownV2(char key);

	static bool GetKeyReleased(char key);

	static void UpdateMouse(HWND hWnd);
	static int GetMouseDeltaX();
	static int GetMouseDeltaY();

	static void SetCursorOnTheCenterOfTheScreen(HWND hWnd);
	static void SetCursorLockedAndInvisible(bool CursorLockedAndInvisible);

	// AppelÅEdans la boucle de message pour traiter WM_MOUSEWHEEL
	static void ProcessMouseWheel(WPARAM wParam);

	// Retourne le delta accumulÅEde la molette depuis le dernier update
	static int GetMouseWheelDelta();

private:
	static inline bool ms_wasKeyPressed[256] = { false };

	static inline POINT ms_LastMousePos = { 0, 0 };
	static inline POINT ms_MouseDelta = { 0, 0 };

	static inline bool ms_cursorLockedAndInvisible = true;

	static int m_mouseWheelDelta;
};