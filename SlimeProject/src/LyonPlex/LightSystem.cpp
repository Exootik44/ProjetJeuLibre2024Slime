#include "pch.h"
#include "LightSystem.h"
#include "InitDirect3DApp.h"
#include "Components.h"

void LightSystem::Initialize(InitDirect3DApp* gameManager)
{
    mp_gameManager = gameManager;

    // Valeurs par defaut globales
    m_passConstants.AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };

    // Lumiere directionnelle
    m_passConstants.DirLight.Direction = { 0.57735f, -0.57735f, 0.57735f };
    m_passConstants.DirLight.Color = { 0.6f, 0.6f, 0.6f };

    // Lumiere point (omnidirectionnelle)
    m_passConstants.PtLight.Position = { 0.0f, 0.0f, 0.0f };
    m_passConstants.PtLight.Color = { 0.8f, 0.8f, 0.8f };
    m_passConstants.PtLight.ConstantAtt = 1.0f;
    m_passConstants.PtLight.LinearAtt = 0.09f;
    m_passConstants.PtLight.QuadraticAtt = 0.032f;
}

void LightSystem::Update(float deltaTime)
{
    EntityManager* entityManager = mp_gameManager->GetEntityManager();

    bool hasDirectionalLight = false;
    bool hasPointLight = false;

    // Parcourir toutes les entites de la scene
    for (Entity* entity : entityManager->GetEntityTab())
    {
        if (!entity)
            continue;

        // Si l'entite possede un composant lumiere
        if (entityManager->HasComponent(entity, COMPONENT_LIGHT))
        {
            LightComponent* light = nullptr;
            TransformComponent* transform = nullptr;
            auto& compTab = entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
            for (auto* comp : compTab)
            {
                if (comp->ID == Light_ID)
                {
                    light = static_cast<LightComponent*>(comp);
                }
                else if (comp->ID == Transform_ID)
                {
                    transform = static_cast<TransformComponent*>(comp);
                }
            }
            if (light)
            {
                // Si un TransformComponent est present, mettre a jour la position de la lumiere
                if (transform)
                {
                    // Charger la matrice du transform
                    DirectX::XMMATRIX worldMatrix = XMLoadFloat4x4(&transform->m_transform.GetMatrix());
                    // Isoler la rotation en supprimant la translation
                    DirectX::XMFLOAT4X4 worldMat;
                    XMStoreFloat4x4(&worldMat, worldMatrix);
                    worldMat._41 = 0.0f;
                    worldMat._42 = 0.0f;
                    worldMat._43 = 0.0f;
                    DirectX::XMMATRIX rotationMatrix = XMLoadFloat4x4(&worldMat);

                    DirectX::XMVECTOR localOffset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
                    // Transformer l'offset en espace monde
                    DirectX::XMVECTOR worldOffset = DirectX::XMVector3Transform(localOffset, rotationMatrix);
                    DirectX::XMFLOAT3 offset;
                    DirectX::XMStoreFloat3(&offset, worldOffset);

                    // Positionner la lumiere en ajoutant l'offset a la position de l'entite
                    light->Position = transform->m_transform.vPosition;
                    light->Position.x += offset.x;
                    light->Position.y += offset.y;
                    light->Position.z += offset.z;
                }

                // Mise a jour des constantes de passage selon le type de lumiere
                if (light->type == LightType::Point)
                {
                    hasPointLight = true;
                    m_passConstants.PtLight.Position = light->Position;
                    m_passConstants.PtLight.Color = light->Color;
                    m_passConstants.PtLight.ConstantAtt = light->ConstantAtt;
                    m_passConstants.PtLight.LinearAtt = light->LinearAtt;
                    m_passConstants.PtLight.QuadraticAtt = light->QuadraticAtt;
                }
                else if (light->type == LightType::Directional)
                {
                    hasDirectionalLight = true;
                    m_passConstants.DirLight.Direction = light->Direction;
                    m_passConstants.DirLight.Color = light->Color;
                }
            }
        }
    }
    if (hasDirectionalLight == false)
    {
        m_passConstants.DirLight.Direction = { 0.0, 0.0f, 0.0f };
        m_passConstants.DirLight.Color = { 0.0f, 0.0f, 0.0f };
    }
}


