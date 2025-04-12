#include "pch.h"
#include "HealthSystem.h"
#include "InitDirect3DApp.h"
#include "Components.h"
#include "EnnemyManager.h"
#include "AssetManager.h"
#include "ParticleManager.h"

void HealthSystem::Initialize(InitDirect3DApp* gameManager)
{
    mp_gameManager = gameManager;

    // Sound
    std::string basePath = AssetManager::GetExecutablePath();

    std::string explosionPath = basePath + "res\\Explosion.wav";
    AssetManager::AddSound("explosion", explosionPath);
    AssetManager::GetSound("explosion");

}

void HealthSystem::Update(float deltaTime)
{
    EntityManager* entityManager = mp_gameManager->GetEntityManager();
    EnnemyManager* ennemyManager = mp_gameManager->GetEnnemyManager();

    for (Entity* entity : entityManager->GetEntityTab())
    {
        if (!entity)
            continue;

        if (entityManager->HasComponent(entity, COMPONENT_HEALTH))
        {
            HealthComponent* health = nullptr;
            EnnemyComponent* ennemy = nullptr;
            TransformComponent* transform = nullptr;
            auto& compTab = entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
            for (auto* comp : compTab)
            {
                if (comp->ID == Health_ID)
                {
                    health = static_cast<HealthComponent*>(comp);
                }
                if (comp->ID == Ennemy_ID)
                {
                    ennemy = static_cast<EnnemyComponent*>(comp);
                }
                if (comp->ID == Transform_ID)
                {
                    transform = static_cast<TransformComponent*>(comp);
                }
            }
            if (health)
            {
                if (health->currentHealth <= 0)
                {
                    // baisse le compte des ennemies
                    if (ennemy != nullptr) 
                    {
                        ennemyManager->SetNbEnnemy(ennemyManager->GetNbEnnemy() - 1);
                        mp_gameManager->GetParticleManager()->Explosion(transform->m_transform.GetPositionX(), transform->m_transform.GetPositionY(), transform->m_transform.GetPositionZ());
                        AssetManager::PlayLocalSound("explosion");
                    }
                    // detruire l'entity
                    entityManager->ToDestroy(entity);
                }
            }
        }
    }
}

