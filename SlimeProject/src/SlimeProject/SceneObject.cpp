#include "pch.h"
#include "SceneObject.h"


SceneObject::SceneObject()
{
}

SceneObject::~SceneObject()
{
}

void SceneObject::Init(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
	
	// MachineStateObject
	ObjectState newObjectState;
	m_objectState = newObjectState;
	m_objectState.mp_objectEntity = mp_objectEntity;
	m_objectState.mp_playerEntity = mp_playerEntity;
	m_objectState.mp_gameManager = mp_gameManager;
	m_objectState.mp_object = this;
	m_objectState.Place();
}

void SceneObject::Update(float deltaTime)
{
	m_objectState.Update(deltaTime);
}