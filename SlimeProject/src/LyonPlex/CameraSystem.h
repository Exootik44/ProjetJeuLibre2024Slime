#pragma once
#include "Components.h"


class InitDirect3DApp;

class CameraSystem
{
public:

    void Initialize(InitDirect3DApp* gameManager);

    void Update();

    DirectX::XMMATRIX DefaultView();

    DirectX::XMMATRIX GetViewMatrix() { return m_viewMatrix; }

    void SetViewMatrix(Transform* transform);

    void SetViewMatrix(DirectX::XMMATRIX viewMatrix);

    void ChangeView() { FPS = !FPS; TPS = !TPS; }

    void SetFPS() { FPS = true; TPS = false; }
    void SetTPS() { TPS = true; FPS = false; }
    void SetTPS_Lock(bool value) { TPS_Lock = value; }
private:

    InitDirect3DApp* mp_gameManager;

    DirectX::XMMATRIX m_viewMatrix;

    bool FPS = false; // First Person Shooter
    bool TPS = false; // Third Person Shooter
    bool TPS_Lock = false; // Third Person Shooter 

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_manualDistance = 0.0f;
};

