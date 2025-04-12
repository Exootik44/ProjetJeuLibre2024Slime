#pragma once

class GameManager;
class EntityManager;
class Movement;
class AttackSystem;
class CameraSystem;
class InitDirect3DApp;

class Scene
{
protected:
	InitDirect3DApp* mp_gameManager;
	EntityManager* mpEntityManager;

	CameraSystem* mp_camera;

public:
	void Initialize(InitDirect3DApp* gameManager);
	virtual void OnInitialize() = 0;

	void Update();
	virtual void OnUpdate() = 0;

	void Close();
	virtual void OnClose() = 0;

	friend class GameManager;
};

