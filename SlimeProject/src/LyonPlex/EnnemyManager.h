#pragma once

#include "EntityManager.h"
#include "InitDirect3DApp.h"
#include "InputManager.h"

class EnnemyManager
{
public:
	EnnemyManager();

	void Initialize(InitDirect3DApp* app);

	void Update();

	void EnnemyAttackSystem();

	void SpawnEnnemy(float posX, float posY, float posZ);
	void SpawnEnnemyBoss(float posX, float posY, float posZ);

	void WaveSystem();

	int GetNbEnnemy() { return m_nbEnnemy; };
	void SetNbEnnemy(int value) { m_nbEnnemy = value; };

private:

	EntityManager* mp_entityManager;
	InitDirect3DApp* mp_gameManager;
	TransformComponent* mp_playerTransform;
	Entity* mp_player;

	bool m_gameStart;
	bool m_startNextWave;

	int m_waveNb;
	int m_nbEnnemy, m_nbEnnemyToSpawn;
};

