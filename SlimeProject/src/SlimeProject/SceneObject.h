#pragma once

#include "EntityManager.h"
#include "StateMachine.h"
#include "InitDirect3DApp.h"

class SceneObject
{
public:
	SceneObject();
	~SceneObject();
	void Init(InitDirect3DApp* gameManager);
	void Update(float deltaTime);

	ObjectState GetObjectState() { return m_objectState; };
	void SetObjectEntity(Entity* object) { mp_objectEntity = object; };

	Entity* GetPlayerEntity() { return mp_playerEntity; };
	void SetPlayerEntity(Entity* player) { mp_playerEntity = player; };

	float GetStartPosY() { return m_startPosY; };
	void SetStartPosY(float value) { m_startPosY = value; };

	float GetListIndex() { return m_listIndex; };
	void SetListIndex(float value) { m_listIndex = value; };

	float GetToDestroy() { return m_toDestroy; };
	void SetToDestroy(float value) { m_toDestroy = value; };

	InitDirect3DApp* mp_gameManager;
	Entity* mp_objectEntity;


private:

	float m_startPosY;

	bool m_toDestroy = false;

	int m_listIndex;

	ObjectState m_objectState;
	Entity* mp_playerEntity;
};

