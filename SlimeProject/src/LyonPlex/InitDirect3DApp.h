#pragma once

#include "WindowDX.h"
#include "Scene.h"
#include "LightSystem.h"

class MeshFactory;

class CameraSystem;
class Attackystem;
class HealthSystem;
class DoorSystem;
class SunOrbitSystem;

class ColliderManager;
class ParticleManager;
class EnnemyManager;
class MovementManager;
class LifeTimeManager;
class TextureManager;

class InitDirect3DApp : public WindowDX
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	bool Initialize(Scene* scene);
	bool InitConstantBuffer();
	bool InitTexture();
	void Update() override;
	void UpdatePhysics();
	void UpdateTimer();
	void Draw() override;

	void Render();
	void RenderUI();
	void CreatePipelineState();
	void CreateUIPipelineStatev2();
	void CreateUIPipelineState(); // ENLEVER ?

	void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }
	void SetScene(Scene* scene) { mp_scene = scene; }

	Scene* GetScene() { return mp_scene; }
	bool IsCursorOnUIEntity(Entity* entity, HWND hWnd);
	void CreateButtonUI();
	void CreatePauseMenu();
	void DestroyPauseMenu();
	EntityManager* GetEntityManager() { return mp_entityManager; }
	MeshFactory* GetFactory() { return mp_meshFactory; }
	ParticleManager* GetParticleManager() { return mp_particleManager; }
	CameraSystem* GetCameraSystem() { return mp_cameraManager; }
	EnnemyManager* GetEnnemyManager() { return mp_ennemyManager; }
	float GetDeltaTime() { return m_deltaTime; }

	friend class Scene;

private:
	D3D12_DEPTH_STENCIL_DESC m_depthStencilDesc;

	MeshFactory* mp_meshFactory;

	EntityManager* mp_entityManager;
	ColliderManager* mp_colliderManager;
	EnnemyManager* mp_ennemyManager;
	ParticleManager* mp_particleManager;
	TextureManager* mp_textureManager;
	MovementManager* mp_movementManager;
	LifeTimeManager* mp_lifeTimeManager;

	HealthSystem* mp_healthSystem;
	AttackSystem* mp_attackSystem;
	CameraSystem* mp_cameraManager;
	LightSystem* mp_lightSystem;
	DoorSystem* mp_doorSystem;
	SunOrbitSystem* mp_sunOrbitSystem;

	Scene* mp_scene;

	float m_deltaTime = 0.0f;
	DWORD m_lastTime = 0;

	ComPtr<ID3D12Resource> m_texture;

	bool m_gameIsPaused, m_pauseScreenCreated, m_mouseIsOnButton;

	ComPtr<ID3D12Resource> m_passConstantBuffer;
	PassConstants* mp_mappedPassCB = nullptr;
};

