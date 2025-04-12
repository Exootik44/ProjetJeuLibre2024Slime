#pragma once

#include "Scene.h"
#include "Player.h"

class Enemy;
class SceneObject;
class PrefabHelper;

class SceneDemo : public Scene
{
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnClose() override;

	float GetSceneObjCount() { return m_sceneObjCount; };
	//void SetSceneObjCount(float value) { m_sceneObjCount = value; };
	void IncrementeSceneObjCount(float value) { m_sceneObjCount += value; };

	std::vector<SceneObject*>& GetSceneObjectList() { return mp_sceneObjectsList; };

protected:
	Entity* mp_playerEntity = nullptr;
	Player* mp_player = nullptr;
	PrefabHelper* prefabHelper = nullptr;
	Entity* mp_boss;
	std::vector<SceneObject*> mp_sceneObjectsList = {};
	std::vector<Enemy*> m_enemyList = {};
	std::vector<Entity*> m_skyboxList = {};
	float skyboxSpeedRot = 0.0001f;

	int m_sceneObjCount = 0;

	bool m_bossBattleStarted,m_bossHasSpawned, m_mapHasChanged;
};