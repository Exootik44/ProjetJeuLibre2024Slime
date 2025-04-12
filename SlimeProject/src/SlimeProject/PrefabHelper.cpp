#include "pch.h"
#include "PrefabHelper.h"

#include "EntityManager.h"
#include "InitDirect3DApp.h"
#include "SceneDemo.h"

#include "Components.h" 
#include "Enemy.h" 
#include "SceneObject.h" 

Entity* PrefabHelper::CreatePlayer(float size, float posX, float posY, float posZ, int health, float jumpForce, float speed, float weight, float highlightIntensity)
{
	// Entity Player
	Entity* entityPlayer = mpEntityManager->CreateEntity();
	entityPlayer->m_tag = Player_TAG;

	mpEntityManager->AddComponent<PlayerComponent>(entityPlayer);
	mpEntityManager->AddComponent<TransformComponent>(entityPlayer);
	mpEntityManager->AddComponent<MeshComponent>(entityPlayer);
	mpEntityManager->AddComponent<VelocityComponent>(entityPlayer);
	mpEntityManager->AddComponent<CameraComponent>(entityPlayer);
	mpEntityManager->AddComponent<AttackComponent>(entityPlayer);
	mpEntityManager->AddComponent<ColliderComponent>(entityPlayer);
	// mpEntityManager->AddComponent<LightComponent>(entityPlayer);
	mpEntityManager->AddComponent<HighlightComponent>(entityPlayer);
	mpEntityManager->AddComponent<GravityComponent>(entityPlayer);
	mpEntityManager->AddComponent<HealthComponent>(entityPlayer);

	float playerSize = size;
	float playerJumpForce = jumpForce;
	float playerSpeed = speed;

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityPlayer->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"PlayerTexture";
		}
		/*if (component->ID == Light_ID)
		{
			LightComponent* light = static_cast<LightComponent*>(component);
			light->type = LightType::Point;
			light->Position = { 0.0f, 0.0f, 0.0f };
			light->Color = { 5.0f, 5.0f, 5.0f };
			light->ConstantAtt = 5.f;
			light->LinearAtt = 0.09f;
			light->QuadraticAtt = 0.032f;
		}*/
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(playerSize, playerSize, playerSize);
			transform->m_transform.vPosition = { posX , posY, posZ };
			transform->m_transform.m_respawnPosition = transform->m_transform.vPosition;
		}
		if (component->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(component);
			//gravity->m_gravityApplied = true;
			gravity->m_weight = weight;
			gravity->m_jumpForce = playerJumpForce;
		}
		if (component->ID == Camera_ID)
		{
			CameraComponent* cam = static_cast<CameraComponent*>(component);
			cam->m_cameraTransform.Scale(1.0f, 1.0f, 1.0f);
			cam->m_cameraTransform.Rotation(0.0, 5.0, 0.0);
		}
		if (component->ID == Attack_ID)
		{
			AttackComponent* attack = static_cast<AttackComponent*>(component);
			attack->projectileTexture = L"PlayerBeamTexture";
			attack->attackCooldown = 0.1f;
			attack->damage = 5;

			attack->projectileSpeed = 100;
			attack->projectileSizeX = 0.2f;
			attack->projectileSizeY = 0.2f;
			attack->projectileSizeZ = 1.0f;
		}
		if (component->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(component);
			highlight->isHighlighted = true;
			highlight->intensity = highlightIntensity;
		}
		if (component->ID == Health_ID)
		{
			HealthComponent* health = static_cast<HealthComponent*>(component);
			health->maxHealth = 100;
			health->currentHealth = 1;
		}
	}
	// Player & statemachine
	mp_player = new Player;
	mp_player->Init(mp_gameManager, mpEntityManager, mp_scene);
	mp_player->SetPlayerEntity(entityPlayer);
	mp_player->SetScale(playerSize);
	mp_player->SetJumpForce(playerJumpForce);
	mp_player->SetSpeed(playerSpeed);

	return entityPlayer;
}

void PrefabHelper::CreatePnj(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	// Entity Player
	Entity* entityPnj = mpEntityManager->CreateEntity();
	entityPnj->m_tag = Pnj_TAG;

	mpEntityManager->AddComponent<TransformComponent>(entityPnj);
	mpEntityManager->AddComponent<MeshComponent>(entityPnj);
	mpEntityManager->AddComponent<ColliderComponent>(entityPnj);
	mpEntityManager->AddComponent<HighlightComponent>(entityPnj);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityPnj->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"FireTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (component->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(component);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}

	}
}
void PrefabHelper::CreateDolem(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, std::string name, std::string textureName, std::vector<std::string> chattingText)
{
	Entity* entityPnj = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(entityPnj);
	mpEntityManager->AddComponent<MeshComponent>(entityPnj);
	mpEntityManager->AddComponent<ColliderComponent>(entityPnj);
	mpEntityManager->AddComponent<SceneObjectComponent>(entityPnj);
	mpEntityManager->AddComponent<PnjComponent>(entityPnj);


	entityPnj->m_tag = Pnj_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[entityPnj->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			std::wstring wTextureName(textureName.begin(), textureName.end());
			mesh->m_textureID = wTextureName;
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Pnj_ID) 
		{
			PnjComponent* pnj = static_cast<PnjComponent*>(comp);
			pnj->m_name = name;
			pnj->m_textTextureList = chattingText;
		}
	}
}
void PrefabHelper::CreateDolem(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, std::string name, std::string textureName, std::vector<std::string> chattingText,bool hasQuest, int collectibleNB, std::string completQuestText)
{
	Entity* entityPnj = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(entityPnj);
	mpEntityManager->AddComponent<MeshComponent>(entityPnj);
	mpEntityManager->AddComponent<ColliderComponent>(entityPnj);
	mpEntityManager->AddComponent<SceneObjectComponent>(entityPnj);
	mpEntityManager->AddComponent<PnjComponent>(entityPnj);

	entityPnj->m_tag = Pnj_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[entityPnj->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			std::wstring wTextureName(textureName.begin(), textureName.end());
			mesh->m_textureID = wTextureName;
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Pnj_ID)
		{
			PnjComponent* pnj = static_cast<PnjComponent*>(comp);
			pnj->m_name = name;
			pnj->m_textTextureList = chattingText;
			pnj->m_hasQuest = hasQuest;
			pnj->m_nbCollectible = collectibleNB;
			pnj->m_compleQuestTextTexture = completQuestText;
		}
	}
}
Entity* PrefabHelper::CreateSun(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	// Entity sun
	Entity* entitySun = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(entitySun);
	mpEntityManager->AddComponent<MeshComponent>(entitySun);
	mpEntityManager->AddComponent<VelocityComponent>(entitySun);
	mpEntityManager->AddComponent<LightComponent>(entitySun);
	mpEntityManager->AddComponent<HighlightComponent>(entitySun);
	mpEntityManager->AddComponent<SunOrbitComponent>(entitySun);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entitySun->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"FireTexture";
		}
		if (component->ID == Light_ID)
		{
			LightComponent* light = static_cast<LightComponent*>(component);
			light->type = LightType::Point;
			light->Position = { 0.0f, 0.0f, 0.0f };
			light->Color = { 150.0f, 140.0f, 150.0f };
			light->ConstantAtt = 1.f;
			light->LinearAtt = 0.0000001f;
			light->QuadraticAtt = 0.000002f;
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (component->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(component);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
		if (component->ID == SunOrbit_ID)
		{
			SunOrbitComponent* sunOrbit = static_cast<SunOrbitComponent*>(component);
			sunOrbit->orbitCenter = { 0.f, 5000.f, 0.f };
			sunOrbit->orbitRadius = 15000.f;
			sunOrbit->angularSpeed = 0.05f;
			sunOrbit->currentAngle = 0.f;
		}
	}
	return entitySun;
}

Entity* PrefabHelper::CreateBoss(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	// Entity Boss
	
	//Core
	Entity* entityBoss = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(entityBoss);
	mpEntityManager->AddComponent<MeshComponent>(entityBoss);
	mpEntityManager->AddComponent<VelocityComponent>(entityBoss);
	mpEntityManager->AddComponent<HighlightComponent>(entityBoss);
	mpEntityManager->AddComponent<ColliderComponent>(entityBoss);
	mpEntityManager->AddComponent<EnnemyComponent>(entityBoss);
	//mpEntityManager->AddComponent<SunOrbitComponent>(entitySun);

	entityBoss->m_tag = Boss_TAG;

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityBoss->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"BossTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (component->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(component);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
		//if (component->ID == SunOrbit_ID)
		//{
		//	SunOrbitComponent* sunOrbit = static_cast<SunOrbitComponent*>(component);
		//	sunOrbit->orbitCenter = { 0.f, 5000.f, 0.f };
		//	sunOrbit->orbitRadius = 15000.f;
		//	sunOrbit->angularSpeed = 0.05f;
		//	sunOrbit->currentAngle = 0.f;
		//}
	}

	int nbCorpParts = 20;

	for (int i = 0; i < nbCorpParts; ++i)
	{
		// Generation d'un angle aleatoire entre 0 et 2π
		float randomAngle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (2.0f * 3.14);
		float randStart = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (180);

		CreateBossBody(100, 100, 100, 1.f, randomAngle, 2, randStart, entityBoss);
	}

	//CreateBossBody(100, 100, 100, 1.f, 3.14 / 2, 2, entitySun);
	//CreateBossBody(100, 100, 100, 1.f, 0, 2, entitySun);
	//CreateBossBody(100, 100, 100, 1.f, 5, 2, entitySun);
	//CreateBossBody(100, 100, 100, 1.f, -10, -2, entitySun);
	return entityBoss;
}

void PrefabHelper::CreateBossBody(float sizeX, float sizeY, float sizeZ, float intensity,float rotAngle,float rotSpeed,float rotStart, Entity* boss)
{
	// Entity Boss
	//body part
	Entity* entityBody = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(entityBody);
	mpEntityManager->AddComponent<MeshComponent>(entityBody);
	mpEntityManager->AddComponent<VelocityComponent>(entityBody);
	mpEntityManager->AddComponent<SunOrbitComponent>(entityBody);
	
	TransformComponent* transformBoss = nullptr;
	
	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[boss->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transformBoss = static_cast<TransformComponent*>(component);
		}
	}

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityBody->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"BossBallTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(0, 0, 0);
		}
		if (component->ID == SunOrbit_ID)
		{
			SunOrbitComponent* sunOrbit = static_cast<SunOrbitComponent*>(component);
			float posx = transformBoss->m_transform.vPosition.x;
			float posy = transformBoss->m_transform.vPosition.y;
			float posz = transformBoss->m_transform.vPosition.z;

			sunOrbit->orbitCenter = { posx,posy,posz };
			sunOrbit->orbitRadius = 400.f;
			sunOrbit->angularSpeed = 2.f;
			sunOrbit->currentAngle = rotStart;
			sunOrbit->tiltAngle = rotAngle;
			sunOrbit->entityOrbitAround = boss;
		}
	}
}

Entity* PrefabHelper::CreateSkyBox(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity, std::string textureName)
{
	Entity* entitySkyBox = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(entitySkyBox);
	mpEntityManager->AddComponent<MeshComponent>(entitySkyBox);
	mpEntityManager->AddComponent<HighlightComponent>(entitySkyBox);
	entitySkyBox->m_tag = Skybox_TAG;

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entitySkyBox->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSkyBoxSphere();

			std::wstring wTextureName(textureName.begin(), textureName.end());
			mesh->m_textureID = wTextureName;
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posY, posZ, posZ);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
	}

	return entitySkyBox;
}

void PrefabHelper::CreateAltar(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	Entity* newBoxBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newBoxBlock);
	mpEntityManager->AddComponent<MeshComponent>(newBoxBlock);
	mpEntityManager->AddComponent<ColliderComponent>(newBoxBlock);
	mpEntityManager->AddComponent<HighlightComponent>(newBoxBlock);
	mpEntityManager->AddComponent<SceneObjectComponent>(newBoxBlock);
	newBoxBlock->m_tag = Altar_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[newBoxBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"altarTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
	}

	Entity* newBox = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newBox);
	mpEntityManager->AddComponent<MeshComponent>(newBox);
	mpEntityManager->AddComponent<ColliderComponent>(newBox);
	mpEntityManager->AddComponent<HighlightComponent>(newBox);
	mpEntityManager->AddComponent<SceneObjectComponent>(newBox);
	newBox->m_tag = Altar_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[newBox->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"DoorTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX*2, sizeY/4, sizeZ*2);
			transform->m_transform.Move(posZ, posX, posY- sizeY/2);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
	}
}

void PrefabHelper::CreateBoxBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health, float intensity)
{
	Entity* newBoxBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newBoxBlock);
	mpEntityManager->AddComponent<MeshComponent>(newBoxBlock);
	mpEntityManager->AddComponent<ColliderComponent>(newBoxBlock);
	mpEntityManager->AddComponent<HighlightComponent>(newBoxBlock);
	mpEntityManager->AddComponent<SceneObjectComponent>(newBoxBlock);
	mpEntityManager->AddComponent<GravityComponent>(newBoxBlock);
	newBoxBlock->m_tag = SceneObject_TAG;

	if (health != 0)
		mpEntityManager->AddComponent<HealthComponent>(newBoxBlock);
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[newBoxBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"BoxTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Health_ID)
		{
			HealthComponent* healthComp = static_cast<HealthComponent*>(comp);
			healthComp->maxHealth = healthComp->currentHealth = health;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
		if (comp->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(comp);
			gravity->m_weight = 30;
			gravity->m_jumpForce = 0;
		}
	}
	//ObjectStateMachine
	SceneObject* newSceneObject = new SceneObject;
	newSceneObject->SetObjectEntity(newBoxBlock);
	newSceneObject->SetPlayerEntity(mp_player->mp_playerEntity);
	newSceneObject->Init(mp_gameManager);
	newSceneObject->SetListIndex(mp_scene->GetSceneObjCount());
	mp_scene->IncrementeSceneObjCount(1);
	mp_scene->GetSceneObjectList().push_back(newSceneObject);
}
void PrefabHelper::CreateBoxEatable(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	Entity* newBoxBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newBoxBlock);
	mpEntityManager->AddComponent<MeshComponent>(newBoxBlock);
	mpEntityManager->AddComponent<ColliderComponent>(newBoxBlock);
	mpEntityManager->AddComponent<HighlightComponent>(newBoxBlock);
	//mpEntityManager->AddComponent<SceneObjectComponent>(newBoxBlock);
	mpEntityManager->AddComponent<GravityComponent>(newBoxBlock);
	newBoxBlock->m_tag = EatableObject_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[newBoxBlock->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"BoxTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
		if (comp->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(comp);
			gravity->m_weight = 30;
			gravity->m_jumpForce = 0;
		}
	}
	//ObjectStateMachine
	SceneObject* newSceneObject = new SceneObject;
	newSceneObject->SetObjectEntity(newBoxBlock);
	newSceneObject->SetPlayerEntity(mp_player->mp_playerEntity);
	newSceneObject->Init(mp_gameManager);
	newSceneObject->SetListIndex(mp_scene->GetSceneObjCount());
	mp_scene->IncrementeSceneObjCount(1);
	mp_scene->GetSceneObjectList().push_back(newSceneObject);
}

void PrefabHelper::CreateWater(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float tilingFactor, std::string textureName)
{
	Entity* floor = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(floor);
	mpEntityManager->AddComponent<MeshComponent>(floor);
	mpEntityManager->AddComponent<ColliderComponent>(floor);
	mpEntityManager->AddComponent<TexturePropertiesComponent>(floor);
	mpEntityManager->AddComponent<SceneObjectComponent>(floor);
	mpEntityManager->AddComponent<HighlightComponent>(floor);

	floor->m_tag = DeathZone_TAG;

	// Parcours des composants pour configurer le sol
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[floor->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();


			std::wstring wTextureName(textureName.begin(), textureName.end());
			mesh->m_textureID = wTextureName;
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == TextureProperties_ID)
		{
			TexturePropertiesComponent* textureProperties = static_cast<TexturePropertiesComponent*>(comp);
			textureProperties->tilingFactor = tilingFactor;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 0.8f;
		}
	}
}

void PrefabHelper::CreateFloor2(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float tilingFactorGrass, float tilingFactorFloor)
{
	Entity* floor = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(floor);
	mpEntityManager->AddComponent<MeshComponent>(floor);
	mpEntityManager->AddComponent<ColliderComponent>(floor);
	mpEntityManager->AddComponent<TexturePropertiesComponent>(floor);
	mpEntityManager->AddComponent<SceneObjectComponent>(floor);
	floor->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le sol
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[floor->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"GroundTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == TextureProperties_ID)
		{
			TexturePropertiesComponent* textureProperties = static_cast<TexturePropertiesComponent*>(comp);
			textureProperties->tilingFactor = tilingFactorFloor;
		}
	}
	CreateGrass(sizeX, 2.f, sizeZ, posX, posY + sizeY * 0.5 + 0.5f, posZ, 1.f, tilingFactorGrass);
}

void PrefabHelper::CreateWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* wall = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(wall);
	mpEntityManager->AddComponent<MeshComponent>(wall);
	mpEntityManager->AddComponent<ColliderComponent>(wall);
	mpEntityManager->AddComponent<SceneObjectComponent>(wall);
	wall->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[wall->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"wallV2Texture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

void PrefabHelper::CreateTreeTrunk(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* TreeTrunk = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(TreeTrunk);
	mpEntityManager->AddComponent<ColliderComponent>(TreeTrunk);
	mpEntityManager->AddComponent<MeshComponent>(TreeTrunk);
	mpEntityManager->AddComponent<SceneObjectComponent>(TreeTrunk);
	TreeTrunk->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[TreeTrunk->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"WoodTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}
void PrefabHelper::CreateTreeBranch(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* TreeBranch = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(TreeBranch);
	mpEntityManager->AddComponent<ColliderComponent>(TreeBranch);
	mpEntityManager->AddComponent<MeshComponent>(TreeBranch);
	mpEntityManager->AddComponent<SceneObjectComponent>(TreeBranch);
	TreeBranch->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[TreeBranch->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"WoodTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

void PrefabHelper::CreateGreenCube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	Entity* GreenCub = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(GreenCub);
	mpEntityManager->AddComponent<MeshComponent>(GreenCub);
	mpEntityManager->AddComponent<ColliderComponent>(GreenCub);
	mpEntityManager->AddComponent<HighlightComponent>(GreenCub);
	mpEntityManager->AddComponent<SceneObjectComponent>(GreenCub);
	GreenCub->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[GreenCub->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"LeafTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
	}
}

void PrefabHelper::CreateGrass(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity, float tilingFactorGrass)
{
	Entity* Grass = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(Grass);
	mpEntityManager->AddComponent<MeshComponent>(Grass);
	mpEntityManager->AddComponent<ColliderComponent>(Grass);
	mpEntityManager->AddComponent<HighlightComponent>(Grass);
	mpEntityManager->AddComponent<SceneObjectComponent>(Grass);
	mpEntityManager->AddComponent<TexturePropertiesComponent>(Grass);
	Grass->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[Grass->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"GrassTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
		if (comp->ID == TextureProperties_ID)
		{
			TexturePropertiesComponent* textureProperties = static_cast<TexturePropertiesComponent*>(comp);
			textureProperties->tilingFactor = tilingFactorGrass;
		}
	}
}

void PrefabHelper::CreateStickyWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* StickyWall = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(StickyWall);
	mpEntityManager->AddComponent<MeshComponent>(StickyWall);
	mpEntityManager->AddComponent<ColliderComponent>(StickyWall);
	StickyWall->m_tag = StickySurface_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[StickyWall->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"StickyTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

Enemy* PrefabHelper::CreateEnnemy(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{

	Entity* ennemy = mpEntityManager->CreateEntity();
	ennemy->m_tag = Enemy_TAG;

	mpEntityManager->AddComponent<TransformComponent>(ennemy);
	mpEntityManager->AddComponent<MeshComponent>(ennemy);
	mpEntityManager->AddComponent<ColliderComponent>(ennemy);
	mpEntityManager->AddComponent<EnnemyComponent>(ennemy);
	mpEntityManager->AddComponent<AttackComponent>(ennemy);
	mpEntityManager->AddComponent<HealthComponent>(ennemy);
	mpEntityManager->AddComponent<HighlightComponent>(ennemy);
	mpEntityManager->AddComponent<VelocityComponent>(ennemy);
	mpEntityManager->AddComponent<GravityComponent>(ennemy);

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
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Health_ID)
		{
			HealthComponent* healthComp = static_cast<HealthComponent*>(comp);
			healthComp->maxHealth = healthComp->currentHealth = 100;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 3.0f;
		}
		if (comp->ID == Attack_ID)
		{
			AttackComponent* bulletProperty = static_cast<AttackComponent*>(comp);
			bulletProperty->damage = 10;
		}
		if (comp->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(comp);
			//gravity->m_gravityApplied = true;
			gravity->m_weight = 50;
			gravity->m_jumpForce = 500;
		}
		if (comp->ID == Velocity_ID)
		{
			VelocityComponent* vel = static_cast<VelocityComponent*>(comp);
			vel->vx = 0.0f;
			vel->vy = 0.0f;
			vel->vz = 0.0f;
		}
	}

	////ObjectStateMachine
	Enemy* newEnemy = new Enemy;
	newEnemy->SetEnemyEntity(ennemy);
	newEnemy->SetPlayerEntity(mp_player->mp_playerEntity);
	newEnemy->SetStartPosY(20);
	newEnemy->Init(mp_gameManager);

	return newEnemy;
}
void PrefabHelper::CreateProjectile(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	EntityManager* entityManager = mp_gameManager->GetEntityManager();

	Entity* meteor = entityManager->CreateEntity();
	meteor->m_tag = Projectile_TAG;

	entityManager->AddComponent<TransformComponent>(meteor);
	entityManager->AddComponent<MeshComponent>(meteor);
	entityManager->AddComponent<ColliderComponent>(meteor);
	entityManager->AddComponent<AttackComponent>(meteor);
	//entityManager->AddComponent<HealthComponent>(meteor);
	entityManager->AddComponent<HighlightComponent>(meteor);
	entityManager->AddComponent<VelocityComponent>(meteor);
	entityManager->AddComponent<GravityComponent>(meteor);

	for (auto& comp : entityManager->GetComponentToAddTab()[meteor->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"projectileRockTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Health_ID)
		{
			HealthComponent* healthComp = static_cast<HealthComponent*>(comp);
			healthComp->maxHealth = healthComp->currentHealth = 1;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 3.0f;
		}
		if (comp->ID == Attack_ID)
		{
			AttackComponent* bulletProperty = static_cast<AttackComponent*>(comp);
			bulletProperty->damage = 10;
		}
		if (comp->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(comp);
			//gravity->m_gravityApplied = true;
			gravity->m_weight = 50;
			gravity->m_jumpForce = 500;
		}
		if (comp->ID == Velocity_ID)
		{
			VelocityComponent* vel = static_cast<VelocityComponent*>(comp);
			vel->vx = 0.0f;
			vel->vy = 0.0f;
			vel->vz = 0.0f;
		}
	}
	//ObjectStateMachine
	Projectile* newProjectile = new Projectile;
	newProjectile->SetObjectEntity(meteor);
	newProjectile->SetPlayerEntity(mp_player->mp_playerEntity);
	newProjectile->SetStartPosY(posY);
	newProjectile->Init(mp_gameManager);
	mp_player->GetProjectileList().push_back(newProjectile);

}
void PrefabHelper::CreateSphere(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* Sphere = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(Sphere);
	mpEntityManager->AddComponent<MeshComponent>(Sphere);
	mpEntityManager->AddComponent<ColliderComponent>(Sphere);
	mpEntityManager->AddComponent<HighlightComponent>(Sphere);

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[Sphere->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"GrassTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 3.0f;
		}
	}
}

void PrefabHelper::CreateSphereArtefact(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity)
{
	Entity* Sphere = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(Sphere);
	mpEntityManager->AddComponent<MeshComponent>(Sphere);
	mpEntityManager->AddComponent<ColliderComponent>(Sphere);
	mpEntityManager->AddComponent<HighlightComponent>(Sphere);
	Sphere->m_tag = QuestObject_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[Sphere->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"IceTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = intensity;
		}
	}
}

void PrefabHelper::CreateJumpPad(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* jumpPad = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(jumpPad);
	mpEntityManager->AddComponent<MeshComponent>(jumpPad);
	mpEntityManager->AddComponent<ColliderComponent>(jumpPad);
	mpEntityManager->AddComponent<GravityComponent>(jumpPad);
	jumpPad->m_tag = PowerJumping_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[jumpPad->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"JumpadTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Gravity_ID)
		{
			GravityComponent* gravity = static_cast<GravityComponent*>(comp);
			gravity->m_weight = 30;
			gravity->m_jumpForce = 0;
		}
	}
	//ObjectStateMachine
	SceneObject* newSceneObject = new SceneObject;
	newSceneObject->SetObjectEntity(jumpPad);
	newSceneObject->SetPlayerEntity(mp_player->mp_playerEntity);
	newSceneObject->Init(mp_gameManager);
	newSceneObject->SetListIndex(mp_scene->GetSceneObjCount());
	mp_scene->IncrementeSceneObjCount(1);
	mp_scene->GetSceneObjectList().push_back(newSceneObject);
}

void PrefabHelper::CreateBouncySurface(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* bouncySurface = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(bouncySurface);
	mpEntityManager->AddComponent<MeshComponent>(bouncySurface);
	mpEntityManager->AddComponent<ColliderComponent>(bouncySurface);
	bouncySurface->m_tag = BouncySurface_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[bouncySurface->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"BlueBeamTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

void PrefabHelper::CreateButton(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, Entity* doorToLink)
{
	Entity* button = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(button);
	mpEntityManager->AddComponent<MeshComponent>(button);
	mpEntityManager->AddComponent<ColliderComponent>(button);
	mpEntityManager->AddComponent<ButtonComponent>(button);
	button->m_tag = Button_TAG;


	// Configuration des composants
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[button->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"ButtonTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Button_ID)
		{
			ButtonComponent* buttonComp = static_cast<ButtonComponent*>(comp);
			buttonComp->linkedDoor = doorToLink;
			buttonComp->activated = false;
		}
	}
}

Entity* PrefabHelper::CreateDoor(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, DirectX::XMFLOAT3 targetPosition)
{
	Entity* door = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(door);
	mpEntityManager->AddComponent<MeshComponent>(door);
	mpEntityManager->AddComponent<ColliderComponent>(door);
	mpEntityManager->AddComponent<DoorComponent>(door);
	door->m_tag = SceneObject_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[door->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"DoorTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Door_ID)
		{
			DoorComponent* doorComp = static_cast<DoorComponent*>(comp);
			doorComp->isMoving = false;
			doorComp->targetPosition = targetPosition;
			doorComp->moveSpeed = 100.f;
		}
	}
	return door;
}

Entity* PrefabHelper::CreateBridge(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, DirectX::XMFLOAT3 targetPosition)
{
	Entity* bridge = mpEntityManager->CreateEntity();

	mpEntityManager->AddComponent<TransformComponent>(bridge);
	mpEntityManager->AddComponent<MeshComponent>(bridge);
	mpEntityManager->AddComponent<ColliderComponent>(bridge);
	mpEntityManager->AddComponent<DoorComponent>(bridge);
	bridge->m_tag = SceneObject_TAG;

	for (auto& comp : mpEntityManager->GetComponentToAddTab()[bridge->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"bridgeTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
		if (comp->ID == Door_ID)
		{
			DoorComponent* doorComp = static_cast<DoorComponent*>(comp);
			doorComp->isMoving = false;
			doorComp->targetPosition = targetPosition;
			doorComp->moveSpeed = 100.f;
		}
	}
	return bridge;
}


void PrefabHelper::CreateTree1(float scale, float posX, float posY, float posZ)
{
	// === Test arbre === 
	CreateTreeTrunk(10.f * scale, 80.f * scale, 10.f * scale, posX + 10.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateGreenCube(50.f * scale, 50.f * scale, 50.f * scale, posX + 10.f * scale, posY + 100.f * scale, posZ + 10.f * scale, 1.2f);

	CreateTreeBranch(30.f * scale, 5.f * scale, 5.f * scale, posX + 30.f * scale, posY + 60.f * scale, posZ + 10.f * scale);
	CreateGreenCube(20.f * scale, 20.f * scale, 20.f * scale, posX + 55.f * scale, posY + 60.f * scale, posZ + 10.f * scale, 1.2f);

	CreateTreeBranch(20.f * scale, 5.f * scale, 5.f * scale, posX + -5.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateGreenCube(20.f * scale, 20.f * scale, 20.f * scale, posX + -25.f * scale, posY + 40.f * scale, posZ + 10.f * scale, 1.2f);

	CreateTreeBranch(5.f * scale, 5.f * scale, 20.f * scale, posX + 10.f * scale, posY + 50.f * scale, posZ + -5.f * scale);
	CreateGreenCube(20.f * scale, 20.f * scale, 20.f * scale, posX + 10.f * scale, posY + 50.f * scale, posZ + -25.f * scale, 1.2f);
}

void PrefabHelper::CreateTree2(float scale, float posX, float posY, float posZ)
{
	// === Test arbre === 
	CreateTreeTrunk(10.f * scale, 80.f * scale, 10.f * scale, posX + 10.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateGreenCube(50.f * scale, 50.f * scale, 50.f * scale, posX + 10.f * scale, posY + 100.f * scale, posZ + 10.f * scale, 1.2f);

	CreateTreeBranch(10.f * scale, 5.f * scale, 5.f * scale, posX + 20.f * scale, posY + 60.f * scale, posZ + 10.f * scale);
	CreateGreenCube(10.f * scale, 10.f * scale, 10.f * scale, posX + 30.f * scale, posY + 60.f * scale, posZ + 10.f * scale, 1.2f);

	CreateTreeBranch(10.f * scale, 5.f * scale, 5.f * scale, posX + 0.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateGreenCube(20.f * scale, 20.f * scale, 20.f * scale, posX + -15.f * scale, posY + 40.f * scale, posZ + 10.f * scale, 1.2f);
}

void PrefabHelper::CreateTree3(float scale, float posX, float posY, float posZ)
{
	// === Test arbre === 
	CreateTreeTrunk(10.f * scale, 60.f * scale, 10.f * scale, posX + 0.f * scale, posY + 30.f * scale, posZ + 0.f * scale);
	CreateGreenCube(30.f * scale, 70.f * scale, 30.f * scale, posX + 0.f * scale, posY + 90.f * scale, posZ + 0.f * scale, 1.2f);
}