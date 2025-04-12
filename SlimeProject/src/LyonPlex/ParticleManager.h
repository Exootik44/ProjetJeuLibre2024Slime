#pragma once

#include "EntityManager.h"
#include "InitDirect3DApp.h"

class ParticleManager
{
public:
	ParticleManager();
	~ParticleManager();

	void Initialize(InitDirect3DApp* app);

	void Update();

	void CreateParticle(float startPosX, float startPosY, float startPosZ, float size, float speedX, float speedY, float speedZ, std::wstring textureName);

	void MakeEffect(float startPosX, float startPosY, float startPosZ, int _minNbPart, int _maxNbPart, int _minSpeed, int _maxSpeed, int minSize, int maxSize, std::wstring textureName);

	void Explosion(float startPosX, float startPosY, float startPosZ);

private:

	EntityManager* mp_entityManager;
	InitDirect3DApp* mp_gameManager;

};

