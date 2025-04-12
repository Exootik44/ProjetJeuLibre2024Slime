#include "pch.h"
#include "SceneMVP.h"

#include "EntityManager.h"
#include "InitDirect3DApp.h"
#include "InputManager.h"
#include "CameraSystem.h"
#include "AssetManager.h"


void SceneMVP::OnInitialize()
{
	// Sound
	std::string basePath = AssetManager::GetExecutablePath();

	std::string beamPath = basePath + "res\\bubblebeam.wav";
	std::string beamPlayerPath = basePath + "res\\BeamPlayer.wav";
	AssetManager::AddSound("beam", beamPath);
	AssetManager::AddSound("beamPlayer", beamPlayerPath);

	// Create Entity 1 = player
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
		mpEntityManager->AddComponent<LightComponent>(entityPlayer);
		mpEntityManager->AddComponent<HighlightComponent>(entityPlayer);
		mpEntityManager->AddComponent<GravityComponent>(entityPlayer);
		mpEntityManager->AddComponent<HealthComponent>(entityPlayer);

		float playerSize = 30;
		float playerJumpForce = 300;
		float playerSpeed = 5;

		for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityPlayer->tab_index]->vec_components)
		{
			if (component->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(component);
				mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
				mesh->m_textureID = L"PlayerTexture";
			}
			if (component->ID == Light_ID)
			{
				LightComponent* light = static_cast<LightComponent*>(component);
				light->type = LightType::Point;
				light->Position = { 0.0f, 0.0f, 0.0f };
				light->Color = { 5.0f, 5.0f, 5.0f };
				light->ConstantAtt = 5.f;
				light->LinearAtt = 0.09f;
				light->QuadraticAtt = 0.032f;
			}
			if (component->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(component);
				transform->m_transform.Scale(playerSize, playerSize, playerSize);
				transform->m_transform.vPosition = { 0.0f , 15.0f, 0.0f };
				transform->m_transform.m_respawnPosition = transform->m_transform.vPosition;
			}
			if (component->ID == Gravity_ID)
			{
				GravityComponent* gravity = static_cast<GravityComponent*>(component);
				//gravity->m_gravityApplied = true;
				gravity->m_weight = 50;
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
				attack->damage = 2;

				attack->projectileSpeed = 100;
				attack->projectileSizeX = 0.2f;
				attack->projectileSizeY = 0.2f;
				attack->projectileSizeZ = 1.0f;
			}
			if (component->ID == Highlight_ID)
			{
				HighlightComponent* highlight = static_cast<HighlightComponent*>(component);
				highlight->isHighlighted = true;
				highlight->intensity = 3.0f;
			}
			if (component->ID == Health_ID)
			{
				HealthComponent* health = static_cast<HealthComponent*>(component);
				health->maxHealth = 100;
				health->currentHealth = 1;
			}
		}
		mp_playerEntity = entityPlayer;

		// Player & statemachine
		mp_player = new Player;
		//mp_player->Init(mp_gameManager,mpEntityManager);
		mp_player->SetPlayerEntity(mp_playerEntity);
		mp_player->SetScale(playerSize);
		mp_player->SetJumpForce(playerJumpForce);
		mp_player->SetSpeed(playerSpeed);
	}

	//PNJ QUEST
	Entity* entityPnj = mpEntityManager->CreateEntity();
	entityPnj->m_tag = Pnj_TAG;

	mpEntityManager->AddComponent<TransformComponent>(entityPnj);
	mpEntityManager->AddComponent<MeshComponent>(entityPnj);
	mpEntityManager->AddComponent<ColliderComponent>(entityPnj);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityPnj->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"PnjTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(20, 20, 20);
			transform->m_transform.Move(30,70,10);
			//transform->m_transform.m_respawnPosition = transform->m_transform.vPosition;
		}
	}

	//APPLE QUEST
	Entity* entityApple = mpEntityManager->CreateEntity();
	entityApple->m_tag = QuestObject_TAG;

	mpEntityManager->AddComponent<TransformComponent>(entityApple);
	mpEntityManager->AddComponent<MeshComponent>(entityApple);
	mpEntityManager->AddComponent<ColliderComponent>(entityApple);

	for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityApple->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"AppleTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.Scale(20, 20, 20);
			transform->m_transform.Move(60, 70, 10);
			//transform->m_transform.m_respawnPosition = transform->m_transform.vPosition;
		}
	}

	//Skybox
	Entity* entitySkyBox = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(entitySkyBox);
	mpEntityManager->AddComponent<MeshComponent>(entitySkyBox);
	entitySkyBox->m_tag = Skybox_TAG;

	for (auto& comp : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entitySkyBox->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSkyBoxSphere();
			mesh->m_textureID = L"SkyboxTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(559000, 559000, 559000);
			transform->m_transform.Move(0, 0, 0);
		}
	}


	// UI ELEMENTS

	// === Creation projectile ===
	//CreateProjectile(5, 5, 5, 0, 3, -30);
	CreateProjectile(15, 15, 15, -30, 8, -30);
	CreateProjectile(25, 25, 25, 40, 13, -30);
	// === Creation enemy ===
	//CreateEnnemy(5, 5, 5, 70, 3, -30);
	/*CreateEnnemy(10, 10, 10, 30, 5, -30);
	CreateEnnemy(15, 15, 15, 0, 7.5, -30);
	CreateEnnemy(20, 20, 20, -30, 10, -30);
	CreateEnnemy(25, 25, 25, -60, 12.5, -30);

	CreateEnnemy(5, 5, 5, 50, 3, 0);
	CreateEnnemy(5, 5, 5, 50, 3, 30);*/

	CreateJumpPad(20, 10, 20, 30, 30, 150);
	CreateBouncySurface(20, 10, 200, -60, 30, 150);

	// === Zone de d�part ===
	CreateFloor(200.f, 50.f, 200.f, 0.f, -25.f, 0.f);
	CreateWall(1.f, 20.f, 200.f, -100.f, 10.f, 0.f);
	CreateWall(1.f, 20.f, 80.f, 100.f, 10.f, 60.f);
	CreateWall(1.f, 20.f, 80.f, 100.f, 10.f, -60.f);
	CreateWall(200.f, 20.f, 1.f, 0.f, 10.f, -100.f);
	CreateWall(80.f, 20.f, 1.f, -60.f, 10.f, 100.f);
	CreateWall(80.f, 20.f, 1.f, 60.f, 10.f, 100.f);

	// === CHEMIN FACILE (a gauche) (chemin + orienté sur l'absorption et la regurgitation d'item peut etre)
	// === Pont de sortie ===
	CreateFloor(30.f, 1.f, 100.f, 0.f, -1.f, 150.f);

	// === Nouvelle zone (zone du pont) ===
	CreateFloor(100.f, 100.f, 100.f, 0.f, -50.f, 250.f);
	CreateBoxBlock(20.f, 20.f, 20.f, 30.f, 10.f, 250.f, 0, 2);
	CreateBoxBlock(15.f, 15.f, 15.f, 30.f, 7.5f, 232.f, 0, 2);
	CreateBoxBlock(10.f, 10.f, 10.f, 17.f, 5.0f, 232.f, 0, 2);

	// === Zone surelevee accessible par escalade ===
	CreateFloor(100.f, 10.f, 100.f, 70.f, 20.f, 330.f);
	CreateBoxBlock(20.f, 20.f, 20.f, 70.f, 20.f, 410.f, 0, 1);
	CreateBoxBlock(20.f, 20.f, 20.f, 100.f, 20.f, 440.f, 0, 2);
	CreateBoxBlock(20.f, 20.f, 20.f, 70.f, 20.f, 470.f, 0, 2);

	// === Zone surelevee accessible par le parcours des box ===
	CreateFloor(100.f, 20.f, 100.f, 70.f, 30.f, 550.f);
	CreateStickyWall(100.f, 100.f, 5.f, 70.f, 80.f, 600.f);

	// === Zone surelevee accessible par escalade ===
	CreateFloor(100.f, 20.f, 200.f, 70.f, 110.f, 700.f);
	CreateEnnemy(5, 5, 5, 70.f, 132.5f, 750.f);
	CreateEnnemy(5, 5, 5, 80.f, 132.5f, 750.f);
	CreateEnnemy(5, 5, 5, 90.f, 132.5f, 750.f);
	CreateEnnemy(5, 5, 5, 100.f, 132.5f, 750.f);
	CreateEnnemy(5, 5, 5, 110.f, 132.5f, 750.f);
	CreateEnnemy(5, 5, 5, 70.f, 132.5f, 770.f);
	CreateEnnemy(5, 5, 5, 80.f, 132.5f, 770.f);
	CreateEnnemy(5, 5, 5, 90.f, 132.5f, 770.f);
	CreateEnnemy(5, 5, 5, 100.f, 132.5f, 770.f);
	CreateEnnemy(5, 5, 5, 110.f, 132.5f, 770.f);
	CreateEnnemy(5, 5, 5, 70.f, 132.5f, 720.f);
	CreateEnnemy(5, 5, 5, 80.f, 132.5f, 720.f);
	CreateEnnemy(5, 5, 5, 90.f, 132.5f, 720.f);
	CreateEnnemy(5, 5, 5, 100.f, 132.5f, 720.f);
	CreateEnnemy(5, 5, 5, 110.f, 132.5f, 720.f);
	CreateEnnemy(10, 10, 10, 60.f, 135.f, 750.f);
	CreateEnnemy(10, 10, 10, 45.f, 135.f, 750.f);
	CreateEnnemy(15, 15, 15, 30.f, 137.5f, 750.f);

	// === StickySurface atteignable uniquement quand on a absorber suffisament d'éléments ===
	CreateStickyWall(300.f, 5.f, 30.f, 250.f, 190.5f, 650.f);

	CreateFloor(400.f, 100.f, 400.f, 400.f, 100.f, 600.f);
	CreateFloor(100.f, 20.f, 30.f, 500.f, 110.f, 550.f);
	CreateWall(20.f, 600.f, 140.f, 450.f, 160.f, 625.f);
	CreateWall(20.f, 600.f, 40.f, 450.f, 160.f, 525.f);

	// === CHEMIN DUR (a droite)
	CreateBoxBlock(20.f, 20.f, 20.f, 130.f, -5.f, -10.f, 0, 2);
	CreateBoxBlock(20.f, 20.f, 20.f, 160.f, 0.f, 30.f, 0, 2);
	CreateBoxBlock(20.f, 20.f, 20.f, 190.f, 10.f, -30.f, 0, 2);
	CreateBoxBlock(20.f, 20.f, 20.f, 230.f, 15.f, 10.f, 0, 2);

	CreateFloor(140.f, 50.f, 140.f, 340.f, 0.f, 10.f);

	CreateStickyWall(120.f, 200.f, 5.f, 500.f, 50.f, -30.f);
	CreateStickyWall(120.f, 200.f, 5.f, 600.f, 50.f, 30.f);
	CreateStickyWall(120.f, 200.f, 5.f, 700.f, 50.f, -30.f);
	CreateStickyWall(5.f, 200.f, 120.f, 800.f, 50.f, 30.f);

	CreateFloor(140.f, 50.f, 140.f, 730.f, 80.f, 160.f);
	CreateBoxBlock(20.f, 20.f, 20.f, 680.f, 115.f, 200, 0, 1);
	CreateBoxBlock(15.f, 15.f, 15.f, 697.5f, 112.5f, 190.f, 0, 1);
	CreateBoxBlock(10.f, 10.f, 10.f, 690.f, 110.0f, 177.5f, 0, 1);

	CreateStickyWall(30.f, 5.f, 300.f, 680.f, 150.f, 350.f);

	CreateFloor(200.f, 50.f, 200.f, 700.f, 70.f, 550.f);


	// Lumiere directionnel : 
	{
		Entity* sunEntity = mpEntityManager->CreateEntity();
		mpEntityManager->AddComponent<LightComponent>(sunEntity);

		for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[sunEntity->tab_index]->vec_components)
		{
			if (component->ID == Light_ID)
			{
				LightComponent* light = static_cast<LightComponent*>(component);
				light->type = LightType::Directional;
				light->Direction = { 0.57735f, 0.57735f, -0.57735f };
				light->Color = { 1.0f, 1.0f, 0.95f };
			}
		}

	}

	mp_camera->SetTPS();
}

void SceneMVP::OnUpdate()
{
	TransformComponent* transform = nullptr;
	VelocityComponent* velComponent = nullptr;
	CameraComponent* camComponent = nullptr;
	GravityComponent* gravity = nullptr;

	PlayerComponent* playerComponent = nullptr;

	for (auto* component : mpEntityManager->GetComponentsTab()[mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Camera_ID)
		{
			camComponent = static_cast<CameraComponent*>(component);
		}
		if (component->ID == Player_ID)
		{
			playerComponent = static_cast<PlayerComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
	}

	for (auto& entity : mpEntityManager->GetEntityTab())
	{
		if (entity == nullptr)
			continue; // L'entite est nulle, on passe a la suivante

		// Pointeurs pour les composants
		TransformComponent* objTransform = nullptr;
		SceneObjectComponent* sceneObj = nullptr;

		// On parcourt les composants de l'entite
		for (auto* component : mpEntityManager->GetComponentsTab()[entity->tab_index]->vec_components)
		{
			if (component->ID == SceneObject_ID)
				sceneObj = static_cast<SceneObjectComponent*>(component);

			if (component->ID == Transform_ID)
				objTransform = static_cast<TransformComponent*>(component);
		}

		// Si les deux composants sont trouves, on applique la rotation
		if (sceneObj && objTransform)
		{
			objTransform->m_transform.Rotation(sceneObj->speedRotX, sceneObj->speedRotY, sceneObj->speedRotZ);
		}
	}

	// Mettez a jour la souris en passant le handle de la fenetre
	InputManager::UpdateMouse(GetActiveWindow());

	int deltaX = InputManager::GetMouseDeltaX();
	int deltaY = InputManager::GetMouseDeltaY();

	// Sensibilite de la souris
	const float sensitivity = 0.005f;

	if (InputManager::GetKeyIsPressed(MK_LBUTTON))
	{
		camComponent->m_cameraTransform.Rotation(0.0f, deltaY * sensitivity, deltaX * sensitivity);
		camComponent->freeCam = true;
	}
	else
	{
		/*camComponent->freeCam = false;*/
		// Mettre a jour la rotation de la camera en fonction du delta
		transform->m_transform.Rotation(0.0f, 0.0f, deltaX * sensitivity);
		//camComponent->m_cameraTransform.matrix = transform->m_transform.matrix;
	}

	//StateMachine
	mp_player->Update(mp_gameManager->GetDeltaTime());
	


	//	DEBUG						AFFICHE DELTATIME
	/*wchar_t title[256];
	swprintf_s(title, 256, L"lag meters: %f", mp_gameManager->GetDeltaTime());
	SetWindowText(GetActiveWindow(), title);*/


	wchar_t title[256];
	swprintf_s(title, 256, L"player state: %f", mp_gameManager->GetDeltaTime());
	SetWindowText(GetActiveWindow(), title);

	//posy
	//std::string stateStr = std::to_string(mp_player->GetLastPosY());
	//std::wstring wStateStr(stateStr.begin(), stateStr.end());

	//std::string posyStr = std::to_string(transform->m_transform.vPosition.y);
	//std::wstring wPosyStr(stateStr.begin(), stateStr.end());

	//wchar_t title[256];
	//swprintf_s(title, 256, L"player posy: %ls, transformPosY: %ls", wStateStr.c_str(), wPosyStr.c_str());
	//SetWindowText(GetActiveWindow(), title);
}

void SceneMVP::OnClose()
{
	Close();
}

void SceneMVP::CreateBoxBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health, float intensity)
{
	Entity* newBoxBlock = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(newBoxBlock);
	mpEntityManager->AddComponent<MeshComponent>(newBoxBlock);
	mpEntityManager->AddComponent<ColliderComponent>(newBoxBlock);
	mpEntityManager->AddComponent<HighlightComponent>(newBoxBlock);
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
	}
}

void SceneMVP::CreateFloor(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* floor = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(floor);
	mpEntityManager->AddComponent<MeshComponent>(floor);
	mpEntityManager->AddComponent<ColliderComponent>(floor);
	floor->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le sol
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[floor->tab_index]->vec_components)
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
	}
}

void SceneMVP::CreateWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* wall = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(wall);
	mpEntityManager->AddComponent<MeshComponent>(wall);
	mpEntityManager->AddComponent<ColliderComponent>(wall);
	wall->m_tag = SceneObject_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[wall->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"WallTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

void SceneMVP::CreateStickyWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
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
			mesh->m_textureID = L"IceTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}
void SceneMVP::CreateJumpPad(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	Entity* jumpPad = mpEntityManager->CreateEntity();
	mpEntityManager->AddComponent<TransformComponent>(jumpPad);
	mpEntityManager->AddComponent<MeshComponent>(jumpPad);
	mpEntityManager->AddComponent<ColliderComponent>(jumpPad);
	jumpPad->m_tag = PowerJumping_TAG;

	// Parcours des composants pour configurer le mur
	for (auto& comp : mpEntityManager->GetComponentToAddTab()[jumpPad->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateCube();
			mesh->m_textureID = L"FireTexture";
		}
		if (comp->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(comp);
			transform->m_transform.Scale(sizeX, sizeY, sizeZ);
			transform->m_transform.Move(posZ, posX, posY);
		}
	}
}

void SceneMVP::CreateBouncySurface(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
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

void SceneMVP::CreateEnnemy(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{

	Entity* ennemy = mpEntityManager->CreateEntity();
	//ennemy->m_tag = Enemy_TAG;
	ennemy->m_tag = EatableObject_TAG;

	mpEntityManager->AddComponent<TransformComponent>(ennemy);
	mpEntityManager->AddComponent<MeshComponent>(ennemy);
	mpEntityManager->AddComponent<ColliderComponent>(ennemy);
	mpEntityManager->AddComponent<EnnemyComponent>(ennemy);
	mpEntityManager->AddComponent<AttackComponent>(ennemy);
	mpEntityManager->AddComponent<HealthComponent>(ennemy);
	mpEntityManager->AddComponent<HighlightComponent>(ennemy);

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
			healthComp->maxHealth = healthComp->currentHealth = 1;
		}
		if (comp->ID == Highlight_ID)
		{
			HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
			highlight->isHighlighted = true;
			highlight->intensity = 3.0f;
		}
	}
}
void SceneMVP::CreateProjectile(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
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

	for (auto& comp : entityManager->GetComponentToAddTab()[meteor->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(comp);
			mesh->m_mesh = mp_gameManager->GetFactory()->CreateSphere();
			mesh->m_textureID = L"MeteorTexture";
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
	}
}
void SceneMVP::CreateSphere(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
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

void SceneMVP::CreateTree1(float scale, float posX, float posY, float posZ)
{
	// === Test arbre === 
	CreateWall(10.f * scale, 80.f * scale, 10.f * scale, posX + 10.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateSphere(50.f * scale, 50.f * scale, 50.f * scale, posX + 10.f * scale, posY + 105.f * scale, posZ + 10.f * scale);

	CreateWall(30.f * scale, 5.f * scale, 5.f * scale, posX + 30.f * scale, posY + 60.f * scale, posZ + 10.f * scale);
	CreateSphere(20.f * scale, 20.f * scale, 20.f * scale, posX + 55.f * scale, posY + 60.f * scale, posZ + 10.f * scale);

	CreateWall(20.f * scale, 5.f * scale, 5.f * scale, posX + -5.f * scale, posY + 40.f * scale, posZ + 10.f * scale);
	CreateSphere(20.f * scale, 20.f * scale, 20.f * scale, posX + -25.f * scale, posY + 40.f * scale, posZ + 10.f * scale);

	CreateWall(5.f * scale, 5.f * scale, 20.f * scale, posX + 10.f * scale, posY + 50.f * scale, posZ + -5.f * scale);
	CreateSphere(20.f * scale, 20.f * scale, 20.f * scale, posX + 10.f * scale, posY + 50.f * scale, posZ + -25.f * scale);
}
