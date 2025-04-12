#pragma once

class InitDirect3DApp;
class EntityManager;

class LifeTimeManager
{
public:
	void Initialize(InitDirect3DApp* gameManager);

	void Update(float deltaTime);

private:
	InitDirect3DApp* mp_gameManager;

};

