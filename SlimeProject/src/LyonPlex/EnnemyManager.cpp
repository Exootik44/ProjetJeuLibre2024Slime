#include "pch.h"
#include "EnnemyManager.h"

EnnemyManager::EnnemyManager()
{
}

void EnnemyManager::Initialize(InitDirect3DApp* app)
{
	mp_gameManager = app;
    m_nbEnnemy = 0;
    m_nbEnnemyToSpawn = 0;
    m_waveNb = 1;
    m_startNextWave = true;
    m_gameStart = false;
	mp_entityManager = app->GetEntityManager();
}

void EnnemyManager::Update()
{
    if (!m_gameStart) 
    {
        if (InputManager::GetKeyIsPressed(VK_RETURN)) { m_gameStart = true; };

        // Skip wave at start
        if (InputManager::GetKeyIsPressed('1')) { m_waveNb = 1; m_gameStart = true; };
        if (InputManager::GetKeyIsPressed('2')) { m_waveNb = 2; m_gameStart = true; };
        if (InputManager::GetKeyIsPressed('3')) { m_waveNb = 3; m_gameStart = true; };
    }
    else
    {
        WaveSystem();
        EnnemyAttackSystem();
    }
}

void EnnemyManager::EnnemyAttackSystem()
{
    // Premiere boucle : rechercher la transform du joueur
    mp_playerTransform = nullptr;
    for (auto& entity : mp_entityManager->GetEntityTab())
    {
        if (entity == nullptr)
            continue;

        auto components = mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
        for (auto* comp : components)
        {
            if (comp->ID == Player_ID)
            {
                mp_player = entity;
                for (auto* pcomp : components)
                {
                    if (pcomp->ID == Transform_ID)
                    {
                        mp_playerTransform = static_cast<TransformComponent*>(pcomp);
                        break;
                    }
                }
            }
            if (mp_playerTransform != nullptr)
                break;
        }
        if (mp_playerTransform != nullptr)
            break;
    }

    // Si aucun joueur n'a ete trouve, on sort
    if (mp_playerTransform == nullptr)
    {
        OutputDebugString(L"No PlayerTransform found, skipping enemy update\n");
        return;
    }

    // Deuxieme boucle : mettre a jour les ennemis
    for (auto& entity : mp_entityManager->GetEntityTab())
    {
        if (entity == nullptr)
            continue;

        auto components = mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components;
        bool isEnemy = false;
        TransformComponent* enemyTransform = nullptr;
        for (auto* comp : components)
        {
            if (comp->ID == Ennemy_ID)
                isEnemy = true;
            if (comp->ID == Transform_ID)
                enemyTransform = static_cast<TransformComponent*>(comp);
        }

        if (isEnemy && enemyTransform != nullptr)
        {
            // Faire suivre l'ennemi vers le joueur
            enemyTransform->m_transform.FollowTarget(mp_playerTransform->m_transform.GetPositionF3(), 0.2f);
            enemyTransform->m_transform.LookAt(mp_playerTransform->m_transform.GetPositionF3());

            AttackComponent* attack = nullptr;
            auto& playerComponents = mp_gameManager->GetEntityManager()->GetComponentsTab()[entity->tab_index]->vec_components;
            for (auto* component : playerComponents)
            {
                if (component->ID == Attack_ID)
                {
                    attack = static_cast<AttackComponent*>(component);
                    attack->projectileTexture = L"EnnemyBeamTexture";
                    break;
                }
            }
            if (attack)
            {
                // Declencher l'attaque en definissant le flag et en indiquant la cible
                float minAtkCooldown = 0.5f;
                float maxAtkCooldown = 1.f;

                attack->attackRequested = true;
                attack->attackCooldown = minAtkCooldown + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (maxAtkCooldown - minAtkCooldown);
                attack->damage = 2;

                attack->projectileSpeed = 80.f;
                attack->projectileSizeX = 0.2f;
                attack->projectileSizeY = 0.2f;
                attack->projectileSizeZ = 1.0f;
            }
        }
    }
}

void EnnemyManager::SpawnEnnemy(float posX, float posY, float posZ)
{
    Entity* ennemy = mp_entityManager->CreateEntity();
    mp_entityManager->AddComponent<TransformComponent>(ennemy);
    mp_entityManager->AddComponent<MeshComponent>(ennemy);
    mp_entityManager->AddComponent<ColliderComponent>(ennemy);
    mp_entityManager->AddComponent<EnnemyComponent>(ennemy);
    mp_entityManager->AddComponent<AttackComponent>(ennemy);
    mp_entityManager->AddComponent<HealthComponent>(ennemy);
    mp_entityManager->AddComponent<HighlightComponent>(ennemy);

    for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[ennemy->tab_index]->vec_components)
    {
        if (comp->ID == Mesh_ID)
        {
            MeshComponent* mesh = static_cast<MeshComponent*>(comp);
            mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
            mesh->m_textureID = L"DroneTexture";
        }
        if (comp->ID == Transform_ID)
        {
            TransformComponent* transform = static_cast<TransformComponent*>(comp);
            transform->m_transform.Scale(5.f, 5.f, 5.f);
            transform->m_transform.Move(posZ, posX, posY);
        }
        if (comp->ID == Health_ID)
        {
            HealthComponent* healthComp = static_cast<HealthComponent*>(comp);
            healthComp->maxHealth = healthComp->currentHealth = 10;
        }
        if (comp->ID == Highlight_ID)
        {
            HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
            highlight->isHighlighted = true;
            highlight->intensity = 3.0f;
        }
    }
}

void EnnemyManager::SpawnEnnemyBoss(float posX, float posY, float posZ)
{
    Entity* ennemy = mp_entityManager->CreateEntity();
    mp_entityManager->AddComponent<TransformComponent>(ennemy);
    mp_entityManager->AddComponent<MeshComponent>(ennemy);
    mp_entityManager->AddComponent<ColliderComponent>(ennemy);
    mp_entityManager->AddComponent<EnnemyComponent>(ennemy);
    mp_entityManager->AddComponent<AttackComponent>(ennemy);
    mp_entityManager->AddComponent<HealthComponent>(ennemy);
    mp_entityManager->AddComponent<HighlightComponent>(ennemy);

    for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[ennemy->tab_index]->vec_components)
    {
        if (comp->ID == Mesh_ID)
        {
            MeshComponent* mesh = static_cast<MeshComponent*>(comp);
            mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
            mesh->m_textureID = L"DroneTexture";
        }
        if (comp->ID == Transform_ID)
        {
            TransformComponent* transform = static_cast<TransformComponent*>(comp);
            transform->m_transform.Scale(9.f, 9.f, 9.f);
            transform->m_transform.Move(posZ, posX, posY);
        }
        if (comp->ID == Health_ID)
        {
            HealthComponent* healthComp = static_cast<HealthComponent*>(comp);
            healthComp->maxHealth = 200;
            healthComp->currentHealth = 200;
        }
        if (comp->ID == Highlight_ID)
        {
            HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
            highlight->isHighlighted = true;
            highlight->intensity = 4.0f;
        }
    }
}

void EnnemyManager::WaveSystem()
{
    if (m_startNextWave) 
    {
        switch (m_waveNb)
        {
        case 1:
            m_nbEnnemyToSpawn = 5;

            break;
        case 2:
            m_nbEnnemyToSpawn = 10;

            break;
        case 3:
            m_nbEnnemyToSpawn = 20;

            break;
        default:

            break;
        }

        while (m_nbEnnemy < m_nbEnnemyToSpawn)
        {
            float randPosX = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            float randPosY = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            float randPosZ = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            SpawnEnnemy(randPosX, randPosY, randPosZ);
            m_nbEnnemy++;
            OutputDebugString(L"SpawnEnnemy\n");
        }
        if (m_waveNb == 3)
        {
            float randPosX = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            float randPosY = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            float randPosZ = -50 + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (50 - -50);
            SpawnEnnemyBoss(randPosX, randPosY, randPosZ);
        }

        m_startNextWave = false;
    }
    else
    {
        if (m_nbEnnemy <= 0 && m_waveNb < 3)
        {
            m_waveNb++;				
            wchar_t buffer[256];
            swprintf_s(buffer, 256, L"NB waves %d\r\n", m_waveNb);
            OutputDebugString(buffer);
            m_startNextWave = true;
        }
    }
}
