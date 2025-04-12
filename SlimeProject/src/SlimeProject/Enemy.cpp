#include "pch.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

void Enemy::Init(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
	// MachineStateObject
	EnemyState newObjectState;
	m_enemyState = newObjectState;
	m_enemyState.mp_enemyEntity = mp_enemyEntity;
	m_enemyState.mp_playerEntity = mp_playerEntity;
	m_enemyState.mp_gameManager = mp_gameManager;
	//m_enemyState.mp_projectile = this;
	//m_enemyState.Shoot();
}

void Enemy::Update(float deltaTime)
{
	m_enemyState.Update(deltaTime);
}