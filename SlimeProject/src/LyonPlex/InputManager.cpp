#include "pch.h"
#include "InputManager.h"

int InputManager::m_mouseWheelDelta = 0;

bool InputManager::GetKeyIsPressed(char key) 
{
	return (GetAsyncKeyState(static_cast<unsigned char>(key)) < 0);
}

bool InputManager::GetKeyIsReleased(char key)
{
	return (GetAsyncKeyState(static_cast<unsigned char>(key)) >= 0);
}

bool InputManager::GetKeyDown(char key) 
{
	// Utiliser la touche comme index
	unsigned char ucKey = static_cast<unsigned char>(key);
	bool isKeyPressed = (GetAsyncKeyState(ucKey) & 0x8000) != 0;
	if (isKeyPressed && !ms_wasKeyPressed[ucKey]) {
		ms_wasKeyPressed[ucKey] = true;
		return true;
	}
	if (!isKeyPressed) {
		ms_wasKeyPressed[ucKey] = false;
	}
	return false;
}

// Fonction qui retourne true uniquement lors de la premiere frame ou la touche est appuyee
bool InputManager::GetKeyDownV2(char key)
{
	// Convertir le char en code de touche
	unsigned char ucKey = static_cast<unsigned char>(key);

	// Recuperer l'etat actuel (true si la touche est enfoncee)
	bool currentState = (GetAsyncKeyState(ucKey) & 0x8000) != 0;

	// Utiliser un conteneur statique pour memoriser l'etat precedent
	static std::unordered_map<char, bool> previousState;
	bool wasPressed = previousState[ucKey];

	// Mettre a jour l'etat pour la prochaine frame
	previousState[ucKey] = currentState;

	// Retourne true si actuellement appuye et qu'il ne l'etait pas la frame precedente
	return (currentState && !wasPressed);
}

// Fonction qui retourne true uniquement lors de la premiere frame ou la touche est relachee
bool InputManager::GetKeyReleased(char key)
{
	unsigned char ucKey = static_cast<unsigned char>(key);
	bool currentState = (GetAsyncKeyState(ucKey) & 0x8000) != 0;

	static std::unordered_map<char, bool> previousState;
	bool wasPressed = previousState[ucKey];

	previousState[ucKey] = currentState;

	// Retourne true si la touche etait appuyee la frame precedente et ne l'est plus maintenant
	return (!currentState && wasPressed);
}

void InputManager::UpdateMouse(HWND hWnd)
{
	if (ms_cursorLockedAndInvisible == true)
	{
		SetCursorOnTheCenterOfTheScreen(hWnd);
	}

	else
	{
		POINT currentPos;
		// Recuperer la position de la souris en coordonnees ecran
		GetCursorPos(&currentPos);
		// Convertir en coordonnees client
		ScreenToClient(hWnd, &currentPos);

		// Calculer le delta par rapport a la derniere position
		ms_MouseDelta.x = currentPos.x - ms_LastMousePos.x;
		ms_MouseDelta.y = currentPos.y - ms_LastMousePos.y;

		// Mettre a jour la derniere position
		ms_LastMousePos = currentPos;
	}
}

void InputManager::SetCursorOnTheCenterOfTheScreen(HWND hWnd)
{
	// Obtenir les dimensions de la fenetre client
	RECT rect;
	GetClientRect(hWnd, &rect);
	POINT center;
	center.x = (rect.right - rect.left) / 2;
	center.y = (rect.bottom - rect.top) / 2;

	// Convertir le centre en coordonnees ecran
	POINT centerScreen = center;
	ClientToScreen(hWnd, &centerScreen);
	POINT currentPos;
	// Recuperer la position de la souris en coordonnees ecran
	GetCursorPos(&currentPos);

	// Calculer le delta par rapport au centre
	ms_MouseDelta.x = currentPos.x - centerScreen.x;
	ms_MouseDelta.y = currentPos.y - centerScreen.y;

	// Reinitialiser la souris au centre de l'ecran
	SetCursorPos(centerScreen.x, centerScreen.y);
}

int InputManager::GetMouseDeltaX()
{
	return ms_MouseDelta.x;
}

int InputManager::GetMouseDeltaY()
{
	return ms_MouseDelta.y;
}

void InputManager::SetCursorLockedAndInvisible(bool CursorLockedAndInvisible) 
{ 
	ms_cursorLockedAndInvisible = CursorLockedAndInvisible; 
}

void InputManager::ProcessMouseWheel(WPARAM wParam)
{
	// GET_WHEEL_DELTA_WPARAM extrait le delta de la molette (généralement en multiples de WHEEL_DELTA, qui vaut 120)
	m_mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
}

int InputManager::GetMouseWheelDelta()
{
	int delta = m_mouseWheelDelta;
	// Réinitialisation après lecture pour éviter de cumulater indéfiniment
	m_mouseWheelDelta = 0;
	return delta;
}