#include "pch.h"
#include "ColliderManager.h"
#include "InputManager.h"

void ColliderManager::Initialize(EntityManager* entityManager, ParticleManager* particleManager)
{
	mp_entityManager = entityManager;
	mp_particleManager = particleManager;
}

void ColliderManager::Update()
{
	for (auto& entity1 : mp_entityManager->GetEntityTab())
	{
		if (entity1 == nullptr || !mp_entityManager->HasComponent(entity1, COMPONENT_COLLIDER))
			continue;

		TransformComponent* transform1 = nullptr;
		ColliderComponent* collider1 = nullptr;
		VelocityComponent* velocity1 = nullptr;
		HealthComponent* health1 = nullptr;
		AttackComponent* attack1 = nullptr;
		PlayerComponent* player1 = nullptr;
		MeshComponent* mesh1 = nullptr;
		GravityComponent* gravity1 = nullptr;

		for (auto* component : mp_entityManager->GetComponentsTab()[entity1->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform1 = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Collider_ID)
			{
				collider1 = static_cast<ColliderComponent*>(component);
			}
			if (component->ID == Velocity_ID)
			{
				velocity1 = static_cast<VelocityComponent*>(component);
			}
			if (component->ID == Health_ID)
			{
				health1 = static_cast<HealthComponent*>(component);
			}
			if (component->ID == Attack_ID)
			{
				attack1 = static_cast<AttackComponent*>(component);
			}
			if (component->ID == Player_ID)
			{
				player1 = static_cast<PlayerComponent*>(component);
			}
			if (component->ID == Mesh_ID)
			{
				mesh1 = static_cast<MeshComponent*>(component);
			}
			if (component->ID == Gravity_ID)
			{
				gravity1 = static_cast<GravityComponent*>(component);
			}
		}
		if (!transform1 || !collider1 || !collider1->m_isDrawable)
			continue;


		for (uint32_t entity2Index = entity1->tab_index + 1; entity2Index < 1000; entity2Index++)
		{
			auto& entity2 = mp_entityManager->GetEntityTab()[entity2Index];
			if (!entity2 || !mp_entityManager->HasComponent(entity2, COMPONENT_COLLIDER) || (mp_entityManager->HasComponent(entity1, COMPONENT_SCENEOBJECT)&& mp_entityManager->HasComponent(entity2, COMPONENT_SCENEOBJECT)))
				continue;

			TransformComponent* transform2 = nullptr;
			ColliderComponent* collider2 = nullptr;
			VelocityComponent* velocity2 = nullptr;
			HealthComponent* health2 = nullptr;
			AttackComponent* attack2 = nullptr;
			PlayerComponent* player2 = nullptr;
			MeshComponent* mesh2 = nullptr;
			GravityComponent* gravity2 = nullptr;

			for (auto* component : mp_entityManager->GetComponentsTab()[entity2Index]->vec_components)
			{
				if (component->ID == Transform_ID)
				{
					transform2 = static_cast<TransformComponent*>(component);
				}
				if (component->ID == Collider_ID)
				{
					collider2 = static_cast<ColliderComponent*>(component);
				}
				if (component->ID == Velocity_ID)
				{
					velocity2 = static_cast<VelocityComponent*>(component);
				}
				if (component->ID == Health_ID)
				{
					health2 = static_cast<HealthComponent*>(component);
				}
				if (component->ID == Attack_ID)
				{
					attack2 = static_cast<AttackComponent*>(component);
				}
				if (component->ID == Player_ID)
				{
					player2 = static_cast<PlayerComponent*>(component);
				}
				if (component->ID == Mesh_ID)
				{
					mesh2 = static_cast<MeshComponent*>(component);
				}
				if (component->ID == Gravity_ID)
				{
					gravity2 = static_cast<GravityComponent*>(component);
				}
			}
			if (!transform2 || !collider2 || !collider2->m_isDrawable)
				continue;

			// Test de collision
			if (AABBIntersect(*transform1, *transform2))
			{

				collider1->m_isColliding = true;
				collider2->m_isColliding = true;

				//if (velocity1 && !velocity2)
				if (mp_entityManager->HasComponent(entity1, COMPONENT_VELOCITY) && !mp_entityManager->HasComponent(entity2, COMPONENT_VELOCITY))
				{
					// Calculer le vecteur de correction minimal
					DirectX::XMVECTOR correction = ResolveAABBCollision(*transform1, *transform2);

					// Charger la position actuelle dans un XMVECTOR
					DirectX::XMVECTOR newPos = DirectX::XMLoadFloat3(&transform1->m_transform.vPosition);

					// Soustraire le vecteur de correction
					newPos = DirectX::XMVectorSubtract(newPos, correction);

					// Stocker la nouvelle position dans vPosition
					DirectX::XMStoreFloat3(&transform1->m_transform.vPosition, newPos);
					transform1->m_transform.UpdateMatrix();
				}
				else if (!mp_entityManager->HasComponent(entity1, COMPONENT_VELOCITY) && mp_entityManager->HasComponent(entity2, COMPONENT_VELOCITY))
				{
					// Calculer le vecteur de correction minimal
					DirectX::XMVECTOR correction = ResolveAABBCollision(*transform2, *transform1);

					// Charger la position actuelle dans un XMVECTOR
					DirectX::XMVECTOR newPos = DirectX::XMLoadFloat3(&transform2->m_transform.vPosition);

					// Soustraire le vecteur de correction
					newPos = DirectX::XMVectorSubtract(newPos, correction);

					// Stocker la nouvelle position dans vPosition
					DirectX::XMStoreFloat3(&transform2->m_transform.vPosition, newPos);
					transform2->m_transform.UpdateMatrix();
				}
				else if (mp_entityManager->HasComponent(entity1, COMPONENT_VELOCITY) && mp_entityManager->HasComponent(entity2, COMPONENT_VELOCITY))// Si les deux sont dynamiques, repartir la correction
				{
					DirectX::XMVECTOR correction = ResolveAABBCollision(*transform1, *transform2);
					DirectX::XMVECTOR halfCorr = DirectX::XMVectorScale(correction, 0.5f);

					// Charger la position actuelle dans un XMVECTOR
					DirectX::XMVECTOR newPos1 = DirectX::XMLoadFloat3(&transform1->m_transform.vPosition);
					DirectX::XMVECTOR newPos2 = DirectX::XMLoadFloat3(&transform2->m_transform.vPosition);

					// Soustraire le vecteur de correction
					newPos1 = DirectX::XMVectorSubtract(newPos1, halfCorr);
					newPos2 = DirectX::XMVectorAdd(newPos2, halfCorr);
					// Stocker la nouvelle position dans vPosition
					DirectX::XMStoreFloat3(&transform1->m_transform.vPosition, newPos1);
					DirectX::XMStoreFloat3(&transform2->m_transform.vPosition, newPos2);

					transform1->m_transform.UpdateMatrix();
					transform2->m_transform.UpdateMatrix();
				}

				/*if (health1 && attack2)
				{
					health1->currentHealth -= attack2->damage;
				}
				if (health2 && attack1)
				{
					health2->currentHealth -= attack1->damage;
				}*/

				if (entity1 && collider1->m_isDestructable && !entity1->m_isDestroyed)
				{
					mp_entityManager->ToDestroy(entity1);
					mp_particleManager->Explosion(transform1->m_transform.GetPositionX(), transform1->m_transform.GetPositionY(), transform1->m_transform.GetPositionZ());
				}

				if (entity2 && collider2->m_isDestructable && !entity2->m_isDestroyed)
				{
					//m_particleManager->Explosion(transform2->m_transform.GetPositionX(), transform2->m_transform.GetPositionY(), transform2->m_transform.GetPositionZ());
					//mp_entityManager->DestroyEntity(entity2);
					mp_entityManager->ToDestroy(entity2);
					mp_particleManager->Explosion(transform2->m_transform.GetPositionX(), transform2->m_transform.GetPositionY(), transform2->m_transform.GetPositionZ());
				}
			}
			else
			{
				collider1->m_isColliding = false;
				collider2->m_isColliding = false;
			}
		}
	}
}

bool ColliderManager::AABBIntersect(const TransformComponent& a, const TransformComponent& b)
{
	// Calculer les bornes pour lobjet A
	float aMinX = a.m_transform.vPosition.x - a.m_transform.vScale.x / 2;
	float aMaxX = a.m_transform.vPosition.x + a.m_transform.vScale.x / 2;
	float aMinY = a.m_transform.vPosition.y - a.m_transform.vScale.y / 2;
	float aMaxY = a.m_transform.vPosition.y + a.m_transform.vScale.y / 2;
	float aMinZ = a.m_transform.vPosition.z - a.m_transform.vScale.z / 2;
	float aMaxZ = a.m_transform.vPosition.z + a.m_transform.vScale.z / 2;

	// Calculer les bornes pour lobjet B
	float bMinX = b.m_transform.vPosition.x - b.m_transform.vScale.x / 2;
	float bMaxX = b.m_transform.vPosition.x + b.m_transform.vScale.x / 2;
	float bMinY = b.m_transform.vPosition.y - b.m_transform.vScale.y / 2;
	float bMaxY = b.m_transform.vPosition.y + b.m_transform.vScale.y / 2;
	float bMinZ = b.m_transform.vPosition.z - b.m_transform.vScale.z / 2;
	float bMaxZ = b.m_transform.vPosition.z + b.m_transform.vScale.z / 2;

	// Verification du chevauchement sur chaque axe
	if (aMaxX < bMinX || aMinX > bMaxX)
		return false;
	if (aMaxY < bMinY || aMinY > bMaxY)
		return false;
	if (aMaxZ < bMinZ || aMinZ > bMaxZ)
		return false;

	return true;
}

int ColliderManager::CollideWith(Entity* entity)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
	}
	if (!transform1)
		return -1;

	for (auto& entity2 : mp_entityManager->GetEntityTab())
	{
		if (!entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : mp_entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}
		if (!transform2)
			continue;

		// Test de collision
		if (AABBIntersect(*transform1, *transform2))
		{
			return entity2->m_tag;
		}
	}

	return -1;
}

XMVECTOR ColliderManager::ResolveAABBCollision(const TransformComponent& a, const TransformComponent& b)
{
	// Calculer les bornes pour lobjet A
	float aMinX = a.m_transform.vPosition.x - a.m_transform.vScale.x / 2;
	float aMaxX = a.m_transform.vPosition.x + a.m_transform.vScale.x / 2;
	float aMinY = a.m_transform.vPosition.y - a.m_transform.vScale.y / 2;
	float aMaxY = a.m_transform.vPosition.y + a.m_transform.vScale.y / 2;
	float aMinZ = a.m_transform.vPosition.z - a.m_transform.vScale.z / 2;
	float aMaxZ = a.m_transform.vPosition.z + a.m_transform.vScale.z / 2;

	// Calculer les bornes pour lobjet B
	float bMinX = b.m_transform.vPosition.x - b.m_transform.vScale.x / 2;
	float bMaxX = b.m_transform.vPosition.x + b.m_transform.vScale.x / 2;
	float bMinY = b.m_transform.vPosition.y - b.m_transform.vScale.y / 2;
	float bMaxY = b.m_transform.vPosition.y + b.m_transform.vScale.y / 2;
	float bMinZ = b.m_transform.vPosition.z - b.m_transform.vScale.z / 2;
	float bMaxZ = b.m_transform.vPosition.z + b.m_transform.vScale.z / 2;

	// Calcul des profondeurs de penetration sur chaque axe
	// On calcule combien lobjet B setend au dela de lobjet A sur chaque cote
	float penX1 = bMaxX - aMinX; // penetration si B est a droite de A
	float penX2 = aMaxX - bMinX; // penetration si A est a droite de B

	float penY1 = bMaxY - aMinY;
	float penY2 = aMaxY - bMinY;

	float penZ1 = bMaxZ - aMinZ;
	float penZ2 = aMaxZ - bMinZ;

	// On prend la penetration minimale pour chaque axe
	float penX = (penX1 < penX2) ? penX1 : penX2;
	float penY = (penY1 < penY2) ? penY1 : penY2;
	float penZ = (penZ1 < penZ2) ? penZ1 : penZ2;

	// On trouve l'axe ou la penetration est la moins importante
	float minPenetration = penX;
	DirectX::XMVECTOR correction = DirectX::XMVectorSet((penX1 < penX2 ? -penX1 : penX2), 0.0f, 0.0f, 0.0f);

	if (penY < minPenetration)
	{
		minPenetration = penY;
		correction = DirectX::XMVectorSet(0.0f, (penY1 < penY2 ? -penY1 : penY2), 0.0f, 0.0f);
	}
	if (penZ < minPenetration)
	{
		minPenetration = penZ;
		correction = DirectX::XMVectorSet(0.0f, 0.0f, (penZ1 < penZ2 ? -penZ1 : penZ2), 0.0f);
	}

	// On peut choisir d'appliquer la moitie de la correction a chaque objet
	//DirectX::XMVECTOR correction = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	return DirectX::XMVectorScale(correction, 1.f);
}