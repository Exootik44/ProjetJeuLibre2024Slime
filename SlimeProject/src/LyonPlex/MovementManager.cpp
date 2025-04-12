#include "pch.h"
#include "MovementManager.h"

#include "InitDirect3DApp.h"

void MovementManager::Initialize(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
	m_limitPosMin = -50000; 
	m_limitPosMax = 50000; 

	m_maxJumpTime = 0.5;
	/*m_jumpTime = */m_fallTime = 1;
	m_jumpTime /*= m_fallTime */ = 0;
}

void MovementManager::Update(float deltaTime)
{
	for (Entity* entity : mp_gameManager->GetEntityManager()->GetEntityTab())
	{
		if (!entity)
			continue;


		if (mp_gameManager->GetEntityManager()->HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_VELOCITY))
		{
			TransformComponent* transform = nullptr;
			VelocityComponent* velocity = nullptr;


			for (auto* component : mp_gameManager->GetEntityManager()->GetComponentsTab()[entity->tab_index]->vec_components)
			{
				if (component->ID == Transform_ID)
				{
					transform = static_cast<TransformComponent*>(component);
				}
				if (component->ID == Velocity_ID)
				{
					velocity = static_cast<VelocityComponent*>(component);
				}
			}

			if (mp_gameManager->GetEntityManager()->HasComponent(entity, COMPONENT_GRAVITY))
			{
				GravityComponent* gravity = nullptr;

				for (auto* component : mp_gameManager->GetEntityManager()->GetComponentsTab()[entity->tab_index]->vec_components)
				{
					if (component->ID == Gravity_ID)
					{
						gravity = static_cast<GravityComponent*>(component);
					}
				}

				if (gravity && transform)
				{
					// Quand le joueur saute
					//if (transform->m_isJumping)
					//{
					//	if (m_jumpStart)
					//	{

					//	}
					//	
					//	float y = transform->m_transform.vPosition.y + (gravity->m_jumpForce * m_jumpTime) - (0.5f * gravity->m_gravityForce * (m_jumpTime * m_jumpTime));
					//	transform->m_transform.vPosition.y = y;
					//	m_jumpTime += deltaTime;  // Incrémente le temps


					//}
					/*else
					{
						m_jumpTime = 0;
					}*/

					//if (gravity->m_gravityApplied)
					//{
					//	// Application constante de gravity
					///*	float gForce = gravity->m_gravityForce * gravity->m_weight;
					//	transform->m_transform.vPosition.y -= gForce * deltaTime;*/
					//}
				}
			}

			if (transform != nullptr && velocity != nullptr)
			{
				Move(entity, velocity, transform, deltaTime);
			}
		}
	}
}

void MovementManager::SetVelocity(VelocityComponent* velComponent, float velFront, float velRight, float velUp)
{
	velComponent->vz = velFront;
	velComponent->vx = velRight;
	velComponent->vy = velUp;
}

void MovementManager::Move(Entity* entity, VelocityComponent* velComponent, TransformComponent* transformComponent, float deltaTime)
{
	transformComponent->m_transform.Move(velComponent->vz * deltaTime, velComponent->vx * deltaTime, velComponent->vy * deltaTime);

	
	
	if (mp_gameManager->GetEntityManager()->HasComponent(entity, COMPONENT_PLAYER)) 
	{
		if (transformComponent->m_transform.GetPositionX() > m_limitPosMax)
		{
			transformComponent->m_transform.vPosition.x = m_limitPosMax;
		}
		else if (transformComponent->m_transform.GetPositionX() < m_limitPosMin)
		{
			transformComponent->m_transform.vPosition.x = m_limitPosMin;
		}
		if (transformComponent->m_transform.GetPositionY() > m_limitPosMax)
		{
			transformComponent->m_transform.vPosition.y = m_limitPosMax;
		}
		else if (transformComponent->m_transform.GetPositionY() < m_limitPosMin)
		{
			transformComponent->m_transform.vPosition.y = m_limitPosMin;
		}
		if (transformComponent->m_transform.GetPositionZ() > m_limitPosMax)
		{
			transformComponent->m_transform.vPosition.z = m_limitPosMax;
		}
		else if (transformComponent->m_transform.GetPositionZ() < m_limitPosMin)
		{
			transformComponent->m_transform.vPosition.z = m_limitPosMin;
		}
		SetVelocity(velComponent, 0, 0, 0);
	}
	else
	{
		if (transformComponent->m_transform.GetPositionX() > m_limitPosMax || transformComponent->m_transform.GetPositionX() < m_limitPosMin
			|| transformComponent->m_transform.GetPositionY() > m_limitPosMax || transformComponent->m_transform.GetPositionY() < m_limitPosMin
			|| transformComponent->m_transform.GetPositionZ() > m_limitPosMax || transformComponent->m_transform.GetPositionZ() < m_limitPosMin)
		{
			//mp_gameManager->GetEntityManager()->ToDestroy(entity);
		}
	}
}
