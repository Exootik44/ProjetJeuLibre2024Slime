#pragma once

#include "EntityManager.h"
#include "StateMachine.h"
#include "InitDirect3DApp.h"
#include "Projectile.h"

class SceneDemo;

class Player
{
public:
	Player();
	void Init(InitDirect3DApp* gameManager, EntityManager* entityManager, SceneDemo* scene);
	void Update(float deltaTime);

	PlayerState& GetPlayerState() { return m_playerState; };
	void SetPlayerEntity(Entity* player) { mp_playerEntity = player; m_playerState.mp_playerEntity = mp_playerEntity ; };

	//void OnCollision(Entity* otherEntity) override;

	void Regurgitation();

	float CheckPosY();

	float GetLastPosY() { return m_lastPosY; };
	void SetLastPosY(float value) { m_lastPosY = value; };

	float GetJumpPosY() { return m_jumpPosY; };
	void SetJumpPosY(float value) { m_jumpPosY = value; };

	float GetScale() { return m_scale; };
	void SetScale(float value) { m_scale = value; };

	float GetJumpForce() { return m_jumpForce; };
	void SetJumpForce(float value) { m_jumpForce = value; };

	float GetSpeed() { return m_speed; };
	void SetSpeed(float value) { m_speed = value; };

	float GetProjectileCount() { return m_projectileCount; };
	void SetProjectileCount(float value) { m_projectileCount = value; };
	void IncrementeProjectileCount(float value) { m_projectileCount += value; };

	bool CanJump() { return canJump; };
	void SetCanJump(float value) { canJump = value; };

	Entity* GetLifePointUI() { return lifepointUI; };

	std::vector<Projectile*>& GetProjectileList() { return m_projectilesList; };
	std::vector<Entity*>& GetUIList() { return m_UIList; };
	std::vector<Entity*>& GetUIBossList() { return m_UIBossList; };

	/*void IncrementeBridgeCondition() { m_bridgeConditionCount++; }
	int GetBridgeConditionCount() const { return m_bridgeConditionCount; }*/


	InitDirect3DApp* mp_gameManager;
	Entity* mp_playerEntity;
	EntityManager* mp_entityManager;

	SceneDemo* mp_scene;

private:
	Entity* lifepointUI;

	PlayerState m_playerState;
	float m_lastPosY, m_jumpPosY,m_scale,m_jumpForce, m_speed; 
	bool canJump;

	int m_index;

	std::vector<Projectile*> m_projectilesList = {};

	int m_projectileCount;

	std::vector<Entity*> m_UIList = {};
	std::vector<Entity*> m_UIBossList = {};

	//int m_bridgeConditionCount = 0;
};

