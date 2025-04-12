#pragma once

class InitDirect3DApp;
class EntityManager;

class AttackSystem
{
protected:

    InitDirect3DApp* m_gameManager;

public:
    void Initialize(InitDirect3DApp* gameManager);

    void Update(float deltaTime);
};