#include "pch.h"

#include "SceneTest.h"

#include "EntityManager.h"
#include "InitDirect3DApp.h"
#include "InputManager.h"
#include "CameraSystem.h"
#include "AssetManager.h"
#include "MovementManager.h"


void SceneTest::CreateDefaultBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* newIceBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newIceBlock);
	mpEntityManager->AddComponent<MeshComponent>(newIceBlock);
	mpEntityManager->AddComponent<HealthComponent>(newIceBlock);
	mpEntityManager->AddComponent<ColliderComponent>(newIceBlock);

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[newIceBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"IceTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Health_ID)
		{
			HealthComponent* health = static_cast<HealthComponent*>(comp);
			health->currentHealth = 20;
			health->maxHealth = 20;
		}
	}
}

void SceneTest::OnInitialize()
{
	// Sound
	std::string basePath = AssetManager::GetExecutablePath();

	std::string beamPath = basePath + "res\\bubblebeam.wav";
	std::string beamPlayerPath = basePath + "res\\BeamPlayer.wav";
	AssetManager::AddSound("beam", beamPath);
	AssetManager::AddSound("beamPlayer", beamPlayerPath);

	// Music
	std::string electroPath = basePath + "res\\ElectroMusic.wav";
	std::string albatorPath = basePath + "res\\AlbatorOST.wav";
	AssetManager::AddMusic("electro", electroPath);
	AssetManager::AddMusic("albator", albatorPath);

	AssetManager::GetMusic("electro").play();
	AssetManager::GetMusic("electro").setVolume(30);

	// Player
	Entity* player = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(player);
	mpEntityManager->AddComponent<MeshComponent>(player);
	mpEntityManager->AddComponent<VelocityComponent>(player);
	mpEntityManager->AddComponent<CameraComponent>(player);
	mpEntityManager->AddComponent<AttackComponent>(player);
	mpEntityManager->AddComponent<ColliderComponent>(player);
	mpEntityManager->AddComponent<PlayerComponent>(player);

	mpEntityManager->AddComponent<HealthComponent>(player);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[player->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"PlayerTexture"; // On assigne la texture
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(1.0f, 1.0f, 1.0f);
		}
		if (component->ID == Collider_ID)
		{
			ColliderComponent* collider = static_cast<ColliderComponent*>(component);
			collider->m_isSolid = true;
			collider->m_isDynamic = true;
		}
		if (component->ID == Player_ID)
		{
			OutputDebugString(L"PlayerCreated !\n");
		}
		if (component->ID == Attack_ID)
		{
			AttackComponent* attack = static_cast<AttackComponent*>(component);
			attack->projectileTexture = L"BlueBeamTexture";
			attack->attackCooldown = 0.1f;
			attack->damage = 2;

			attack->projectileSpeed = 1;
			attack->projectileSizeX = 0.2f;
			attack->projectileSizeY = 0.2f;
			attack->projectileSizeZ = 1.0f;
		}
		if (component->ID == Health_ID)
		{
			HealthComponent* health = static_cast<HealthComponent*>(component);
			health->currentHealth = 100;
			health->maxHealth = 100;
		}
	}
	mp_playerEntity = player;

	Entity* entityIceBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(entityIceBlock);
	mpEntityManager->AddComponent<MeshComponent>(entityIceBlock);
	mpEntityManager->AddComponent<HealthComponent>(entityIceBlock);
	mpEntityManager->AddComponent<ColliderComponent>(entityIceBlock);

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityIceBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"IceTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(1.0f, 1.0f, 1.0f);
			transform->m_transform.Move(5.0f, 0.0f, 0.0f);
		}
		if (comp->ID == Health_ID)
		{
			HealthComponent* health = static_cast<HealthComponent*>(comp);
			health->currentHealth = 100;
			health->maxHealth = 100;
		}
		if (comp->ID == Collider_ID)
		{
			ColliderComponent* collider = static_cast<ColliderComponent*>(comp);
			collider->m_isSolid = true;
		}
	}
	mp_iceBlockEntity = entityIceBlock;

	Entity* entitySkyBox = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(entitySkyBox);
	mpEntityManager->AddComponent<MeshComponent>(entitySkyBox);

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entitySkyBox->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSkyBoxCube();
			mesh->m_textureID = L"SkyBox";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(1000, 1000, 1000);
			transform->m_transform.Move(0, 0, 0);
		}
	}

	// 2
	Entity* floor = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(floor);
	mpEntityManager->AddComponent<MeshComponent>(floor);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[floor->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"WallTexture"; // On assigne la texture
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(200.f, 1.0f, 200.f);
			transform->m_transform.Move(0.0f, 0.0f, -1.0f);
		}
	}

	for (int i = 1; i < 10; i++)
	{
		for (int j = 1; j < 10; j++)
		{
			float posX = 10 * i - 50;
			float posY = 10 * j;
			CreateDefaultBlock(8, 8, 8, posX, posY, 0);
		}
	}

	compteur = 50;
	compteur2 = 150;


	mp_camera->SetFPS();

}

void SceneTest::OnUpdate()
{
	TransformComponent* transform = nullptr;
	CameraComponent* camComponent = nullptr;
	ColliderComponent* collider = nullptr;

	for (auto* component : mpEntityManager->GetComponentsTab()[mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Camera_ID)
		{
			camComponent = static_cast<CameraComponent*>(component);
		}
		if (component->ID == Collider_ID)
		{
			collider = static_cast<ColliderComponent*>(component);
		}
	}
	InputManager::UpdateMouse(GetActiveWindow());
	int deltaX = InputManager::GetMouseDeltaX();
	int deltaY = InputManager::GetMouseDeltaY();

	// Sensibilite de la souris
	const float sensitivity = 0.005f;
	if (InputManager::GetKeyIsPressed(MK_LBUTTON))
	{
		// Mettre a jour la rotation de la camera en fonction du delta
		//camComponent->m_cameraTransform.Rotation(0.0f, deltaY * sensitivity, deltaX * sensitivity);
		//transform->m_transform;
	}

	//if (InputManager::GetKeyIsPressed('D')) velComponent->vx = 50.f;
	//if (InputManager::GetKeyIsPressed('Q')) velComponent->vx = -50.f;

	//if (InputManager::GetKeyIsPressed('Z')) velComponent->vz = 50.f;
	//if (InputManager::GetKeyIsPressed('S')) velComponent->vz = -50.f;

	//if (InputManager::GetKeyIsPressed(VK_SPACE)) velComponent->vy = 50.f;
	//if (InputManager::GetKeyIsPressed('E')) velComponent->vy = -50.f;

	//if (InputManager::GetKeyIsPressed(VK_SHIFT))
	//{
	//	velComponent->vx *= 2;
	//	velComponent->vy *= 2;
	//	velComponent->vz *= 2;
	//}
	/*
	if (InputManager::GetKeyIsPressed('Q')) transform->m_transform.Move(0.0f, -0.4f, 0.0f);
	if (InputManager::GetKeyIsPressed('D')) transform->m_transform.Move(0.0f, 0.4, 0.0f);

	if (InputManager::GetKeyIsPressed('Z')) transform->m_transform.Move(0.4, 0.0f, 0.0f);
	if (InputManager::GetKeyIsPressed('S')) transform->m_transform.Move(-0.4f, 0.0f, 0.0f);

	if (InputManager::GetKeyIsPressed('A')) transform->m_transform.Move(0.0f, 0.0f, 0.4);
	if (InputManager::GetKeyIsPressed('E')) transform->m_transform.Move(0.0f, 0.0f, -0.4f);*/

	float posY = transform->m_transform.GetPositionY();

	if (transform != nullptr && camComponent != nullptr)
	{

	}

	if (transform)
	{
		transform->m_transform.ResetRoll();
	}

	if (InputManager::GetKeyIsPressed(MK_RBUTTON))
	{
		AttackComponent* attack = nullptr;
		auto& playerComponents = mp_gameManager->GetEntityManager()->GetComponentsTab()[mp_playerEntity->tab_index]->vec_components;
		for (auto* component : playerComponents)
		{
			if (component->ID == Attack_ID)
			{
				attack = static_cast<AttackComponent*>(component);
				break;
			}
		}
		if (attack)
		{
			attack->attackRequested = true;
			AssetManager::PlayLocalSound("beamPlayer");
		}
	}
	if (InputManager::GetKeyIsPressed('W'))
	{
		TransformComponent* transform = nullptr;
		auto& playerComponents = mp_gameManager->GetEntityManager()->GetComponentsTab()[mp_playerEntity->tab_index]->vec_components;
		for (auto* component : playerComponents)
		{
			if (component->ID == Transform_ID)
			{
				transform = static_cast<TransformComponent*>(component);
				break;
			}
		}
		if (transform)
		{
			transform->m_transform.ResetRotation();
		}
	}
}

void SceneTest::OnClose()
{
	Close();
}
