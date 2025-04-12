#include "pch.h"
#include "CameraSystem.h"
#include "InitDirect3DApp.h"
#include "InputManager.h"

void CameraSystem::Initialize(InitDirect3DApp* gameManager)
{
	mp_gameManager = gameManager;
	m_viewMatrix = DefaultView();
}

void CameraSystem::Update()
{
	//for (Entity* entity : mp_gameManager->GetEntityManager()->GetEntityTab())
	//{
	//	if (!entity)
	//		continue;
	//	if (!mp_gameManager->GetEntityManager()->HasComponent(entity, COMPONENT_CAMERA))
	//		continue;

	//	TransformComponent* transformComponent = nullptr;
	//	CameraComponent* camComponent = nullptr;


	//	for (auto* component : mp_gameManager->GetEntityManager()->GetComponentsTab()[entity->tab_index]->vec_components)
	//	{
	//		if (component->ID == Transform_ID)
	//		{
	//			transformComponent = static_cast<TransformComponent*>(component);
	//		}
	//		if (component->ID == Camera_ID)
	//		{
	//			camComponent = static_cast<CameraComponent*>(component);
	//		}
	//	}

	//	if (FPS)
	//	{
	//		if (transformComponent != nullptr && camComponent != nullptr)
	//		{
	//			camComponent->m_cameraTransform.vPosition = transformComponent->m_transform.vPosition;
	//			camComponent->m_cameraTransform.qRotation = transformComponent->m_transform.qRotation;
	//			camComponent->m_cameraTransform.UpdateMatrix();
	//			SetViewMatrix(&camComponent->m_cameraTransform);
	//		}
	//	}

	//	if (TPS)
	//	{


	//		DirectX::XMVECTOR cubePos = DirectX::XMLoadFloat3(&transformComponent->m_transform.vPosition);

	//		if (camComponent->freeCam)
	//		{
	//			float baseDistanceBehind = 3 * transformComponent->m_transform.vScale.z;
	//			float scaleValue = transformComponent->m_transform.vScale.z;

	//			// Offset de base dans l'espace cam�ra (en local)
	//			DirectX::XMVECTOR localOffset = DirectX::XMVectorSet(0.0f, scaleValue, -baseDistanceBehind - scaleValue, 0.0f);


	//			// Appliquer la rotation stock�e dans ta matrice (mise � jour via ta fonction Rotation)
	//			DirectX::XMMATRIX camWorldMatrix = DirectX::XMLoadFloat4x4(&camComponent->m_cameraTransform.matrix);

	//			// L'offset tourn�
	//			DirectX::XMVECTOR offsetTransformed = DirectX::XMVector3Transform(localOffset, camWorldMatrix);

	//			// Position de l'objet (cube)
	//			DirectX::XMVECTOR cubePos = DirectX::XMLoadFloat3(&transformComponent->m_transform.vPosition);

	//			// Position finale de la cam�ra (en ajoutant l'offset tourn� � la position du cube)
	//			DirectX::XMVECTOR eyePos = DirectX::XMVectorAdd(offsetTransformed, cubePos);

	//			// Direction up fixe
	//			DirectX::XMVECTOR upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//			// Matrice de vue
	//			DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePos, cubePos, upDir);

	//			SetViewMatrix(viewMatrix);
	//		}
	//		else
	//		{

	//			float distanceBehind = 5 * powf(transformComponent->m_transform.vScale.z, 1.0f / 1000.0f);
	//			float distanceUp = 5 * powf(transformComponent->m_transform.vScale.z, 1.0f / 1000.0f) / 3;

	//			float fdistanceBehind = 10 * 4.0f;
	//			float fdistanceUp = 10;

	//			DirectX::XMVECTOR localOffset;

	//			DirectX::XMVECTOR eyePos;
	//			DirectX::XMVECTOR upDir;

	//			if (TPS_Lock == false)
	//			{
	//				DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transformComponent->m_transform.matrix);
	//				localOffset = DirectX::XMVectorSet(0.0f, distanceUp, -distanceBehind, 0.0f);
	//				eyePos = DirectX::XMVector3Transform(localOffset, worldMatrix);
	//				upDir = DirectX::XMLoadFloat3(&transformComponent->m_transform.vUp);
	//			}
	//			else
	//			{
	//				localOffset = DirectX::XMVectorSet(0.0f, fdistanceUp, -fdistanceBehind, 0.0f);
	//				eyePos = DirectX::XMVectorAdd(cubePos, localOffset);
	//				upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//			}

	//			DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePos, cubePos, upDir);

	//			SetViewMatrix(viewMatrix);
	//		}
	//	}
	//}
	 // Mise � jour de l'input
	InputManager::UpdateMouse(GetActiveWindow());
	int deltaX = InputManager::GetMouseDeltaX();
	int deltaY = InputManager::GetMouseDeltaY();
	const float sensitivity = 0.005f;

	m_yaw += deltaX * sensitivity;
	m_pitch += deltaY * sensitivity;

	// Limiter le pitch pour �viter un retournement
	if (m_pitch > DirectX::XM_PIDIV2 - 0.01f)
		m_pitch = DirectX::XM_PIDIV2 - 0.01f;
	if (m_pitch < -DirectX::XM_PIDIV2 + 0.01f)
		m_pitch = -DirectX::XM_PIDIV2 + 0.01f;

	

	// Pour chaque entit� poss�dant un composant cam�ra
	for (Entity* entity : mp_gameManager->GetEntityManager()->GetEntityTab())
	{
		if (!entity || !mp_gameManager->GetEntityManager()->HasComponent(entity, COMPONENT_CAMERA))
			continue;

		TransformComponent* transformComponent = nullptr;
		CameraComponent* camComponent = nullptr;
		for (auto* component : mp_gameManager->GetEntityManager()->GetComponentsTab()[entity->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
				transformComponent = static_cast<TransformComponent*>(component);
			if (component->ID == Camera_ID)
				camComponent = static_cast<CameraComponent*>(component);
		}
		if (!transformComponent || !camComponent)
			continue;

		// Appliquer le yaw de fa�on absolue sur l'objet
		// On calcule directement le quaternion correspondant � (roll=0, pitch=0, yaw=m_yaw)
		DirectX::XMVECTOR qObj = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, m_yaw, 0.0f);
		DirectX::XMStoreFloat4(&transformComponent->m_transform.qRotation, qObj);
		transformComponent->m_transform.UpdateMatrix();

		// Calculer l'offset de la cam�ra en coordonn�es sph�riques.
		// Seul le pitch intervient ici pour �lever ou abaisser la cam�ra.
		float cameraFOV = DirectX::XM_PIDIV4;
		float dynamicDistance = -5.f * ((transformComponent->m_transform.vScale.z / 2.0f) / tanf(cameraFOV / 2.0f));

		// R�cup�rer le delta de la molette de la souris pour ajuster manuellement la distance
		int wheelDelta = InputManager::GetMouseWheelDelta();  // Assure-toi que cette fonction existe et renvoie une valeur positive ou n�gative
		const float wheelSensitivity = 0.01f * transformComponent->m_transform.vScale.z; // Ajuste cette sensibilit� selon tes besoins
		m_manualDistance += wheelDelta * wheelSensitivity;
		if (m_manualDistance <= dynamicDistance * 5)
			m_manualDistance = dynamicDistance * 5;


		if (m_manualDistance >= -dynamicDistance )
		{
			m_manualDistance -= wheelDelta * wheelSensitivity;
			//m_manualDistance = -dynamicDistance;
		}

		//wchar_t title[256];
		//swprintf_s(title, 256, L"player state: %f, %f", m_manualDistance, dynamicDistance);
		//SetWindowText(GetActiveWindow(), title);
			// La distance finale est la somme de la distance dynamique et de l'offset manuel.
		float finalDistance = dynamicDistance + m_manualDistance;

		DirectX::XMFLOAT3 offset;
		offset.x = finalDistance * cosf(m_pitch) * sinf(m_yaw);
		offset.y = -finalDistance * sinf(m_pitch);
		offset.z = finalDistance * cosf(m_pitch) * cosf(m_yaw);

		// Calculer la position de la cam�ra : position de l'objet + offset
		DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&transformComponent->m_transform.vPosition);
		DirectX::XMVECTOR cameraPos = DirectX::XMVectorAdd(targetPos, DirectX::XMLoadFloat3(&offset));

		// Limiter la position verticale de la cam�ra - PAS TROP FONCTIONNEL
		//DirectX::XMFLOAT3 camPos;
		//DirectX::XMStoreFloat3(&camPos, cameraPos);
		//const float minY = -60.0f;  // Limite inf�rieure 
		//const float maxY = 120.0f; // Limite sup�rieure 
		//if (camPos.y < minY) camPos.y = minY;
		//if (camPos.y > maxY) camPos.y = maxY;
		//cameraPos = DirectX::XMLoadFloat3(&camPos);

		// Mettre � jour la transformation de la cam�ra gr�ce � LookAt
		XMStoreFloat3(&camComponent->m_cameraTransform.vPosition, cameraPos);
		camComponent->m_cameraTransform.LookAt(transformComponent->m_transform.vPosition);
		camComponent->m_cameraTransform.UpdateMatrix();

		// Mettre � jour la vue via XMMatrixLookAtLH (optionnel si ta cam�ra utilise sa propre matrice)
		DirectX::XMVECTOR upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, targetPos, upDir);
		SetViewMatrix(viewMatrix);
	}
}

DirectX::XMMATRIX CameraSystem::DefaultView()
{
	DirectX::XMFLOAT3 position(0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);

	float cosPitch = cosf(0.0f);
	float sinPitch = sinf(0.0f);
	float cosYaw = cosf(0.0f);
	float sinYaw = sinf(0.0f);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(cosPitch * sinYaw, sinPitch, cosPitch * cosYaw, 0.0f);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return DirectX::XMMatrixLookToLH(pos, forward, up);
}

void CameraSystem::SetViewMatrix(Transform* transform)
{
	DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform->GetMatrix());
	m_viewMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);
}

void CameraSystem::SetViewMatrix(DirectX::XMMATRIX viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

