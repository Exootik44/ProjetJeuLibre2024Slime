#pragma once

#include "EntityManager.h"
#include "StateMachine.h"
#include "InitDirect3DApp.h"

class Enemy
{
public:
	Enemy();
	void Init(InitDirect3DApp* gameManager);
	void Update(float deltaTime);

	EnemyState GetEnemyState() { return m_enemyState; };
	void SetEnemyEntity(Entity* object) { mp_enemyEntity = object; };

	Entity* GetPlayerEntity() { return mp_playerEntity; };
	void SetPlayerEntity(Entity* player) { mp_playerEntity = player; };

	float GetStartPosY() { return m_startPosY; };
	void SetStartPosY(float value) { m_startPosY = value; };

	float GetToDestroy() { return m_toDestroy; };
	void SetToDestroy(float value) { m_toDestroy = value; };

	InitDirect3DApp* mp_gameManager;
	Entity* mp_enemyEntity;

private:

	float m_startPosY;

	bool m_toDestroy = false;

	EnemyState m_enemyState;
	Entity* mp_playerEntity;
};

