#include "pch.h"
#include "Projectile.h"

Projectile::Projectile()
{
}

void Projectile::Init(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
	// MachineStateObject
	ObjectState newObjectState;
	m_objectState = newObjectState;
	m_objectState.mp_objectEntity = mp_objectEntity;
	m_objectState.mp_playerEntity = mp_playerEntity;
	m_objectState.mp_gameManager = mp_gameManager;
	m_objectState.mp_projectile = this;
	//m_objectState.Shoot();
	m_objectState.Place();
} 

void Projectile::Update(float deltaTime)
{
	m_objectState.Update(deltaTime);
}