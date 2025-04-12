#pragma once

#include "EntityManager.h"
#include "ParticleManager.h"

using namespace DirectX;

struct AABB {
	XMFLOAT3 Min;
	XMFLOAT3 Max;
};

class ColliderManager
{
public:
	void Initialize(EntityManager* entityManager, ParticleManager* particleManager);

	void Update();

	bool AABBIntersect(const TransformComponent& a, const TransformComponent& b);

	int CollideWith(Entity* entity);

	XMVECTOR ResolveAABBCollision(const TransformComponent& a, const TransformComponent& b);

private:

	EntityManager* mp_entityManager;
	ParticleManager* mp_particleManager;
};


