#include "pch.h"
#include "ParticleManager.h"

ParticleManager::ParticleManager()
{
}

ParticleManager::~ParticleManager()
{
	
}

void ParticleManager::Initialize(InitDirect3DApp* app)
{
	mp_gameManager = app;
	mp_entityManager = app->GetEntityManager();
}

void ParticleManager::Update()
{
	for (auto& entity : mp_entityManager->GetEntityTab())
	{
		if (entity == nullptr)
			continue;

		TransformComponent* transform1 = nullptr;
		ColliderComponent* collider1 = nullptr;
		HealthComponent* health1 = nullptr;
		AttackComponent* attack1 = nullptr;
		for (auto* component : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
		{
			if (component->ID == Particle_ID)
			{
			}
		}
	}
}

void ParticleManager::CreateParticle(float startPosX, float startPosY, float startPosZ, float size,  float speedX, float speedY, float speedZ, std::wstring textureName)
{
	Entity* newIceBlock = mp_entityManager->CreateEntity();
	mp_entityManager->AddComponent<TransformComponent>(newIceBlock);
	mp_entityManager->AddComponent<MeshComponent>(newIceBlock);
	mp_entityManager->AddComponent<ParticleComponent>(newIceBlock);
	mp_entityManager->AddComponent<VelocityComponent>(newIceBlock);
	mp_entityManager->AddComponent<LifeTimeComponent>(newIceBlock);
	mp_entityManager->AddComponent<HighlightComponent>(newIceBlock);

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[newIceBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = textureName;
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(size, size, size);
			transform->m_transform.Move(startPosZ, startPosX, startPosY);
			transform->m_transform.Rotation(startPosZ, startPosX, startPosY);
		}
		if (comp->ID == Particle_ID)
		{
			ParticleComponent* particle = static_cast<ParticleComponent*>(comp);
		}
		if (comp->ID == Velocity_ID)
		{
			VelocityComponent* vel = static_cast<VelocityComponent*>(comp);
			vel->vx = speedX;
			vel->vy = speedY;
			vel->vz = speedZ;
		}
		if (comp->ID == LifeTime_ID)
		{
			LifeTimeComponent* p_lifeTime = static_cast<LifeTimeComponent*>(comp);
			p_lifeTime->m_lifeTime = 0.8f;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 4.0f;
		}
	}
}

void ParticleManager::Explosion(float startPosX, float startPosY, float startPosZ)
{
	MakeEffect(startPosX, startPosY, startPosZ, 3, 5, -100, 50, 0.3f, 1, L"FireTexture");
}

void ParticleManager::MakeEffect(float startPosX, float startPosY, float startPosZ, int _minNbPart, int _maxNbPart, int _minSpeed, int _maxSpeed, int minSize, int maxSize, std::wstring textureName)
{
	int randSplashNb = std::rand() % (_maxNbPart - _minNbPart + 1) + _minNbPart;
	wchar_t buffer[256];
	for (int i = 0; i < randSplashNb; i++)
	{
		float randSize = minSize + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
		float randSpeedX = _minSpeed + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (_maxSpeed - _minSpeed);
		float randSpeedY = _minSpeed + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (_maxSpeed - _minSpeed);
		float randSpeedZ = _minSpeed + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (_maxSpeed - _minSpeed);

		CreateParticle(startPosX, startPosY, startPosZ, randSize, randSpeedX, randSpeedY, randSpeedZ, textureName);
	}
}