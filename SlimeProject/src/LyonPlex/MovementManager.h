#pragma once

#include "Components.h"
#include "EntityManager.h"

class GameManager;
class InitDirect3DApp;

class MovementManager
{
protected:

	InitDirect3DApp* mp_gameManager;

	bool m_jumpStart;
	float m_jumpTime, m_maxJumpTime, m_fallTime;
	float m_limitPosMin, m_limitPosMax;
public:

	void Initialize(InitDirect3DApp* gameManager);

	void Update(float deltaTime);

	void SetVelocity(VelocityComponent* velComponent, float velFront, float velRight, float velUp);

	void Move(Entity* entity, VelocityComponent* velComponent, TransformComponent* transformComponent, float deltaTime);
};

