#pragma once
#include "Components.h"

class InitDirect3DApp;

class SunOrbitSystem
{
public:
    void Initialize(InitDirect3DApp* gameManager);

    void Update(float deltaTime);

private:
    InitDirect3DApp* mp_gameManager;
    //Entity* mp_entityToOrbitAround;
};
