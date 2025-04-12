#include "pch.h"
#include "StateMachine.h"
#include "Player.h"
#include "Projectile.h"
#include "ColliderManager.h"
#include "SceneObject.h"
#include "SceneDemo.h"
#include "AssetManager.h"

#include <sstream>

bool IsAbove(const TransformComponent& a, const TransformComponent& b);
bool IsBelow(const TransformComponent& a, const TransformComponent& b);
bool AABBIntersect(const TransformComponent& a, const TransformComponent& b);
int CollideFromBelow(Entity* entity, EntityManager* entityManager);
int CollideFromTop(Entity* entity, EntityManager* entityManager);
int CollideWith(Entity* entity, EntityManager* entityManager);
Entity* CollideWithProjectileAttack(Entity* entity, EntityManager* entityManager);
Entity* CollideWithEatable(Entity* entity, EntityManager* entityManager);
Entity* CollideWithTag(ObjetsTAG tag, EntityManager* entityManager, Entity* entity);
void CreateItem(PlayerState& player, std::wstring textureID, float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
void CreateTextBox(PlayerState& player, std::string textureText);
void CreateTextBoss(PlayerState& player, std::string textureText);

// Player
PlayerState::PlayerState() : m_jumpProgress(0), m_jumpTime(2), m_jumpForce(0), m_fallProgress(0), m_chargeProgress(0), m_questTextNb(1), m_questTextNbMax(2), m_questIsCompleted(false), m_questHasStarted(false), m_objectToCollect(0), m_objectCollected(0), m_pnjHasTalked(false),m_startBoss(false), m_bossHp(6), m_bossIsVanquished(false)
{
	//std::cout << "New Gun created with " << mAmmo << " Ammo !" << std::endl;

	m_States[(int)StateType::Idle] = new IdleState();
	m_States[(int)StateType::Jumping] = new JumpingState();
	m_States[(int)StateType::Eating] = new EatingState();
	m_States[(int)StateType::Falling] = new FallingState();
	m_States[(int)StateType::ChargingJump] = new ChargingJumpState();
	m_States[(int)StateType::Moving] = new MovingState();
	m_States[(int)StateType::Sticky] = new StickyState();
	m_States[(int)StateType::Attacking] = new AttackingState();
	m_States[(int)StateType::Talking] = new TalkingState();
	m_States[(int)StateType::Bouncing] = new BouncingState();
	m_States[(int)StateType::PowerJumping] = new PowerJumpingState();
	m_States[(int)StateType::Placing] = new PlacingState();

	m_State = m_States[(int)StateType::Idle];
}

PlayerState::~PlayerState()
{

}

void PlayerState::Update(float deltaTime)
{
	m_State->Update(deltaTime, *this);
}

void PlayerState::StayIdle()
{
	if (TransitionTo(StateType::Idle) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Jump()
{
	if (TransitionTo(StateType::Jumping) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Eat()
{
	if (TransitionTo(StateType::Eating) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Fall()
{
	if (TransitionTo(StateType::Falling) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::ChargeJump()
{
	if (TransitionTo(StateType::ChargingJump) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Move()
{
	if (TransitionTo(StateType::Moving) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Stick()
{
	if (TransitionTo(StateType::Sticky) == false)
		return;
	m_State->Start(*this);
}

void PlayerState::Talk()
{
	if (TransitionTo(StateType::Talking) == false)
		return;
	m_State->Start(*this);
}

bool PlayerState::TransitionTo(StateType newState)
{
	std::ostringstream oss;

	if (m_Transition[(int)m_State->GetStateType()][(int)newState] == 0)
	{
		oss << "[!]Error player is: " << DisplayPlayerState() << std::endl;
		std::string message = oss.str();
		OutputDebugStringA(message.c_str());

		return false;
	}
	else
	{
		m_State = m_States[newState];

		oss << "Player is now: " << DisplayPlayerState() << std::endl;
		std::string message = oss.str();
		OutputDebugStringA(message.c_str());

		return true;
	}
}

std::string PlayerState::DisplayPlayerState()
{
	switch (m_State->GetStateType())
	{
	case PlayerState::StateType::Idle:
		return "idle\n";
		break;
	case PlayerState::StateType::Jumping:
		return "jumping\n";
		break;
	case PlayerState::StateType::Eating:
		return "Eating\n";
		break;
	case PlayerState::StateType::Falling:
		return "falling\n";
		break;
	case PlayerState::StateType::ChargingJump:
		return "chargingJump\n";
		break;
	case PlayerState::StateType::Moving:
		return "moving\n";
		break;
	case PlayerState::StateType::Sticky:
		return "sticky\n";
		break;
	case PlayerState::StateType::Talking:
		return "talking\n";
		break;
	case PlayerState::StateType::Placing:
		return "placingItem\n";
		break;
	default:
		return "NoState\n";
		break;
	}
}

//idle
void IdleState::Start(PlayerState& player)
{
	OutputDebugStringA("imIdling");
}
void IdleState::Update(float deltaTime, PlayerState& player)
{
	TransformComponent* transform = nullptr;
	VelocityComponent* velComponent = nullptr;
	PlayerComponent* playerComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Player_ID)
		{
			playerComponent = static_cast<PlayerComponent*>(component);
		}
	}
	player.mp_player->SetLastPosY(transform->m_transform.vPosition.y);

	if (InputManager::GetKeyIsPressed('D') || InputManager::GetKeyIsPressed('Q') || InputManager::GetKeyIsPressed('Z') || InputManager::GetKeyIsPressed('S'))
	{
		int val = 1;
		//running
		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) 	{ velComponent->vx = player.mp_player->GetSpeed() * 2; val = 2;	}
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) 	{ velComponent->vx = -player.mp_player->GetSpeed() * 2; val = 2; }
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) 	{ velComponent->vz = player.mp_player->GetSpeed() * 2; val = 2; }
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) 	{ velComponent->vz = -player.mp_player->GetSpeed() * 2; val = 2; }
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

		static float cooldown = 0.0f;

		if (cooldown <= 0.0f) {
			AssetManager::PlayLocalSound("Move1");
			cooldown = 0.2f;
		}
		else {
			cooldown -= deltaTime * val;
		}
	}
	//if (InputManager::GetKeyIsPressed('D') || InputManager::GetKeyIsPressed('Q') || InputManager::GetKeyIsPressed('Z') || InputManager::GetKeyIsPressed('S'))
	//{
	//	// PLAY SOUND
	//	player.TransitionTo(PlayerState::StateType::Moving);
	//}


	int colliderTAG = CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());

	if (colliderTAG >= 0)
	{
		if (colliderTAG == PowerJumping_TAG)
		{
			// PLAY SOUND
			player.TransitionTo(PlayerState::StateType::PowerJumping);
		}
		else if (colliderTAG == BouncySurface_TAG)
		{
			// PLAY SOUND
			player.TransitionTo(PlayerState::StateType::Bouncing);
		}
		else if (colliderTAG == Pnj_TAG)
		{
			// PLAY SOUND
			/*OutputDebugStringA("IM A DOLEEEEEEM !");*/
		}
		player.mp_player->SetCanJump(true);
	}
	else if (colliderTAG == -1) // Chute Libre
	{
		player.TransitionTo(PlayerState::StateType::Falling);
	}
	//else
	//	player.mp_player->SetCanJump(true); // sert a limiter le nombre de jumps

	if (InputManager::GetKeyDown('C'))
	{
		player.TransitionTo(PlayerState::StateType::Placing);
	}
	if (InputManager::GetKeyIsPressed(MK_RBUTTON))
	{
		player.TransitionTo(PlayerState::StateType::Eating);
	}
	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		player.TransitionTo(PlayerState::StateType::Attacking);
	}

	colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());

	if (colliderTAG == StickySurface_TAG)
	{
		AssetManager::PlayLocalSound("JumpFromSticky");
		player.TransitionTo(PlayerState::StateType::Sticky);
	}

	// Talk with dolem
	if (CollideWithTag(Pnj_TAG, player.mp_gameManager->GetEntityManager(), player.GetPlayerEntity()))
	{	
		PnjComponent* pnj = nullptr;
		for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[CollideWithTag(Pnj_TAG, player.mp_gameManager->GetEntityManager(), player.GetPlayerEntity())->tab_index]->vec_components)
		{
			if (component->ID == Pnj_ID)
			{
				pnj = static_cast<PnjComponent*>(component);
				
			}
		}

		if (InputManager::GetKeyDown('E'))
		{
			//OutputDebugStringA("STOP TALKING TO ME !");
			if (pnj->m_textTextureList.size() > pnj->m_actualIdText + 1)
			{
				for (auto* uiElement : player.mp_player->GetUIList())
				{
					player.mp_gameManager->GetEntityManager()->ToDestroy(uiElement);
				}
				player.mp_player->GetUIList().clear();
				player.m_pnjHasTalked = false;

				pnj->m_actualIdText++;
			}
			if (pnj->m_hasQuest && pnj->m_textTextureList.size() <= pnj->m_actualIdText + 1)
			{
				OutputDebugStringA("Quest Accepted !");
				player.m_objectToCollect = pnj->m_nbCollectible;
				player.m_questHasStarted = true;
			}
		}
		else
		{
			if (pnj->m_textTextureList.empty())
			{
				CreateTextBox(player, "TextBoxTutoDoorTexture");
			}
			else
			{
				if (pnj->m_hasQuest && player.m_questIsCompleted && pnj->m_compleQuestTextTexture != "")
				{
					CreateTextBox(player, pnj->m_compleQuestTextTexture);
				}
				else
				{
					CreateTextBox(player, pnj->m_textTextureList[pnj->m_actualIdText]);
				}
			}
		}

		if (!player.m_pnjHasTalked)
		{
			int randNb = rand() & 1;
			if (randNb)
			{
				AssetManager::PlayLocalSound("pnjTalk1");
			}
			else
			{
				AssetManager::PlayLocalSound("pnjTalk2");
			}

			player.m_pnjHasTalked = true;
		}
	}

	if (!CollideWithTag(Pnj_TAG, player.mp_gameManager->GetEntityManager(), player.GetPlayerEntity()) && player.mp_player->GetUIList().size() > 0)
	{
		for (auto* uiElement : player.mp_player->GetUIList())
		{
			player.mp_gameManager->GetEntityManager()->ToDestroy(uiElement);
		}
		player.mp_player->GetUIList().clear();
		player.m_pnjHasTalked = false;
	}

	//collectible quest
	Entity* collectibleItem = CollideWithTag(QuestObject_TAG, player.mp_gameManager->GetEntityManager(), player.mp_playerEntity);
	if (collectibleItem != nullptr && player.m_questHasStarted)
	{
		OutputDebugStringA("You colected a quest item !");
		player.mp_gameManager->GetEntityManager()->ToDestroy(collectibleItem);
		player.m_objectCollected++;

		if (player.m_objectCollected >= player.m_objectToCollect)
		{
			OutputDebugStringA("Quest Completed !");
			player.m_questIsCompleted = true;
		}
	}

	//StartBoss
	if (CollideWithTag(Altar_TAG, player.mp_gameManager->GetEntityManager(), player.GetPlayerEntity()))
	{
		if (InputManager::GetKeyDown('E') && !player.m_startBoss)
		{
			player.m_startBoss = true;
			//CreateTextBoss(player, "BossHpTexture6");
			OutputDebugStringA("Boss Battle Has Started !");

			std::string newTextBossHp = "BossHpTexture" + std::to_string(player.m_bossHp);
			OutputDebugStringA(newTextBossHp.c_str());

			CreateTextBoss(player, newTextBossHp);
		}
	}

	if (player.m_startBoss)
	{
		if (player.m_bossHp <= 0)
		{
			player.m_bossIsVanquished = true;
			CreateTextBox(player, "winTexture");
		}
		else if(playerComponent->m_projectileHasCollided)
		{
			player.m_bossHp--;

			if (!player.mp_player->GetUIBossList().empty()) 
			{
				for (auto* uiElement : player.mp_player->GetUIBossList())
				{
					player.mp_gameManager->GetEntityManager()->ToDestroy(uiElement);
				}
				player.mp_player->GetUIBossList().clear();

				std::string newTextBossHp = "BossHpTexture" + std::to_string(player.m_bossHp);
				OutputDebugStringA(newTextBossHp.c_str());

				CreateTextBoss(player, newTextBossHp);
				playerComponent->m_projectileHasCollided = false;
			}
		}
	}

	//Jump
	if (InputManager::GetKeyDown(VK_SPACE))
	{
		// PLAY SOUND
		AssetManager::PlayLocalSound("Jump");
		player.TransitionTo(PlayerState::StateType::Jumping);
		//player.TransitionTo(PlayerState::StateType::ChargingJump);
	}

	//Button
	Entity* collidedButton = CollideWithTag(Button_TAG, player.mp_gameManager->GetEntityManager(), player.mp_playerEntity);
	if (collidedButton != nullptr)
	{
		ButtonComponent* buttonComp = nullptr;
		for (auto* comp : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[collidedButton->tab_index]->vec_components)
		{
			if (comp->ID == Button_ID)
			{
				buttonComp = static_cast<ButtonComponent*>(comp);
				break;
			}
		}

		if (buttonComp && !buttonComp->activated && playerComponent->m_bridgeConditionCount >= 2 && player.m_questIsCompleted)
		{
			if (buttonComp->linkedDoor)
			{

				TransformComponent* doorTransform = nullptr;
				DoorComponent* doorComp = nullptr;
				for (auto* comp : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[buttonComp->linkedDoor->tab_index]->vec_components)
				{
					if (comp->ID == Transform_ID)
						doorTransform = static_cast<TransformComponent*>(comp);
					if (comp->ID == Door_ID)
						doorComp = static_cast<DoorComponent*>(comp);
				}
				if (doorTransform && doorComp)
				{
					doorComp->isMoving = true;
				}
			}
			buttonComp->activated = true;
		}
	}
	// Changement du Respawn
	if (transform->m_transform.GetPositionZ() > 4500.f)
	{
		transform->m_transform.m_respawnPosition = { 250.f, 310.f, 5100.f };
	}
}

//jumping
void JumpingState::Start(PlayerState& player)
{
	OutputDebugStringA("imJumping");

}
void JumpingState::Update(float deltaTime, PlayerState& player)
{
	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}

	if (player.m_jumpProgress == 0)
	{

		player.mp_player->SetJumpPosY(transform->m_transform.vPosition.y);
	}

	float gForce = gravity->m_gravityForce * gravity->m_weight;

	float y = player.mp_player->GetJumpPosY() + (gravity->m_jumpForce * player.m_jumpProgress) - (0.5f * gForce * (player.m_jumpProgress * player.m_jumpProgress));
	transform->m_transform.vPosition.y = y;
	transform->m_transform.UpdateMatrix();
	player.m_jumpProgress += deltaTime * 1;

	//running
	if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
	//walking
	if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

	if (player.m_jumpProgress > deltaTime)
	{
		int colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());

		if (colliderTAG >= 0)
		{
			if (colliderTAG == StickySurface_TAG)
			{
				AssetManager::PlayLocalSound("JumpFromSticky");
				player.m_jumpProgress = 0;
				player.TransitionTo(PlayerState::StateType::Sticky);
			}
			else if (colliderTAG == DeathZone_TAG)
			{
				player.m_jumpProgress = 0;
				transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
				transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
				player.TransitionTo(PlayerState::StateType::Idle);
			}
			else
			{
				colliderTAG = CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
				if (colliderTAG >= 0)
				{
					if (colliderTAG == PowerJumping_TAG)
					{
						// PLAY SOUND
						player.TransitionTo(PlayerState::StateType::PowerJumping);
					}
					if (colliderTAG == BouncySurface_TAG)
					{
						// PLAY SOUND
						player.m_jumpProgress = 0;
						player.TransitionTo(PlayerState::StateType::Bouncing);
					}
					else
					{
						AssetManager::PlayLocalSound("Fallimpact");
						player.m_jumpProgress = 0;
						player.TransitionTo(PlayerState::StateType::Idle);
						//OutputDebugStringA("Player has finished jumping");
					}
				}
				else if (CollideFromTop(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
				{
					AssetManager::PlayLocalSound("LateralHit");
					player.m_jumpProgress = 0;
					player.TransitionTo(PlayerState::StateType::Idle);
					//OutputDebugStringA("Player has finished jumping");
				}
			}
		}
	}

	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (InputManager::GetKeyDown('C'))
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Placing);
	}
	if (InputManager::GetKeyDown(MK_RBUTTON))
	{
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Eating);
	}

	if (player.mp_player->CanJump())
	{
		if (InputManager::GetKeyDown(VK_SPACE))
		{
			AssetManager::PlayLocalSound("Jump2");
			player.mp_player->SetCanJump(false);
			player.m_jumpProgress = 0;
			player.TransitionTo(PlayerState::StateType::Jumping);
			//player.Jump();
		}
	}

	if (player.m_jumpForce > 0)
	{
		gravity->m_jumpForce = player.m_jumpForce;
		player.m_jumpForce = 0; // Reset apres utilisation
	}

	if (transform->m_transform.GetPositionY() < -80) // respawn
	{
		player.m_jumpProgress = 0;
		transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
		transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
		player.TransitionTo(PlayerState::StateType::Idle);
	}
}

//eating
void EatingState::Start(PlayerState& player)
{
	OutputDebugStringA("imEating");
}
void EatingState::Update(float deltaTime, PlayerState& player)
{

	Entity* eatable = nullptr;
	Entity* playerEntity = player.mp_playerEntity;

	PlayerComponent* playerComp = nullptr;
	VelocityComponent* velComponent = nullptr;
	TransformComponent* playerTransform = nullptr;
	HealthComponent* healthComp = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Player_ID)
		{
			playerComp = static_cast<PlayerComponent*>(component);
		}
		if (component->ID == Transform_ID)
		{
			playerTransform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Health_ID)
		{
			healthComp = static_cast<HealthComponent*>(component);
		}
	}
	eatable = CollideWithEatable(playerEntity, player.mp_gameManager->GetEntityManager());
	if (eatable != nullptr)
	{

		TransformComponent* transformEatable = nullptr;
		MeshComponent* meshComp = nullptr;
		ColliderComponent* collider = nullptr;

		for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[eatable->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transformEatable = static_cast<TransformComponent*>(component);
				//transformEatable->m_transform.vPosition = { 0.0f,100.0f,0.0f };
			}
			if (component->ID == Collider_ID)
			{
				collider = static_cast<ColliderComponent*>(component);
			}
			if (component->ID == Mesh_ID)
			{
				meshComp = static_cast<MeshComponent*>(component);
			}
		}


		float sizeRatio = //  ==> Volume1 / Volume 2
			(transformEatable->m_transform.vScale.z * transformEatable->m_transform.vScale.y * transformEatable->m_transform.vScale.x)
			/ (playerTransform->m_transform.vScale.z * playerTransform->m_transform.vScale.y * playerTransform->m_transform.vScale.x);

		if (sizeRatio <= 0.35)
		{
			AssetManager::PlayLocalSound("Eat");
			DirectX::XMFLOAT3 tempScale = transformEatable->m_transform.vScale;
			transformEatable->m_transform.Identity();
			transformEatable->m_transform.UpdateMatrix();
			transformEatable->m_transform.vScale = tempScale;
			transformEatable->m_transform.UpdateMatrix();
			collider->m_isDrawable = false;
			meshComp->m_drawable = false;

			float mediumScale = (transformEatable->m_transform.vScale.z + transformEatable->m_transform.vScale.y + transformEatable->m_transform.vScale.x) / 3;

			playerTransform->m_transform.vScale.z += playerTransform->m_transform.vScale.z * (mediumScale / playerTransform->m_transform.vScale.z) / 8;
			playerTransform->m_transform.vScale.y += playerTransform->m_transform.vScale.y * (mediumScale / playerTransform->m_transform.vScale.y) / 8;
			playerTransform->m_transform.vScale.x += playerTransform->m_transform.vScale.x * (mediumScale / playerTransform->m_transform.vScale.x) / 8;
			playerTransform->m_transform.UpdateMatrix();


			DirectX::XMFLOAT3 vScale;
			std::wstring m_textureID = L"";


			//int newHpValue = healthComp->currentHealth + 1;
			//healthComp->currentHealth = newHpValue;
			//std::string hpOutpout = "My hp now :" + std::to_string(newHpValue);
			//OutputDebugStringA(hpOutpout.c_str());

			if (healthComp->currentHealth < healthComp->maxHealth)
			{
				int newHpValue = healthComp->currentHealth + 1;
				healthComp->currentHealth = newHpValue;
				std::string hpOutpout = "My hp now :" + std::to_string(newHpValue);
				OutputDebugStringA(hpOutpout.c_str());

				/*TransformComponent* transformLifeComp = nullptr;
				for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_player->GetLifePointUI()->tab_index]->vec_components)
				{
					if (component->ID == Transform_ID)
					{
						transformLifeComp = static_cast<TransformComponent*>(component);
						if (healthComp->currentHealth < 11)
						{
							transformLifeComp->m_transform.Scale(newHpValue, newHpValue, 0);
						}
					}
				}*/
			}

			if (eatable->m_tag == Projectile_TAG)
			{
				InventoryItem2 newProjectile;
				newProjectile.m_itemEntity = eatable;
				newProjectile.m_index = playerComp->m_throwables.size();

				playerComp->m_throwables.push_back(newProjectile);
			}
			else if (eatable->m_tag == EatableObject_TAG || eatable->m_tag == PowerJumping_TAG)
			{
				InventoryItem2 newItem;
				newItem.m_itemEntity = eatable;
				newItem.m_index = playerComp->m_inventory.size();

				playerComp->m_inventory.push_back(newItem);
			}
			else if (eatable->m_tag == Enemy_TAG)
			{
				player.mp_gameManager->GetEntityManager()->ToDestroy(eatable);
			}
			OutputDebugStringA("imEating");
		}
		else
			OutputDebugStringA("ItemTooBigToEat");

	}
	else
	{
		// PLAY SOUND 
	}
	if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
	//walking
	if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

	player.TransitionTo(PlayerState::StateType::Idle);
}




//Placing
void PlacingState::Start(PlayerState& player)
{
	OutputDebugStringA("imPlacing");
}
void PlacingState::Update(float deltaTime, PlayerState& player)
{
	Entity* sceneObject = nullptr;
	Entity* playerEntity = player.mp_playerEntity;

	PlayerComponent* playerComp = nullptr;
	VelocityComponent* velComponent = nullptr;

	{
		//PlayerComponent* playerComp = nullptr;
		TransformComponent* playerTransform = nullptr;

		ColliderComponent* collider = nullptr;

		for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[playerEntity->tab_index]->vec_components)
		{
			if (component->ID == Player_ID)
			{
				playerComp = static_cast<PlayerComponent*>(component);
			}
			if (component->ID == Transform_ID)
			{
				playerTransform = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Velocity_ID)
			{
				velComponent = static_cast<VelocityComponent*>(component);
			}
		}

		if (playerComp->m_inventory.empty() == false)
		{
			InventoryItem2& lastItem = playerComp->m_inventory.back();

			TransformComponent* transform = nullptr;
			MeshComponent* meshComp = nullptr;

			for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[lastItem.m_itemEntity->tab_index]->vec_components)
			{
				if (component->ID == Transform_ID)
				{
					transform = static_cast<TransformComponent*>(component);
					transform->m_transform.Move(
						playerTransform->m_transform.vPosition.z + (playerTransform->m_transform.vFront.z * (playerTransform->m_transform.vScale.z + transform->m_transform.vScale.z)),
						playerTransform->m_transform.vPosition.x + (playerTransform->m_transform.vFront.x * (playerTransform->m_transform.vScale.x + transform->m_transform.vScale.x)),
						playerTransform->m_transform.vPosition.y /*+ (transform->m_transform.vFront.y * (transform->m_transform.vScale.y + lastItem.vScale.y))*/
					);

					float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
					float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
					float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
					transform->m_transform.Rotation(roll, pitch, yaw);
				}
				if (component->ID == Collider_ID)
				{
					collider = static_cast<ColliderComponent*>(component);
					collider->m_isDrawable = true;
				}
				if (component->ID == Mesh_ID)
				{
					meshComp = static_cast<MeshComponent*>(component);
					meshComp->m_drawable = true;
				}
				if (meshComp && transform && collider)
					break;
			}


			////ObjectStateMachine
			//SceneObject* newItem = new SceneObject;
			//newItem->SetObjectEntity(lastItem.m_itemEntity);
			////newProjectile->SetPlayerEntity(player.GetPlayerEntity());
			//newItem->SetStartPosY(transform->m_transform.vPosition.y);
			////newProjectile->GetObjectState().Shoot();
			//newItem->Init(player.mp_gameManager);
			//newItem->SetListIndex(player.mp_player->mp_scene->GetSceneObjectList().size());

			//player.mp_player->mp_scene->GetSceneObjectList().push_back(newItem);


			playerComp->m_inventory.pop_back();

			if (playerTransform->m_transform.vScale.z > 1)
			{
				float mediumScale = (transform->m_transform.vScale.z + transform->m_transform.vScale.y + transform->m_transform.vScale.x) / 3;

				playerTransform->m_transform.vScale.z -= playerTransform->m_transform.vScale.z * (mediumScale / playerTransform->m_transform.vScale.z) / 8;
				playerTransform->m_transform.vScale.y -= playerTransform->m_transform.vScale.y * (mediumScale / playerTransform->m_transform.vScale.y) / 8;
				playerTransform->m_transform.vScale.x -= playerTransform->m_transform.vScale.x * (mediumScale / playerTransform->m_transform.vScale.x) / 8;
				playerTransform->m_transform.UpdateMatrix();
			}
		}
		else
		{
			// PLAY SOUND
		}

		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

		OutputDebugStringA("imPlacingItem");
	}
	player.TransitionTo(PlayerState::StateType::Idle);
}

//falling
void FallingState::Start(PlayerState& player)
{
	OutputDebugStringA("imFalling");
}
void FallingState::Update(float deltaTime, PlayerState& player)
{
	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}

	if (player.m_fallProgress == 0)
	{
		player.mp_player->SetJumpPosY(transform->m_transform.vPosition.y);
	}

	float gForce = gravity->m_gravityForce * gravity->m_weight;
	float y = player.mp_player->GetJumpPosY()/* + (-gravity->m_jumpForce * player.m_fallProgress)*/ - (0.5f * gForce * (player.m_fallProgress * player.m_fallProgress));
	transform->m_transform.vPosition.y = y;
	transform->m_transform.UpdateMatrix();
	player.m_fallProgress += deltaTime * 1;

	/*if (player.m_fallProgress > deltaTime)
	{*/
	if (InputManager::GetKeyIsPressed('D') || InputManager::GetKeyIsPressed('Q') || InputManager::GetKeyIsPressed('Z') || InputManager::GetKeyIsPressed('S'))
	{
		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();
	}

	if (player.m_fallProgress > deltaTime)
	{
		int colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
		if (colliderTAG >= 0)
		{
			if (colliderTAG == StickySurface_TAG)
			{
				AssetManager::PlayLocalSound("JumpFromSticky");
				player.m_fallProgress = 0;
				player.TransitionTo(PlayerState::StateType::Sticky);
			}
			else if (colliderTAG == DeathZone_TAG)
			{
				player.m_fallProgress = 0;
				transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
				transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
				player.TransitionTo(PlayerState::StateType::Idle);
			}
			else
			{
				colliderTAG = CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
				if (colliderTAG >= 0)
				{
					if (colliderTAG == BouncySurface_TAG)
					{
						// PLAY SOUND
						player.m_fallProgress = 0;
						player.TransitionTo(PlayerState::StateType::Bouncing);
					}
					else if (colliderTAG == PowerJumping_TAG)
					{
						// PLAY SOUND
						player.m_fallProgress = 0;
						player.TransitionTo(PlayerState::StateType::PowerJumping);

					}
					else
					{
						if (player.m_fallProgress > 0.2)
							AssetManager::PlayLocalSound("Fallimpact");
						player.m_fallProgress = 0;
						player.TransitionTo(PlayerState::StateType::Idle);
						//OutputDebugStringA("Player has finished jumping");
					}
				}
				else if (CollideFromTop(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
				{
					AssetManager::PlayLocalSound("LateralHit");
					player.m_fallProgress = 0;
					player.TransitionTo(PlayerState::StateType::Idle);
					//OutputDebugStringA("Player has finished jumping");
				}
			}
		}
	}
	/*if (CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
	{
		player.m_fallProgress = 0;
		player.TransitionTo(PlayerState::StateType::Idle);
	}*/
	//if (CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == StickySurface_TAG)
	//{
	//	player.m_fallProgress = 0;
	//	// PLAY SOUND
	//	player.TransitionTo(PlayerState::StateType::Sticky);
	//}

	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (player.mp_player->CanJump())
	{
		if (InputManager::GetKeyDown(VK_SPACE))
		{
			AssetManager::PlayLocalSound("Jump2");
			player.mp_player->SetCanJump(false);
			player.m_fallProgress = 0;
			player.TransitionTo(PlayerState::StateType::Jumping);
			//player.Jump();
		}
	}

	if (transform->m_transform.GetPositionY() < -80) // respawn
	{
		player.m_fallProgress = 0;
		transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
		transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
		player.TransitionTo(PlayerState::StateType::Idle);
	}
}

//chargejump
void ChargingJumpState::Start(PlayerState& player)
{
	std::ostringstream oss;
	oss << "charge jump start\n";
	OutputDebugStringA(oss.str().c_str());
}
void ChargingJumpState::Update(float deltaTime, PlayerState& player)
{
	//OutputDebugStringA("Player has finished charge jumping");

	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
	}
	if (InputManager::GetKeyIsPressed(VK_SPACE))
	{
		//std::ostringstream oss;
		//oss << "charge jump start" << transform->m_transform.vScale.x << "\n";
		//OutputDebugStringA(oss.str().c_str());

		player.m_chargeProgress += deltaTime;

		float scaleFactor = 1.0f - (0.5f * player.m_chargeProgress); // Facteur de reduction
		float newScale = player.mp_player->GetScale() * scaleFactor; // Applique le facteur a la scale initiale

		if (newScale >= 1)
		{
			transform->m_transform.Scale(newScale, newScale, newScale);
		}

		gravity->m_jumpForce += 1;
	}
	if (InputManager::GetKeyReleased(VK_SPACE))
	{
		float chargeFactor = 1.0f + player.m_chargeProgress;
		player.m_jumpForce = player.mp_player->GetJumpForce() * chargeFactor * 0.5;

		player.m_chargeProgress = 0;
		transform->m_transform.Scale(player.mp_player->GetScale(), player.mp_player->GetScale(), player.mp_player->GetScale());
		player.TransitionTo(PlayerState::StateType::Jumping);
	}
}

//moving
void MovingState::Start(PlayerState& player)
{
	OutputDebugStringA("imMoving");
}
void MovingState::Update(float deltaTime, PlayerState& player)
{
	//OutputDebugStringA("imMoving");
	VelocityComponent* velComponent = nullptr;
	TransformComponent* transform = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
	}

	//float currentPosY = transform->m_transform.vPosition.y;

	if (InputManager::GetKeyIsPressed('D') || InputManager::GetKeyIsPressed('Q') || InputManager::GetKeyIsPressed('Z') || InputManager::GetKeyIsPressed('S'))
	{
		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

	}
	else
	{
		player.TransitionTo(PlayerState::StateType::Idle);
	}



	//player.mp_player->SetLastPosY(currentPosY);

	if (CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == -1) // Chute Libre
	{
		// PLAY SOUND
		player.TransitionTo(PlayerState::StateType::Idle);
	}
	if (CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == Pnj_TAG) // Parle au pnj
	{
		player.TransitionTo(PlayerState::StateType::Talking);
	}

	if (InputManager::GetKeyDown(MK_RBUTTON))
	{
		player.TransitionTo(PlayerState::StateType::Eating);
	}

	if (InputManager::GetKeyDown('C'))
	{
		// PLAY SOUND
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (InputManager::GetKeyDown(VK_SPACE))
	{
		AssetManager::PlayLocalSound("Jump");
		//player.TransitionTo(PlayerState::StateType::ChargingJump);
		player.TransitionTo(PlayerState::StateType::Jumping);
		//player.Jump();
	}

}

//Sticky
void StickyState::Start(PlayerState& player)
{
	OutputDebugStringA("imSticky");

}
void StickyState::Update(float deltaTime, PlayerState& player)
{
	GravityComponent* gravityComp = nullptr;
	VelocityComponent* velComponent = nullptr;
	TransformComponent* transform = nullptr;

	static float cooldown = 0.0f;

	if (cooldown <= 0.0f) {
		AssetManager::PlayLocalSound("Sticky");
		cooldown = 0.8f;
	}
	else {
		cooldown -= deltaTime;
	}


	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Gravity_ID)
		{
			gravityComp = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
	}

	if (InputManager::GetKeyIsPressed('D')
		|| InputManager::GetKeyIsPressed('Q')
		|| InputManager::GetKeyIsPressed('Z')
		|| InputManager::GetKeyIsPressed('S')
		|| InputManager::GetKeyIsPressed('A')
		|| InputManager::GetKeyIsPressed('W'))
	{


		//Running
		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('A') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vy = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('W') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vy = -player.mp_player->GetSpeed() * 2;
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('A') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vy = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('W') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vy = -player.mp_player->GetSpeed();
	}


	if (CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == -1)
	{
		player.TransitionTo(PlayerState::StateType::Idle);
	}
	else
		player.mp_player->SetCanJump(true);

	if (CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
	{
		player.TransitionTo(PlayerState::StateType::Idle);
	}

	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		// PLAY SOUND
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (InputManager::GetKeyDown(VK_SPACE))
	{
		AssetManager::PlayLocalSound("JumpFromSticky"); // METTRE SOUND STICKY
		player.TransitionTo(PlayerState::StateType::Jumping);
		//player.Jump();
	}
}

//Attacking
void AttackingState::Start(PlayerState& player)
{
	OutputDebugStringA("imAttacking");
}
void AttackingState::Update(float deltaTime, PlayerState& player)
{
	//Entity* throwable = nullptr;
	Entity* playerEntity = player.mp_playerEntity;

	PlayerComponent* playerComp = nullptr;
	VelocityComponent* velComponent = nullptr;
	HealthComponent* healthComp = nullptr;


	//PlayerComponent* playerComp = nullptr;
	TransformComponent* playerTransform = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Player_ID)
		{
			playerComp = static_cast<PlayerComponent*>(component);
		}
		if (component->ID == Transform_ID)
		{
			playerTransform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Health_ID)
		{
			healthComp = static_cast<HealthComponent*>(component);
		}
	}

	if (playerComp->m_throwables.empty() == false)
	{
		//AssetManager::PlayLocalSound("pop");
		AssetManager::PlayLocalSound("Shoot");


		if (healthComp->currentHealth > 1)
		{
			int newHpValue = healthComp->currentHealth - 1;
			healthComp->currentHealth = newHpValue;
			std::string hpOutpout = "My hp now :" + std::to_string(newHpValue);
			OutputDebugStringA(hpOutpout.c_str());

			InventoryItem2& lastItem = playerComp->m_throwables.back();

			TransformComponent* transform = nullptr;
			ColliderComponent* collider = nullptr;
			MeshComponent* meshComp = nullptr;

			for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[lastItem.m_itemEntity->tab_index]->vec_components)
			{
				if (component->ID == Transform_ID)
				{
					transform = static_cast<TransformComponent*>(component);
					transform->m_transform.Move(
						playerTransform->m_transform.vPosition.z + (playerTransform->m_transform.vFront.z * (playerTransform->m_transform.vScale.z + transform->m_transform.vScale.z)),
						playerTransform->m_transform.vPosition.x + (playerTransform->m_transform.vFront.x * (playerTransform->m_transform.vScale.x + transform->m_transform.vScale.x)),
						playerTransform->m_transform.vPosition.y /*+ (transform->m_transform.vFront.y * (transform->m_transform.vScale.y + lastItem.vScale.y))*/
					);
					float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
					float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
					float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
					transform->m_transform.Rotation(roll, pitch, yaw);
				}
				if (component->ID == Collider_ID)
				{
					collider = static_cast<ColliderComponent*>(component);
					collider->m_isDrawable = true;
				}
				if (component->ID == Mesh_ID)
				{
					meshComp = static_cast<MeshComponent*>(component);
					meshComp->m_drawable = true;
				}
				if (meshComp && transform && collider)
					break;
			}



			Projectile* proj = nullptr;
			for (auto* projectile : player.mp_player->GetProjectileList())
			{
				if (projectile->mp_objectEntity->id == lastItem.m_itemEntity->id)
				{
					projectile->GetObjectState().TransitionTo(ObjectState::StateType::Shooting);
					break;
				}
			}

			playerComp->m_throwables.pop_back();

			if (playerTransform->m_transform.vScale.z > 1)
			{
				float mediumScale = (transform->m_transform.vScale.z + transform->m_transform.vScale.y + transform->m_transform.vScale.x) / 3;

				playerTransform->m_transform.vScale.z -= playerTransform->m_transform.vScale.z * (mediumScale / playerTransform->m_transform.vScale.z) / 8;
				playerTransform->m_transform.vScale.y -= playerTransform->m_transform.vScale.y * (mediumScale / playerTransform->m_transform.vScale.y) / 8;
				playerTransform->m_transform.vScale.x -= playerTransform->m_transform.vScale.x * (mediumScale / playerTransform->m_transform.vScale.x) / 8;
				playerTransform->m_transform.UpdateMatrix();
			}
		}
		else
		{
			// PLAY SOUND
		}

		if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
		if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
		//walking
		if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
		if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

		OutputDebugStringA("imThrowingItem");
	}
	player.TransitionTo(PlayerState::StateType::Idle);

}
//Talking
void TalkingState::Start(PlayerState& player)
{
	OutputDebugStringA("Hello Adventurer !");
}
void TalkingState::Update(float deltaTime, PlayerState& player)
{
	if (player.mp_player->GetUIList().size() < 1)
	{
		if (player.m_questIsCompleted)
		{
			player.m_questTextNb = 3;
		}
		CreateTextBox(player,"");
	}
	if (InputManager::GetKeyIsPressed('E'))
	{
		if (player.m_questTextNb < player.m_questTextNbMax)
		{
			player.m_questTextNb++;
			if (player.m_questTextNb >= player.m_questTextNbMax)
			{
				player.m_questHasStarted = true;
			}
		}
		if (player.m_questIsCompleted)
		{

		}

		CreateTextBox(player,"");
	}

	TransformComponent* transform = nullptr;
	VelocityComponent* velComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}


	int colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
	if (colliderTAG != Pnj_TAG)
	{
		if (player.mp_player->GetUIList().size() > 0)
		{
			for (auto* uiElement : player.mp_player->GetUIList())
			{
				player.mp_gameManager->GetEntityManager()->ToDestroy(uiElement);
			}
			player.mp_player->GetUIList().clear();
		}
		player.TransitionTo(PlayerState::StateType::Idle);
	}


	if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
	//walking
	if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();
}


//Bouncing
void BouncingState::Start(PlayerState& player)
{
	OutputDebugStringA("imBouncing");

}
void BouncingState::Update(float deltaTime, PlayerState& player)
{
	OutputDebugStringA("imBouncing");
	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}

	if (player.m_jumpProgress == 0)
	{
		//player.mp_player->SetSpeed(player.mp_player->GetSpeed() / 1.5);
		player.mp_player->SetJumpPosY(transform->m_transform.vPosition.y);
	}

	float gForce = gravity->m_gravityForce * gravity->m_weight;

	float bJumpForce = gravity->m_jumpForce / 1.5f;

	float y = player.mp_player->GetJumpPosY() + (bJumpForce * player.m_jumpProgress) - (0.5f * gForce * (player.m_jumpProgress * player.m_jumpProgress));
	transform->m_transform.vPosition.y = y;
	transform->m_transform.UpdateMatrix();
	player.m_jumpProgress += deltaTime * 1;

	if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
	//walking
	if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

	if (player.m_jumpProgress > deltaTime)
	{
		int colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
		if (colliderTAG >= 0)
		{
			if (colliderTAG == StickySurface_TAG)
			{
				AssetManager::PlayLocalSound("JumpFromSticky");
				player.m_jumpProgress = 0;
				//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
				player.TransitionTo(PlayerState::StateType::Sticky);
			}
			else
			{
				colliderTAG = CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
				if (colliderTAG >= 0)
				{
					if (colliderTAG == BouncySurface_TAG)
					{
						// PLAY SOUND
						player.m_jumpProgress = 0;
						//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
						player.TransitionTo(PlayerState::StateType::Bouncing);
					}
					else if (colliderTAG == PowerJumping_TAG)
					{
						// PLAY SOUND
						player.m_jumpProgress = 0;
						//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
						player.TransitionTo(PlayerState::StateType::PowerJumping);

					}
					else
					{
						player.m_jumpProgress = 0;
						//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
						player.TransitionTo(PlayerState::StateType::Idle);
						OutputDebugStringA("Player has finished jumping");
					}
				}
				else if (CollideFromTop(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
				{
					player.m_jumpProgress = 0;
					//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
					player.TransitionTo(PlayerState::StateType::Idle);
					OutputDebugStringA("Player has finished jumping");
				}
			}
		}
	}

	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (player.mp_player->CanJump())
	{
		if (InputManager::GetKeyDown(VK_SPACE))
		{
			AssetManager::PlayLocalSound("Jump2");
			player.m_jumpProgress = 0;
			//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
			player.TransitionTo(PlayerState::StateType::Jumping);
			//player.Jump();
		}
	}

	//if (CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == StickySurface_TAG)
	//{
	//	// PLAY SOUND
	//	player.m_jumpProgress = 0;
	//	player.TransitionTo(PlayerState::StateType::Sticky);
	//}
	if (player.m_jumpForce > 0)
	{
		gravity->m_jumpForce = player.m_jumpForce;
		player.m_jumpForce = 0; // Reset apres utilisation
		//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
	}

	if (transform->m_transform.GetPositionY() < -500) // respawn
	{
		player.m_jumpProgress = 0;
		//player.mp_player->SetSpeed(player.mp_player->GetSpeed() * 1.5);
		transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
		transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
		player.TransitionTo(PlayerState::StateType::Idle);
	}
}

//PowerJumping
void PowerJumpingState::Start(PlayerState& player)
{
	OutputDebugStringA("imPowerJumping");

}
void PowerJumpingState::Update(float deltaTime, PlayerState& player)
{
	OutputDebugStringA("imPowerJumping");
	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;

	for (auto* component : player.mp_gameManager->GetEntityManager()->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}

	if (player.m_jumpProgress == 0)
	{

		player.mp_player->SetJumpPosY(transform->m_transform.vPosition.y);
	}

	float gForce = gravity->m_gravityForce * gravity->m_weight;
	float powJumpForce = gravity->m_jumpForce * 2;

	float y = player.mp_player->GetJumpPosY() + (powJumpForce * player.m_jumpProgress) - (0.5f * gForce * (player.m_jumpProgress * player.m_jumpProgress));
	transform->m_transform.vPosition.y = y;
	transform->m_transform.UpdateMatrix();
	player.m_jumpProgress += deltaTime * 1;

	if (InputManager::GetKeyIsPressed('D') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Q') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('Z') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed() * 2;
	if (InputManager::GetKeyIsPressed('S') && InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed() * 2;
	//walking
	if (InputManager::GetKeyIsPressed('D') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Q') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vx = -player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('Z') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = player.mp_player->GetSpeed();
	if (InputManager::GetKeyIsPressed('S') && !InputManager::GetKeyIsPressed(VK_SHIFT)) velComponent->vz = -player.mp_player->GetSpeed();

	if (player.m_jumpProgress > deltaTime)
	{
		int colliderTAG = CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
		if (colliderTAG >= 0)
		{
			if (colliderTAG == StickySurface_TAG)
			{
				AssetManager::PlayLocalSound("JumpFromSticky");
				player.m_jumpProgress = 0;
				player.TransitionTo(PlayerState::StateType::Sticky);
			}
			else
			{
				colliderTAG = CollideFromBelow(player.mp_playerEntity, player.mp_gameManager->GetEntityManager());
				if (colliderTAG >= 0)
				{
					if (colliderTAG == BouncySurface_TAG)
					{
						// PLAY SOUND
						player.m_jumpProgress = 0;
						player.TransitionTo(PlayerState::StateType::Bouncing);
					}
					else if (colliderTAG == PowerJumping_TAG)
					{
						// PLAY SOUND
						player.m_jumpProgress = 0;
						player.TransitionTo(PlayerState::StateType::PowerJumping);

					}
					else
					{
						player.m_jumpProgress = 0;
						player.TransitionTo(PlayerState::StateType::Idle);
						OutputDebugStringA("Player has finished jumping");
					}
				}
				else if (CollideFromTop(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) >= 0)
				{
					player.m_jumpProgress = 0;
					player.TransitionTo(PlayerState::StateType::Idle);
					OutputDebugStringA("Player has finished jumping");
				}
			}
		}
	}

	//if (InputManager::GetKeyDown('C'))
	//{
	//	// PLAY SOUND
	//	player.m_jumpProgress = 0;
	//	player.TransitionTo(PlayerState::StateType::Attacking);
	//}
	//if (InputManager::GetKeyDown(MK_RBUTTON))
	//{
	//	player.m_jumpProgress = 0;
	//	player.TransitionTo(PlayerState::StateType::Eating);
	//}

	if (InputManager::GetKeyDown(MK_LBUTTON))
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Attacking);
	}
	if (player.mp_player->CanJump())
	{
		if (InputManager::GetKeyDown(VK_SPACE))
		{
			AssetManager::PlayLocalSound("Jump2");
			player.m_jumpProgress = 0;
			player.TransitionTo(PlayerState::StateType::Jumping);
			//player.Jump();
		}
	}

	if (CollideWith(player.mp_playerEntity, player.mp_gameManager->GetEntityManager()) == StickySurface_TAG)
	{
		// PLAY SOUND
		player.m_jumpProgress = 0;
		player.TransitionTo(PlayerState::StateType::Sticky);
	}
	if (player.m_jumpForce > 0)
	{
		gravity->m_jumpForce = player.m_jumpForce;
		player.m_jumpForce = 0; // Reset apres utilisation
	}

	if (transform->m_transform.GetPositionY() < -500) // respawn
	{
		player.m_jumpProgress = 0;
		transform->m_transform.m_respawnPosition.y += transform->m_transform.vScale.y / 2;
		transform->m_transform.vPosition = transform->m_transform.m_respawnPosition;
		player.TransitionTo(PlayerState::StateType::Idle);
	}
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------------- OBJECT STATE ------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Object
ObjectState::ObjectState()
{
	//std::cout << "New Gun created with " << mAmmo << " Ammo !" << std::endl;

	m_States[(int)StateType::Placing] = new PlaceState();
	m_States[(int)StateType::Shooting] = new ShootState();
	m_States[(int)StateType::Fall] = new FallState();

	m_State = m_States[(int)StateType::Placing];
}

void ObjectState::Update(float deltaTime)
{
	m_State->Update(deltaTime, *this);
}

void ObjectState::Place()
{
	if (TransitionTo(StateType::Placing) == false)
		return;
	m_State->Start(*this);
}

void ObjectState::Shoot()
{
	if (TransitionTo(StateType::Shooting) == false)
		return;
	m_State->Start(*this);
}

bool ObjectState::TransitionTo(StateType newState)
{
	std::ostringstream oss;

	if (m_Transition[(int)m_State->GetStateType()][(int)newState] == 0)
	{
		/*oss << "[!]Error player is: " << DisplayPlayerState() << std::endl;
		std::string message = oss.str();
		OutputDebugStringA(message.c_str());*/

		return false;
	}
	else
	{
		m_State = m_States[newState];

		//oss << "Player is now: " << DisplayPlayerState() << std::endl;
		//std::string message = oss.str();
		//OutputDebugStringA(message.c_str());

		return true;
	}
}
//Place(idle)
void PlaceState::Start(ObjectState& object)
{
}
void PlaceState::Update(float deltaTime, ObjectState& object)
{
	EntityManager* entityManager = object.mp_gameManager->GetEntityManager();

	//OutputDebugStringA("NOOOO");

	if (CollideFromBelow(object.mp_objectEntity, entityManager) == -1) // Chute Libre
	{
		// PLAY SOUND
		object.TransitionTo(ObjectState::StateType::Fall);
	}

	Entity* collidedButton = CollideWithTag(Button_TAG, entityManager, object.mp_objectEntity);
	if (collidedButton != nullptr)
	{
		ButtonComponent* buttonComp = nullptr;
		for (auto* comp : entityManager->GetComponentsTab()[collidedButton->tab_index]->vec_components)
		{
			if (comp->ID == Button_ID)
			{
				buttonComp = static_cast<ButtonComponent*>(comp);
				break;
			}
		}

		//Player Component
		TransformComponent* transformPlayer = nullptr;
		PlayerComponent* playerComp = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[object.mp_playerEntity->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transformPlayer = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Player_ID)
			{
				playerComp = static_cast<PlayerComponent*>(component);
			}
		}

		if (buttonComp && !buttonComp->activated)
		{
			playerComp->m_bridgeConditionCount += 1;
			buttonComp->activated = true;
		}
	}
}
//Shoot
void ShootState::Start(ObjectState& object)
{
	OutputDebugStringA("imAprojectile");
	////Projectile Component
	//TransformComponent* transform = nullptr;

	//for (auto* component : object.mp_gameManager->GetEntityManager()->GetComponentsTab()[object.mp_objectEntity->tab_index]->vec_components)
	//{
	//	if (component->ID == Transform_ID)
	//	{
	//		transform = static_cast<TransformComponent*>(component);
	//	}
	//}

	////Player Component
	//TransformComponent* transformPlayer = nullptr;

	//for (auto* component : object.mp_gameManager->GetEntityManager()->GetComponentsTab()[object.mp_playerEntity->tab_index]->vec_components)
	//{
	//	if (component->ID == Transform_ID)
	//	{
	//		transformPlayer = static_cast<TransformComponent*>(component);
	//	}
	//}
}
void ShootState::Update(float deltaTime, ObjectState& object)
{
	//OutputDebugStringA("WEEEEEEEEEE");

	//Projectile Component
	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;
	ColliderComponent* colliderComponent = nullptr;
	EntityManager* entityManager = object.mp_gameManager->GetEntityManager();

	for (auto* component : entityManager->GetComponentsTab()[object.mp_objectEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
		if (component->ID == Collider_ID)
		{
			colliderComponent = static_cast<ColliderComponent*>(component);
			if (colliderComponent->m_isDrawable == false)
			{
				object.TransitionTo(ObjectState::StateType::Placing);
				return;
			}
		}
	}

	//Player Component
	TransformComponent* transformPlayer = nullptr;
	PlayerComponent* playerComp = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[object.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transformPlayer = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Player_ID)
		{
			playerComp = static_cast<PlayerComponent*>(component);
		}
	}

	if (object.m_fallProgress == 0)
	{
		object.mp_objectEntity->m_tag = ProjectileAttack_TAG;
		object.mp_projectile->SetStartPosY(transformPlayer->m_transform.vPosition.y);
	}
	float gForce = gravity->m_gravityForce * gravity->m_weight;
	float y = object.mp_projectile->GetStartPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
	//float x = object.mp_player->GetJumpPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
	transform->m_transform.vPosition.y = y;
	transform->m_transform.UpdateMatrix();
	velComponent->vz = (transformPlayer->m_transform.vScale.z / transform->m_transform.vScale.z) * gravity->m_jumpForce;
	object.m_fallProgress += 1 * deltaTime;
	if (object.m_fallProgress > deltaTime)
	{
		int colliderTAG = CollideWith(object.mp_objectEntity, entityManager);
		if (colliderTAG >= 0)
		{
			if (colliderTAG != Enemy_TAG)
				object.mp_objectEntity->m_tag = Projectile_TAG;
			object.m_fallProgress = 0;
			velComponent->vz = 0;
			object.TransitionTo(ObjectState::Fall);
		}
		int colliderTAG2 = CollideFromBelow(object.mp_projectile->mp_objectEntity, entityManager);
		if (colliderTAG2 >= 0)
		{
			//if (colliderTAG == BouncySurface_TAG)
			//{
			//	// PLAY SOUND
			//	object.m_fallProgress = 0;
			//	object.TransitionTo(ObjectState::StateType::Bouncing);
			//}
			//else if (colliderTAG == PowerJumping_TAG)
			//{
			//	// PLAY SOUND
			//	object.m_fallProgress = 0;
			//	object.TransitionTo(ObjectState::StateType::PowerJumping);

			{
				if (colliderTAG2 != Enemy_TAG)
					object.mp_objectEntity->m_tag = Projectile_TAG;
				velComponent->vz = 0;
				object.m_fallProgress = 0;
				object.TransitionTo(ObjectState::StateType::Placing);
				OutputDebugStringA("Player has finished jumping");
			}
		}
	}
	if (transform->m_transform.vPosition.y < -100) // respawn
	{
		object.mp_objectEntity->m_tag = Projectile_TAG;
		object.m_fallProgress = 0;
		velComponent->vz = 0;

		object.m_toDestroy = true;
		object.mp_gameManager->GetEntityManager()->ToDestroy(object.mp_objectEntity);

	}

	Entity* collidedButton = CollideWithTag(Button_TAG, entityManager, object.mp_objectEntity);
	if (collidedButton != nullptr)
	{
		ButtonComponent* buttonComp = nullptr;
		for (auto* comp : entityManager->GetComponentsTab()[collidedButton->tab_index]->vec_components)
		{
			if (comp->ID == Button_ID)
			{
				buttonComp = static_cast<ButtonComponent*>(comp);
				break;
			}
		}
		if (buttonComp && !buttonComp->activated)
		{
			if (buttonComp->linkedDoor)
			{
				TransformComponent* doorTransform = nullptr;
				DoorComponent* doorComp = nullptr;
				for (auto* comp : entityManager->GetComponentsTab()[buttonComp->linkedDoor->tab_index]->vec_components)
				{
					if (comp->ID == Transform_ID)
						doorTransform = static_cast<TransformComponent*>(comp);
					if (comp->ID == Door_ID)
						doorComp = static_cast<DoorComponent*>(comp);
				}
				if (doorTransform && doorComp)
				{
					doorComp->isMoving = true;
				}
			}
			buttonComp->activated = true;
		}
	}

	Entity* collidedBoss = CollideWithTag(Boss_TAG, entityManager, object.mp_objectEntity);
	if (collidedBoss != nullptr)
	{
		OutputDebugStringA("BOSS HP -1");
		playerComp->m_projectileHasCollided = true;
	}
}
void FallState::Start(ObjectState& object)
{
	OutputDebugStringA("imAfallingProjectile");
}
void FallState::Update(float deltaTime, ObjectState& object)
{
	//Player Component
	TransformComponent* transformPlayer = nullptr;
	PlayerComponent* playerComp = nullptr;

	for (auto* component : object.mp_gameManager->GetEntityManager()->GetComponentsTab()[object.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transformPlayer = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Player_ID)
		{
			playerComp = static_cast<PlayerComponent*>(component);
		}
	}

	TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;
	ColliderComponent* colliderComponent = nullptr;

	if (object.mp_projectile)
	{
		for (auto* component : object.mp_gameManager->GetEntityManager()->GetComponentsTab()[object.mp_projectile->mp_objectEntity->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Gravity_ID)
			{
				gravity = static_cast<GravityComponent*>(component);
			}
			if (component->ID == Velocity_ID)
			{
				velComponent = static_cast<VelocityComponent*>(component);
			}
			if (component->ID == Collider_ID)
			{
				colliderComponent = static_cast<ColliderComponent*>(component);
				if (colliderComponent->m_isDrawable == false)
				{
					object.TransitionTo(ObjectState::StateType::Placing);
					return;
				}
			}
		}
		if (object.m_fallProgress == 0)
		{
			object.mp_projectile->SetStartPosY(transform->m_transform.vPosition.y);
		}

		if (gravity)
		{
			float gForce = gravity->m_gravityForce * gravity->m_weight;
			float y = object.mp_projectile->GetStartPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
			transform->m_transform.vPosition.y = y;
			transform->m_transform.UpdateMatrix();
			object.m_fallProgress += deltaTime * 1;
		}
		if (object.m_fallProgress > deltaTime)
		{
			int colliderTAG2 = CollideFromBelow(object.mp_projectile->mp_objectEntity, object.mp_gameManager->GetEntityManager());
			if (colliderTAG2 >= 0)
			{
				velComponent->vz = 0;
				object.m_fallProgress = 0;
				object.TransitionTo(ObjectState::StateType::Placing);
				OutputDebugStringA("Player has finished jumping");

			}
		}
		if (transform->m_transform.vPosition.y < -50)
		{
			object.m_fallProgress = 0;
			velComponent->vz = 0;

			object.m_toDestroy = true;
			object.mp_gameManager->GetEntityManager()->ToDestroy(object.mp_objectEntity);
		}
	}
	else
	{
		for (auto* component : object.mp_gameManager->GetEntityManager()->GetComponentsTab()[object.mp_object->mp_objectEntity->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Gravity_ID)
			{
				gravity = static_cast<GravityComponent*>(component);
			}
			if (component->ID == Velocity_ID)
			{
				velComponent = static_cast<VelocityComponent*>(component);
			}
			if (component->ID == Collider_ID)
			{
				colliderComponent = static_cast<ColliderComponent*>(component);
				if (colliderComponent->m_isDrawable == false)
				{
					object.TransitionTo(ObjectState::StateType::Placing);
					return;
				}
			}
		}
		if (object.m_fallProgress == 0)
		{
			object.mp_object->SetStartPosY(transform->m_transform.vPosition.y);
		}
		if (gravity)
		{
			float gForce = gravity->m_gravityForce * gravity->m_weight;
			float y = object.mp_object->GetStartPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
			transform->m_transform.vPosition.y = y;
			transform->m_transform.UpdateMatrix();
			object.m_fallProgress += deltaTime * 1;
		}

		if (object.m_fallProgress > deltaTime)
		{
			int colliderTAG2 = CollideFromBelow(object.mp_object->mp_objectEntity, object.mp_gameManager->GetEntityManager());
			if (colliderTAG2 >= 0)
			{

				object.m_fallProgress = 0;
				object.TransitionTo(ObjectState::StateType::Placing);
				OutputDebugStringA("Player has finished jumping");

			}
		}
		if (transform->m_transform.vPosition.y < -50)
		{
			object.m_fallProgress = 0;
			if (velComponent)
				velComponent->vz = 0;

			object.m_toDestroy = true;
			object.mp_gameManager->GetEntityManager()->ToDestroy(object.mp_objectEntity);

		}

	}


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------------------- ENEMY STATE ------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Enemy
EnemyState::EnemyState()
{
	//std::cout << "New Gun created with " << mAmmo << " Ammo !" << std::endl;

	m_States[(int)StateType::Sleeping] = new SleepingState();
	m_States[(int)StateType::Chasing] = new ChasingState();
	m_States[(int)StateType::Colliding] = new CollidingState();

	m_State = m_States[(int)StateType::Sleeping];
}

void EnemyState::Update(float deltaTime)
{
	m_State->Update(deltaTime, *this);
}

//void EnemyState::Place()
//{
//	if (TransitionTo(StateType::Placing) == false)
//		return;
//	m_State->Start(*this);
//}
//
//void EnemyState::Shoot()
//{
//	if (TransitionTo(StateType::Shooting) == false)
//		return;
//	m_State->Start(*this);
//}

bool EnemyState::TransitionTo(StateType newState)
{
	std::ostringstream oss;

	if (m_Transition[(int)m_State->GetStateType()][(int)newState] == 0)
	{
		/*oss << "[!]Error player is: " << DisplayPlayerState() << std::endl;
		std::string message = oss.str();
		OutputDebugStringA(message.c_str());*/

		return false;
	}
	else
	{
		m_State = m_States[newState];

		//oss << "Player is now: " << DisplayPlayerState() << std::endl;
		//std::string message = oss.str();
		//OutputDebugStringA(message.c_str());

		return true;
	}
}

//Place(idle)
void SleepingState::Start(EnemyState& enemy)
{
}
void SleepingState::Update(float deltaTime, EnemyState& enemy)
{
	//Enemy Component
	TransformComponent* transform = nullptr;
	HealthComponent* health = nullptr;
	for (auto* component : enemy.mp_gameManager->GetEntityManager()->GetComponentsTab()[enemy.mp_enemyEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Health_ID)
		{
			health = static_cast<HealthComponent*>(component);
		}
		if (health && transform)
			break;
	}

	//Player Component
	TransformComponent* transformPlayer = nullptr;
	AttackComponent* attackProj = nullptr;
	for (auto* component : enemy.mp_gameManager->GetEntityManager()->GetComponentsTab()[enemy.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Attack_ID)
		{
			attackProj = static_cast<AttackComponent*>(component);
		}
		if (component->ID == Transform_ID)
		{
			transformPlayer = static_cast<TransformComponent*>(component);
		}
		if (attackProj && transformPlayer)
			break;
	}

	Entity* collideEntity = CollideWithProjectileAttack(enemy.mp_enemyEntity, enemy.mp_gameManager->GetEntityManager());
	if (collideEntity)
	{
		if (health && attackProj)
		{
			health->currentHealth -= attackProj->damage;
		}
		if (transform->m_transform.vScale.z > 1)
		{
			float mediumScale = (transform->m_transform.vScale.z + transform->m_transform.vScale.y + transform->m_transform.vScale.x) / 3;

			transform->m_transform.vScale.z -= transform->m_transform.vScale.z * (mediumScale / transform->m_transform.vScale.z) / 5;
			transform->m_transform.vScale.y -= transform->m_transform.vScale.y * (mediumScale / transform->m_transform.vScale.y) / 5;
			transform->m_transform.vScale.x -= transform->m_transform.vScale.x * (mediumScale / transform->m_transform.vScale.x) / 5;
			transform->m_transform.UpdateMatrix();
		}
		collideEntity->m_tag = Projectile_TAG;
	}
	if (enemy.mp_enemyEntity->m_tag == EnemyBoss_TAG)
		return;

	float dx = transformPlayer->m_transform.vPosition.x - transform->m_transform.vPosition.x;
	float dy = transformPlayer->m_transform.vPosition.y - transform->m_transform.vPosition.y;
	float dz = transformPlayer->m_transform.vPosition.z - transform->m_transform.vPosition.z;
	float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

	if (distance < 0)
		distance = 0;

	if (distance != 0 && distance <= 1000)
	{
		enemy.TransitionTo(EnemyState::Chasing);
	}

}
//Shoot
void ChasingState::Start(EnemyState& enemy)
{
	OutputDebugStringA("imAprojectile");

}
void ChasingState::Update(float deltaTime, EnemyState& enemy)
{
	//OutputDebugStringA("WEEEEEEEEEE");

	//Enemy Component
	TransformComponent* transform = nullptr;
	HealthComponent* health = nullptr;

	for (auto* component : enemy.mp_gameManager->GetEntityManager()->GetComponentsTab()[enemy.mp_enemyEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Health_ID)
		{
			health = static_cast<HealthComponent*>(component);
		}
	}
	//Player Component
	TransformComponent* transformPlayer = nullptr;
	AttackComponent* attackProj = nullptr;

	for (auto* component : enemy.mp_gameManager->GetEntityManager()->GetComponentsTab()[enemy.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transformPlayer = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Attack_ID)
		{
			attackProj = static_cast<AttackComponent*>(component);
		}
	}

	transform->m_transform.FollowTarget(transformPlayer->m_transform.vPosition, 1);
	transform->m_transform.LookAt(transformPlayer->m_transform.vPosition);

	float dx = transformPlayer->m_transform.vPosition.x - transform->m_transform.vPosition.x;
	float dy = transformPlayer->m_transform.vPosition.y - transform->m_transform.vPosition.y;
	float dz = transformPlayer->m_transform.vPosition.z - transform->m_transform.vPosition.z;
	float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

	if (distance > 500)
	{
		enemy.TransitionTo(EnemyState::Sleeping);
	}

	Entity* collideEntity = CollideWithProjectileAttack(enemy.mp_enemyEntity, enemy.mp_gameManager->GetEntityManager());
	if (collideEntity)
	{
		if (health && attackProj)
		{
			health->currentHealth -= attackProj->damage;
		}
		if (transform->m_transform.vScale.z > 1)
		{
			float mediumScale = (transform->m_transform.vScale.z + transform->m_transform.vScale.y + transform->m_transform.vScale.x) / 3;

			transform->m_transform.vScale.z -= transform->m_transform.vScale.z * (mediumScale / transform->m_transform.vScale.z) / 5;
			transform->m_transform.vScale.y -= transform->m_transform.vScale.y * (mediumScale / transform->m_transform.vScale.y) / 5;
			transform->m_transform.vScale.x -= transform->m_transform.vScale.x * (mediumScale / transform->m_transform.vScale.x) / 5;
			transform->m_transform.UpdateMatrix();
		}
		collideEntity->m_tag = Projectile_TAG;
	}
}

void CollidingState::Start(EnemyState& enemy)
{
	OutputDebugStringA("imAfallingProjectile");
}
void CollidingState::Update(float deltaTime, EnemyState& enemy)
{
	/*TransformComponent* transform = nullptr;
	GravityComponent* gravity = nullptr;
	VelocityComponent* velComponent = nullptr;
	EntityManager* entityManager = enemy.mp_gameManager->GetEntityManager();
	for (auto* component : entityManager->GetComponentsTab()[enemy.mp_projectile->mp_objectEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Gravity_ID)
		{
			gravity = static_cast<GravityComponent*>(component);
		}
		if (component->ID == Velocity_ID)
		{
			velComponent = static_cast<VelocityComponent*>(component);
		}
	}*/

	//Enemy Component
	TransformComponent* transform = nullptr;

	for (auto* component : enemy.mp_gameManager->GetEntityManager()->GetComponentsTab()[enemy.mp_enemyEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(component);
		}
	}


	//float gForce = gravity->m_gravityForce * gravity->m_weight;
	//float y = object.mp_projectile->GetStartPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
	////float x = object.mp_player->GetJumpPosY() - (0.5f * gForce * (object.m_fallProgress * object.m_fallProgress));
	//transform->m_transform.vPosition.y = y;
	//transform->m_transform.UpdateMatrix();
	//velComponent->vz = (transformPlayer->m_transform.vScale.z / transform->m_transform.vScale.z) * gravity->m_jumpForce;
	//object.m_fallProgress += 1 * deltaTime;
	//if (object.m_fallProgress > deltaTime)
	//{
	//	int colliderTAG = CollideWith(object.mp_projectile->mp_objectEntity, object.mp_gameManager->GetEntityManager());
	//	if (colliderTAG >= 0)
	//	{
	//		//if (colliderTAG == StickySurface_TAG)
	//		//{
	//		//	// PLAY SOUND
	//		//	object.m_fallProgress = 0;
	//		//	object.TransitionTo(ObjectState::StateType::Sticky);
	//		//}
	//		//else
	//		{
	//			int colliderTAG2 = CollideFromBelow(object.mp_projectile->mp_objectEntity, object.mp_gameManager->GetEntityManager());
	//			if (colliderTAG2 >= 0)
	//			{
	//				{
	//					velComponent->vz = 0;
	//					object.m_fallProgress = 0;
	//					object.TransitionTo(ObjectState::StateType::Placing);
	//					OutputDebugStringA("Player has finished jumping");
	//				}
	//			}
	//		}
	//	}
	//}



	//if (transform->m_transform.vPosition.y < -500) // respawn
	//{
	//	velComponent->vz = 0;
	//	object.m_fallProgress = 0;
	//	object.TransitionTo(ObjectState::StateType::Placing);
	//	object.mp_gameManager->GetEntityManager()->RemoveComponent<ColliderComponent>(object.mp_objectEntity);
	//	object.mp_projectile->SetToDestroy(true);
	//	//delete object.mp_projectile;
	//}
}


bool AABBIntersect(const TransformComponent& a, const TransformComponent& b)
{
	// Calculer les bornes pour lobjet A
	float aMinX = a.m_transform.vPosition.x - a.m_transform.vScale.x / 2;
	float aMaxX = a.m_transform.vPosition.x + a.m_transform.vScale.x / 2;
	float aMinY = a.m_transform.vPosition.y - a.m_transform.vScale.y / 2;
	float aMaxY = a.m_transform.vPosition.y + a.m_transform.vScale.y / 2;
	float aMinZ = a.m_transform.vPosition.z - a.m_transform.vScale.z / 2;
	float aMaxZ = a.m_transform.vPosition.z + a.m_transform.vScale.z / 2;

	// Calculer les bornes pour lobjet B
	float bMinX = b.m_transform.vPosition.x - b.m_transform.vScale.x / 2;
	float bMaxX = b.m_transform.vPosition.x + b.m_transform.vScale.x / 2;
	float bMinY = b.m_transform.vPosition.y - b.m_transform.vScale.y / 2;
	float bMaxY = b.m_transform.vPosition.y + b.m_transform.vScale.y / 2;
	float bMinZ = b.m_transform.vPosition.z - b.m_transform.vScale.z / 2;
	float bMaxZ = b.m_transform.vPosition.z + b.m_transform.vScale.z / 2;

	// Verification du chevauchement sur chaque axe
	if (aMaxX < bMinX || aMinX > bMaxX)
		return false;
	if (aMaxY < bMinY || aMinY > bMaxY)
		return false;
	if (aMaxZ < bMinZ || aMinZ > bMaxZ)
		return false;

	return true;
}

bool IsAbove(const TransformComponent& a, const TransformComponent& b)
{
	// Calcul des bornes pour l'objet A (haut et bas en Y)
	float aMinY = a.m_transform.vPosition.y - a.m_transform.vScale.y / 2;
	//float aMaxY = a.m_transform.vPosition.y + a.m_transform.vScale.y / 2;

	// Calcul des bornes pour l'objet B
	//float bMinY = b.m_transform.vPosition.y - b.m_transform.vScale.y / 2;
	float bMaxY = b.m_transform.vPosition.y + b.m_transform.vScale.y / 2;

	// Verification du chevauchement en X et Z (necessaire pour qu'on considere "au-dessus")
	float aMinX = a.m_transform.vPosition.x - a.m_transform.vScale.x / 2;
	float aMaxX = a.m_transform.vPosition.x + a.m_transform.vScale.x / 2;
	float aMinZ = a.m_transform.vPosition.z - a.m_transform.vScale.z / 2;
	float aMaxZ = a.m_transform.vPosition.z + a.m_transform.vScale.z / 2;

	float bMinX = b.m_transform.vPosition.x - b.m_transform.vScale.x / 2;
	float bMaxX = b.m_transform.vPosition.x + b.m_transform.vScale.x / 2;
	float bMinZ = b.m_transform.vPosition.z - b.m_transform.vScale.z / 2;
	float bMaxZ = b.m_transform.vPosition.z + b.m_transform.vScale.z / 2;

	bool overlapX = (aMaxX > bMinX) && (aMinX < bMaxX);
	bool overlapZ = (aMaxZ > bMinZ) && (aMinZ < bMaxZ);

	if (!(overlapX && overlapZ))
		return false;

	// Calcul de la penetration verticale entre la face inferieure de A et la face superieure de B
	float penetrationY = aMinY - bMaxY;

	// Definir un seuil epsilon (ajustable en fonction de l'echelle de vos objets)
	const float epsilon = 1.0f;

	// Retourne true si la penetration est quasi nulle (touchant)
	return (a.m_transform.vPosition.y > b.m_transform.vPosition.y && penetrationY <= 0 /*&& abs(penetrationY) < epsilon*/);
}
bool IsBelow(const TransformComponent& a, const TransformComponent& b)
{
	// Calcul des bornes pour l'objet A (haut et bas en Y)
	//float aMinY = a.m_transform.vPosition.y - a.m_transform.vScale.y / 2;
	float aMaxY = a.m_transform.vPosition.y + a.m_transform.vScale.y / 2;

	// Calcul des bornes pour l'objet B
	float bMinY = b.m_transform.vPosition.y - b.m_transform.vScale.y / 2;
	//float bMaxY = b.m_transform.vPosition.y + b.m_transform.vScale.y / 2;

	// Verification du chevauchement en X et Z (necessaire pour qu'on considere "au-dessus")
	float aMinX = a.m_transform.vPosition.x - a.m_transform.vScale.x / 2;
	float aMaxX = a.m_transform.vPosition.x + a.m_transform.vScale.x / 2;
	float aMinZ = a.m_transform.vPosition.z - a.m_transform.vScale.z / 2;
	float aMaxZ = a.m_transform.vPosition.z + a.m_transform.vScale.z / 2;

	float bMinX = b.m_transform.vPosition.x - b.m_transform.vScale.x / 2;
	float bMaxX = b.m_transform.vPosition.x + b.m_transform.vScale.x / 2;
	float bMinZ = b.m_transform.vPosition.z - b.m_transform.vScale.z / 2;
	float bMaxZ = b.m_transform.vPosition.z + b.m_transform.vScale.z / 2;

	bool overlapX = (aMaxX > bMinX) && (aMinX < bMaxX);
	bool overlapZ = (aMaxZ > bMinZ) && (aMinZ < bMaxZ);

	if (!(overlapX && overlapZ))
		return false;

	// Calcul de la penetration verticale entre la face inferieure de A et la face superieure de B
	float penetrationY = bMinY - aMaxY;

	// Definir un seuil epsilon (ajustable en fonction de l'echelle de vos objets)
	const float epsilon = 1.0f;

	// Retourne true si la penetration est quasi nulle (touchant)
	return (a.m_transform.vPosition.y < b.m_transform.vPosition.y && penetrationY <= 0 /*&& abs(penetrationY) < epsilon*/);
}

int CollideFromBelow(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component)
		{
			if (component->ID == Transform_ID)
			{
				transform1 = static_cast<TransformComponent*>(component);
			}
		}
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return -1;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (!entity2 || entity == entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component)
			{
				if (component->ID == Transform_ID)
				{
					transform2 = static_cast<TransformComponent*>(component);
				}
			}
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER))
			continue;

		// Test de collision
		if (IsAbove(*transform1, *transform2))
		{
			//float x;
			//if (InputManager::GetKeyIsPressed('H'))
			//{
			//	x = 2; // sert eEdebug avec un point d'arret qui se declenche que quand on appuie sur la touche
			//}
			// PLAY SOUND
			return entity2->m_tag;
		}
	}

	return -1;
}
int CollideFromTop(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return -1;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (!entity2 || entity == entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER))
			continue;

		// Test de collision
		if (IsBelow(*transform1, *transform2))
		{
			//float x;
			//if (InputManager::GetKeyIsPressed('H'))
			//{
			//	x = 2; // sert eEdebug avec un point d'arret qui se declenche que quand on appuie sur la touche
			//}
			// PLAY SOUND
			return entity2->m_tag;
		}
	}

	return -1;
}

int CollideWith(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return -1;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (!entity2 || entity == entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER))
			continue;

		// Test de collision
		if (AABBIntersect(*transform1, *transform2))
		{
			//float x;
			//if (InputManager::GetKeyIsPressed('H'))
			//{
			//	x = 2; // sert eEdebug avec un point d'arret qui se declenche que quand on appuie sur la touche
			//}
			return entity2->m_tag;
		}
	}

	return -1;
}

Entity* CollideWithProjectileAttack(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return nullptr;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (!entity2 || entity == entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER) || entity2->m_tag != ProjectileAttack_TAG /*&& entity2->m_tag != Projectile_TAG)*/)
			continue;

		// Test de collision
		if (AABBIntersect(*transform1, *transform2))
		{
			//float x;
			//if (InputManager::GetKeyIsPressed('H'))
			//{
			//	x = 2; // sert eEdebug avec un point d'arret qui se declenche que quand on appuie sur la touche
			//}
			return entity2;
		}
	}

	return nullptr;
}

Entity* CollideWithEatable(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;
	ColliderComponent* collider1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
		if (component->ID == Collider_ID)
		{
			collider1 = static_cast<ColliderComponent*>(component);
		}
		if (transform1 && collider1)
			break;
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER) || !collider1->m_isDrawable)
		return nullptr;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (entity2 == nullptr)
			continue;
		if (entity2->m_tag != EatableObject_TAG && entity2->m_tag != Projectile_TAG && entity2->m_tag != PowerJumping_TAG && entity2->m_tag != Enemy_TAG)
			continue;
		if (entity == entity2)
			continue;

		TransformComponent* transform2 = nullptr;
		ColliderComponent* collider2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
			if (component->ID == Collider_ID)
			{
				collider2 = static_cast<ColliderComponent*>(component);
			}
			if (transform2 && collider2)
				break;
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER) || !collider2->m_isDrawable)
			continue;

		// Test de collision
		if (AABBIntersect(*transform1, *transform2))
		{
			return entity2;
		}
	}

	return nullptr;
}

int CollideWithSticky(Entity* entity, EntityManager* entityManager)
{

	TransformComponent* transform1 = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transform1 = static_cast<TransformComponent*>(component);
		}
	}
	if (!transform1 || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return -1;

	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (entity2->m_tag != StickySurface_TAG)
			continue;
		if (!entity2 || entity == entity2) // Pas necessaire ?
			continue;

		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}
		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER))
			continue;

		// Test de collision
		if (AABBIntersect(*transform1, *transform2))
		{
			return entity2->m_tag;
		}
	}

	return -1;
}

Entity* CollideWithTag(ObjetsTAG tag, EntityManager* entityManager, Entity* entity)
{

	TransformComponent* transformPlayer = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			transformPlayer = static_cast<TransformComponent*>(component);
		}
	}
	if (!transformPlayer || !entityManager->HasComponent(entity, COMPONENT_COLLIDER))
		return nullptr;


	for (auto& entity2 : entityManager->GetEntityTab())
	{
		if (!entity2 || entity == entity2)
			continue;
		TransformComponent* transform2 = nullptr;

		for (auto* component : entityManager->GetComponentsTab()[entity2->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				transform2 = static_cast<TransformComponent*>(component);
			}
		}

		if (!transform2 || !entityManager->HasComponent(entity2, COMPONENT_COLLIDER) || entity2->m_tag != tag)
			continue;

		// Test de collision
		if (AABBIntersect(*transformPlayer, *transform2))
		{
			return entity2;
		}
	}

	return nullptr;
}

void CreateItem(PlayerState& player, std::wstring textureID, float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ)
{
	EntityManager* entityManager = player.mp_gameManager->GetEntityManager();

	TransformComponent* playerTransform = nullptr;

	for (auto* component : entityManager->GetComponentsTab()[player.mp_playerEntity->tab_index]->vec_components)
	{
		if (component->ID == Transform_ID)
		{
			playerTransform = static_cast<TransformComponent*>(component);
		}
	}

	if (textureID == L"DroneTexture")
	{
		Entity* ennemy = entityManager->CreateEntity();
		//ennemy->m_tag = Projectile_TAG;
		ennemy->m_tag = EatableObject_TAG;

		entityManager->AddComponent<TransformComponent>(ennemy);
		entityManager->AddComponent<MeshComponent>(ennemy);
		entityManager->AddComponent<ColliderComponent>(ennemy);
		entityManager->AddComponent<EnnemyComponent>(ennemy);
		entityManager->AddComponent<AttackComponent>(ennemy);
		entityManager->AddComponent<HealthComponent>(ennemy);
		entityManager->AddComponent<HighlightComponent>(ennemy);

		for (auto& comp : entityManager->GetComponentToAddTab()[ennemy->tab_index]->vec_components)
		{
			if (comp->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(comp);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateCube();
				mesh->m_textureID = L"DroneTexture";
			}
			if (comp->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(comp);
				transform->m_transform.Scale(sizeX, sizeY, sizeZ);
				transform->m_transform.Move(posZ, posX, posY);
				float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
				float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
				float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
				transform->m_transform.Rotation(roll, pitch, yaw);
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
	else if (textureID == L"MeteorTexture")
	{
		Entity* meteor = entityManager->CreateEntity();
		meteor->m_tag = Projectile_TAG;

		entityManager->AddComponent<TransformComponent>(meteor);
		entityManager->AddComponent<MeshComponent>(meteor);
		entityManager->AddComponent<ColliderComponent>(meteor);
		entityManager->AddComponent<AttackComponent>(meteor);
		entityManager->AddComponent<HealthComponent>(meteor);
		entityManager->AddComponent<HighlightComponent>(meteor);
		entityManager->AddComponent<VelocityComponent>(meteor);
		entityManager->AddComponent<GravityComponent>(meteor);

		for (auto& comp : entityManager->GetComponentToAddTab()[meteor->tab_index]->vec_components)
		{
			if (comp->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(comp);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateCube();
				mesh->m_textureID = L"MeteorTexture";
			}
			if (comp->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(comp);
				transform->m_transform.Scale(sizeX, sizeY, sizeZ);
				transform->m_transform.Move(posZ, posX, posY);
				float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
				float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
				float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
				transform->m_transform.Rotation(roll, pitch, yaw);
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
			/*if (comp->ID == Collider_ID)
			{
				ColliderComponent* collider = static_cast<ColliderComponent*>(comp);
				collider->m_isDestructable = true;
			}*/
		}

		//ObjectStateMachine
		Projectile* newProjectile = new Projectile;
		newProjectile->SetObjectEntity(meteor);
		newProjectile->SetPlayerEntity(player.GetPlayerEntity());
		newProjectile->SetStartPosY(posY);
		//newProjectile->GetObjectState().Shoot();
		newProjectile->Init(player.mp_gameManager);
		player.mp_player->GetProjectileList().push_back(newProjectile);

	}
	else if (textureID == L"FireTexture")
	{
		Entity* jumpPad = entityManager->CreateEntity();
		entityManager->AddComponent<TransformComponent>(jumpPad);
		entityManager->AddComponent<MeshComponent>(jumpPad);
		entityManager->AddComponent<ColliderComponent>(jumpPad);
		jumpPad->m_tag = PowerJumping_TAG;

		// Parcours des composants pour configurer le mur
		for (auto& comp : entityManager->GetComponentToAddTab()[jumpPad->tab_index]->vec_components)
		{
			if (comp->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(comp);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateCube();
				mesh->m_textureID = L"FireTexture";
			}
			if (comp->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(comp);
				transform->m_transform.Scale(sizeX, sizeY, sizeZ);
				transform->m_transform.Move(posZ, posX, posY);
				float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
				float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
				float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
				transform->m_transform.Rotation(roll, pitch, yaw);
			}
		}

	}
	else if (textureID == L"BoxTexture")
	{
		Entity* newBoxBlock = entityManager->CreateEntity();
		entityManager->AddComponent<TransformComponent>(newBoxBlock);
		entityManager->AddComponent<MeshComponent>(newBoxBlock);
		entityManager->AddComponent<ColliderComponent>(newBoxBlock);
		entityManager->AddComponent<HighlightComponent>(newBoxBlock);
		entityManager->AddComponent<GravityComponent>(newBoxBlock);
		newBoxBlock->m_tag = EatableObject_TAG;

		for (auto& comp : entityManager->GetComponentToAddTab()[newBoxBlock->tab_index]->vec_components)
		{
			if (comp->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(comp);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateCube();
				mesh->m_textureID = L"BoxTexture";
			}
			if (comp->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(comp);
				transform->m_transform.Scale(sizeX, sizeY, sizeZ);
				transform->m_transform.Move(posZ, posX, posY);
				float yaw = std::atan2(playerTransform->m_transform.mRotation._31, playerTransform->m_transform.mRotation._33);
				float pitch = std::asin(-playerTransform->m_transform.mRotation._32);
				float roll = std::atan2(playerTransform->m_transform.mRotation._12, playerTransform->m_transform.mRotation._22);
				transform->m_transform.Rotation(roll, pitch, yaw);
			}
			if (comp->ID == Highlight_ID)
			{
				HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
				highlight->isHighlighted = true;
				highlight->intensity = 2.f;
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
		//newSceneObject->SetPlayerEntity(nullptr);
		newSceneObject->Init(player.mp_gameManager);
		player.mp_player->mp_scene->GetSceneObjectList().push_back(newSceneObject);

	}
}

void CreateTextBox(PlayerState& player, std::string textureText)
{
	if (player.mp_player->GetUIList().size() <= 0)
	{
		EntityManager* entityManager = player.mp_gameManager->GetEntityManager();

		// UI ELEMENTS
		Entity* entityTextBox = entityManager->CreateEntity();

		entityManager->AddComponent<UIComponent>(entityTextBox);
		entityManager->AddComponent<TransformComponent>(entityTextBox);
		entityManager->AddComponent<MeshComponent>(entityTextBox);
		entityTextBox->m_tag = UI_TAG;

		for (auto& component : entityManager->GetComponentToAddTab()[entityTextBox->tab_index]->vec_components)
		{
			if (component->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(component);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateQuadUI(600, 200);
				
				std::wstring wTextureName(textureText.begin(), textureText.end());
				mesh->m_textureID = wTextureName;
			}
			if (component->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(component);
				//transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
				//transform->m_transform.vPosition = { 300 / 4, 300 / 4, 0.0f };
				transform->m_transform.vPosition = { 1000 / 4, 800 / 4, 0 };  // Centre sur (400,300) en divisant par 4 non par 2
				transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
			}
		}

		player.mp_player->GetUIList().push_back(entityTextBox);
	}
}

void CreateTextBoss(PlayerState& player, std::string textureText)
{
	if (player.mp_player->GetUIBossList().size() <= 0)
	{
		EntityManager* entityManager = player.mp_gameManager->GetEntityManager();

		// UI ELEMENTS
		Entity* entityTextBox = entityManager->CreateEntity();

		entityManager->AddComponent<UIComponent>(entityTextBox);
		entityManager->AddComponent<TransformComponent>(entityTextBox);
		entityManager->AddComponent<MeshComponent>(entityTextBox);
		entityTextBox->m_tag = UI_TAG;

		for (auto& component : entityManager->GetComponentToAddTab()[entityTextBox->tab_index]->vec_components)
		{
			if (component->ID == Mesh_ID)
			{
				MeshComponent* mesh = static_cast<MeshComponent*>(component);
				mesh->m_mesh = player.mp_gameManager->GetFactory()->CreateQuadUI(600, 200);

				std::wstring wTextureName(textureText.begin(), textureText.end());
				mesh->m_textureID = wTextureName;
			}
			if (component->ID == Transform_ID)
			{
				TransformComponent* transform = static_cast<TransformComponent*>(component);
				//transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
				//transform->m_transform.vPosition = { 300 / 4, 300 / 4, 0.0f };
				transform->m_transform.vPosition = { 1000 / 4, 800 / 4 + 150, 0 };  // Centre sur (400,300) en divisant par 4 non par 2
				transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
			}
		}

		player.mp_player->GetUIBossList().push_back(entityTextBox);
	}
}