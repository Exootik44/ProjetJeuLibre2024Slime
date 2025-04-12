#include "pch.h"
#include "Player.h"

//void Player::OnCollision(Entity* entity1, Entity* entity2)
//{
//
//}

Player::Player() : m_lastPosY(10), m_jumpPosY(0), m_scale(0), m_jumpForce(0), m_speed(0), canJump(true)/*, m_lifePoint(1), m_lifePointMax(100)*/
{
}

void Player::Init(InitDirect3DApp* gameManager, EntityManager* entityManager, SceneDemo* scene)
{
	mp_gameManager = gameManager;
	mp_scene = scene;

	// MachineState
	PlayerState newPlayerState;
	m_playerState = newPlayerState;
	m_playerState.mp_gameManager = mp_gameManager;
	mp_entityManager = entityManager;
	m_playerState.mp_player = this;
	m_playerState.Idle();

	// Health UI
	//Entity* entityHpUIBackground = mp_entityManager->CreateEntity();

	//mp_entityManager->AddComponent<UIComponent>(entityHpUIBackground);
	//mp_entityManager->AddComponent<TransformComponent>(entityHpUIBackground);
	//mp_entityManager->AddComponent<MeshComponent>(entityHpUIBackground);

	//for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityHpUIBackground->tab_index]->vec_components)
	//{
	//	if (component->ID == Mesh_ID)
	//	{
	//		MeshComponent* mesh = static_cast<MeshComponent*>(component);
	//		mesh->m_mesh = mp_gameManager->GetFactory()->CreateQuadUI(10,10);
	//		mesh->m_textureID = L"WallTexture";
	//	}
	//	if (component->ID == Transform_ID)
	//	{
	//		TransformComponent* transform = static_cast<TransformComponent*>(component);
	//		//transform->m_transform.Scale(11.0f, 11.0f, 1.0f);
	//		//transform->m_transform.vPosition = { 4.f, 4.f, 0.f };
	//	}
	//}

	//Entity* entityHpUI = mp_entityManager->CreateEntity();

	//mp_entityManager->AddComponent<UIComponent>(entityHpUI);
	//mp_entityManager->AddComponent<TransformComponent>(entityHpUI);
	//mp_entityManager->AddComponent<MeshComponent>(entityHpUI);

	//for (auto& component : mp_gameManager->GetEntityManager()->GetComponentToAddTab()[entityHpUI->tab_index]->vec_components)
	//{
	//	if (component->ID == Mesh_ID)
	//	{
	//		MeshComponent* mesh = static_cast<MeshComponent*>(component);
	//		mesh->m_mesh = mp_gameManager->GetFactory()->CreateQuadUI(5,5);
	//		mesh->m_textureID = L"";
	//	}
	//	if (component->ID == Transform_ID)
	//	{
	//		TransformComponent* transform = static_cast<TransformComponent*>(component);
	//		transform->m_transform.Scale(1.f, 1.f, 1.0f);
	//		transform->m_transform.vPosition = { 5.f, 5.f, 0.f };
	//	}
	//}

	//lifepointUI = entityHpUI;
}

void Player::Update(float deltaTime)
{
	m_playerState.Update(deltaTime);
	int i = 0;
	for (auto projectile : m_projectilesList)
	{
		//if (projectile->mp_objectEntity)
			projectile->Update(mp_gameManager->GetDeltaTime());
	}
	// Ensuite, après la boucle d'update, supprimer les objets marqués

	auto& projectiles = m_projectilesList;
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
		[this](Projectile* proj) {
			if (proj->GetObjectState().m_toDestroy)
			{
				delete proj; // si tu es responsable de la libération mémoire
				IncrementeProjectileCount(-1);
				return true;
			}
			return false;
		}), projectiles.end());
	wchar_t title[256];
	swprintf_s(title, 256, L"player state: %d", m_projectilesList.size());
	SetWindowText(GetActiveWindow(), title);
}

float Player::CheckPosY()
{
	TransformComponent* transform = nullptr;

	for (auto* component : mp_gameManager->GetEntityManager()->GetComponentsTab()[mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
	}

	return GetLastPosY() - transform->m_transform.vPosition.y;
}

