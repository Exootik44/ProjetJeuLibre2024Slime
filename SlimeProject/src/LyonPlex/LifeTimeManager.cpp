#include "pch.h"
#include "LifeTimeManager.h"
#include "InitDirect3DApp.h"
#include "Components.h"

void LifeTimeManager::Initialize(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
}

void LifeTimeManager::Update(float deltaTime)
{
	EntityManager* entityManager = mp_gameManager->GetEntityManager();
	
	for (Entity* entity : entityManager->GetEntityTab())
	{
		if (!entity)
			continue;

		if (entityManager->HasComponent(entity, COMPONENT_LIFETIME))
		{
			LifeTimeComponent* p_lifeTime = nullptr;
			auto& compTab = entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
			for (auto* comp : compTab)
			{
				if (comp->ID == LifeTime_ID)
				{
					p_lifeTime = static_cast<LifeTimeComponent*>(comp);

					if (p_lifeTime->m_lifeTime > 0)
					{
						p_lifeTime->m_lifeTime -= 1 * deltaTime;
					}
					else
					{
						entityManager->ToDestroy(entity);
					}
				}
			}
		}
	}
}
