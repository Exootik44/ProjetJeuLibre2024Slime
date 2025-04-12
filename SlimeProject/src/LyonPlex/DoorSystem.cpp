#include "pch.h"
#include "DoorSystem.h"
#include "Components.h"
#include "InitDirect3DApp.h"
#include "AssetManager.h"

void DoorSystem::Initialize(InitDirect3DApp* gameManager)
{
    m_gameManager = gameManager;
    m_startOpen = false;
}

float DoorSystem::MoveTowards(float current, float target, float step) {
    float difference = target - current;

    if (difference > 0) {
        if (difference < step) {
            return target;
        }
        else {
            return current + step;
        }
    }
    else if (difference < 0) {
        if (-difference < step) {
            return target;
        }
        else {
            return current - step;
        }
    }
    else {
        return target;
    }
}

void DoorSystem::Update(float deltaTime) {
    EntityManager* entityManager = m_gameManager->GetEntityManager();

    for (Entity* entity : entityManager->GetEntityTab()) {
        if (!entity)
            continue;

        DoorComponent* doorComp = nullptr;
        TransformComponent* transform = nullptr;

        for (auto* comp : entityManager->GetComponentsTab()[entity->tab_index]->vec_components) {
            if (comp->ID == Door_ID)
                doorComp = static_cast<DoorComponent*>(comp);
            if (comp->ID == Transform_ID)
                transform = static_cast<TransformComponent*>(comp);
        }

        if (doorComp && transform && doorComp->isMoving) {
            if (!m_startOpen) 
            {
                AssetManager::PlayLocalSound("DoorOpening");
                m_startOpen = true;
            }

            float step = doorComp->moveSpeed * deltaTime;

            transform->m_transform.vPosition.x = MoveTowards(transform->m_transform.vPosition.x, doorComp->targetPosition.x, step);
            transform->m_transform.vPosition.y = MoveTowards(transform->m_transform.vPosition.y, doorComp->targetPosition.y, step);
            transform->m_transform.vPosition.z = MoveTowards(transform->m_transform.vPosition.z, doorComp->targetPosition.z, step);
            transform->m_transform.UpdateMatrix();

            // Si la position cible est atteinte, arreter le mouvement
            if (transform->m_transform.vPosition.x == doorComp->targetPosition.x &&
                transform->m_transform.vPosition.y == doorComp->targetPosition.y &&
                transform->m_transform.vPosition.z == doorComp->targetPosition.z) {
                doorComp->isMoving = false;
                m_startOpen = false;
            }
        }
    }
}
