#include "pch.h"
#include "SunOrbitSystem.h"
#include "InitDirect3DApp.h"

void SunOrbitSystem::Initialize(InitDirect3DApp* gameManager)
{
    mp_gameManager = gameManager;
}

void SunOrbitSystem::Update(float deltaTime)
{
    EntityManager* entityManager = mp_gameManager->GetEntityManager();

    for (Entity* entity : entityManager->GetEntityTab())
    {
        if (!entity)
            continue;

        if (!entityManager->HasComponent(entity, COMPONENT_SUNORBIT))
            continue;

        SunOrbitComponent* sunOrbit = nullptr;
        TransformComponent* transform = nullptr;

        auto& compTab = entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
        for (auto* comp : compTab)
        {
            if (comp->ID == SunOrbit_ID)
                sunOrbit = static_cast<SunOrbitComponent*>(comp);
            if (comp->ID == Transform_ID)
                transform = static_cast<TransformComponent*>(comp);
        }

        if (sunOrbit && transform)
        {
            sunOrbit->currentAngle += sunOrbit->angularSpeed * deltaTime;

            //float x = sunOrbit->orbitCenter.x + sunOrbit->orbitRadius * cosf(sunOrbit->currentAngle);
            //float z = sunOrbit->orbitCenter.z + sunOrbit->orbitRadius * sinf(sunOrbit->currentAngle);
            ////float y = sunOrbit->orbitCenter.y;
            //float y = sunOrbit->orbitCenter.y + z * sinf(sunOrbit->tiltAngle);
            //z = z * cosf(sunOrbit->tiltAngle);

            //transform->m_transform.vPosition = { x, y, z };
            //transform->m_transform.UpdateMatrix();

            // Calcul de la position de base dans le plan horizontal
            float baseX = sunOrbit->orbitRadius * cosf(sunOrbit->currentAngle);
            float baseZ = sunOrbit->orbitRadius * sinf(sunOrbit->currentAngle);
            float baseY = 0.0f;

            // Rotation autour de l'axe X par tiltAngle
            float newY = -baseZ * sinf(sunOrbit->tiltAngle); // note : baseY est 0
            float newZ = baseZ * cosf(sunOrbit->tiltAngle);

            float x, y, z;

            if (sunOrbit->entityOrbitAround == nullptr)
            {
                // Ajout du centre de l'orbite
                x = sunOrbit->orbitCenter.x + baseX;
                y = sunOrbit->orbitCenter.y + newY;
                z = sunOrbit->orbitCenter.z + newZ;
            }
            else
            {
                float entityCenterX;
                float entityCenterY;
                float entityCenterZ;
                
                auto& compTab = entityManager->GetComponentsTab()[sunOrbit->entityOrbitAround->tab_index]->vec_components;
                for (auto* comp : compTab)
                {
                    if (comp->ID == Transform_ID) 
                    {
                        TransformComponent* transformBoss = static_cast<TransformComponent*>(comp);
                        entityCenterX = transformBoss->m_transform.vPosition.x;
                        entityCenterY = transformBoss->m_transform.vPosition.y;
                        entityCenterZ = transformBoss->m_transform.vPosition.z;
                    }
                }

                x = entityCenterX + baseX;
                y = entityCenterY + newY;
                z = entityCenterZ + newZ;
            }


            transform->m_transform.vPosition = { x, y, z };
            transform->m_transform.UpdateMatrix();
        }
    }
}
