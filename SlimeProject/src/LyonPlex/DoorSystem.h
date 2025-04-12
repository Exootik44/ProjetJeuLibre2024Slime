#pragma once
#include "Components.h"

class InitDirect3DApp;
class DoorSystem {
public:
    void Initialize(InitDirect3DApp* gameManager);

    void Update(float deltaTime);

private:
    InitDirect3DApp* m_gameManager = nullptr;

    static float MoveTowards(float current, float target, float step);
    bool m_startOpen;
};
