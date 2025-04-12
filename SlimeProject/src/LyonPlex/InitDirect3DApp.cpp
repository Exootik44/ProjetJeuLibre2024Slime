#include "pch.h"

#include "InitDirect3DApp.h"

#include "MeshFactory.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "HealthSystem.h"
#include "AttackSystem.h"
#include "LightSystem.h"
#include "DoorSystem.h"
#include "SunOrbitSystem.h"

#include "Scene.h"
//#include "SceneTest.h"
//#include "GameScene.h"
//#include "SandBoxScene.h"

#include "TextureManager.h"
#include "CameraSystem.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "EnnemyManager.h"
#include "MovementManager.h"
#include "LifeTimeManager.h"

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : WindowDX(hInstance), m_pauseScreenCreated(false), m_mouseIsOnButton(false)
{
	m_lastTime = timeGetTime();
}

InitDirect3DApp::~InitDirect3DApp()
{
	mp_scene->OnClose();
	delete mp_healthSystem;
	delete mp_attackSystem;
	delete mp_doorSystem;
	delete mp_sunOrbitSystem;
	delete mp_meshFactory;
	delete mp_entityManager;
	delete mp_textureManager;
	delete mp_colliderManager;
	delete mp_particleManager;
	delete mp_ennemyManager;
	delete mp_movementManager;
	delete mp_lifeTimeManager;
	delete mp_cameraManager;
	delete mp_scene;
}

bool InitDirect3DApp::Initialize(Scene* scene)
{
	if (!WindowDX::Initialize())
		return false;

	FlushCommandQueue();

	m_currentFence = 0;

	HRESULT hr = m_d3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(hr))
	{
		MessageBox(0, L"echec de la creation du Fence.", L"Erreur", MB_OK);
		return false;
	}

	// Init les frame Ressources
	for (int i = 0; i < gNum_frameResources; i++)
	{
		auto frameResource = std::make_unique<FrameResource>();
		m_d3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameResource->m_cmdListAlloc));
		frameResource->m_fence = 0;
		m_frameResources.push_back(std::move(frameResource));
	}

	m_currFrameResource = m_frameResources[0].get();


	m_depthStencilDesc = {};
	m_depthStencilDesc.DepthEnable = TRUE;
	m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	m_depthStencilDesc.StencilEnable = FALSE;

	// Cree le pipeline(root signature & PSO)
	CreatePipelineState(); // 3D Scene
	CreateUIPipelineStatev2(); // 2D UI

	// Reinitialiser le command allocator et la command list
	m_currFrameResource->m_cmdListAlloc->Reset();
	m_commandList->Reset(m_currFrameResource->m_cmdListAlloc.Get(), nullptr);

	if (!InitTexture())
	{
		MessageBox(0, L"echec du chargement de la texture !", L"Erreur", MB_OK);
		return false;
	}


	m_commandList->Close();
	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();

	// Initialisation Game Manager et Scene (ECS)
	mp_entityManager = new EntityManager();

	// MeshFactory
	mp_meshFactory = new MeshFactory;
	mp_meshFactory->Initialize(m_d3DDevice.Get(), mp_entityManager, this);

	// Particles
	mp_particleManager = new ParticleManager;
	mp_particleManager->Initialize(this);

	// Collider
	mp_colliderManager = new ColliderManager;
	mp_colliderManager->Initialize(mp_entityManager, mp_particleManager);

	// Ennemy
	mp_ennemyManager = new EnnemyManager;
	mp_ennemyManager->Initialize(this);

	// AttackSystem
	mp_attackSystem = new AttackSystem;
	mp_attackSystem->Initialize(this);

	// LightSystem
	mp_lightSystem = new LightSystem;
	mp_lightSystem->Initialize(this);

	// DoorSystem
	mp_doorSystem = new DoorSystem;
	mp_doorSystem->Initialize(this);

	// SunOrbitSystem
	mp_sunOrbitSystem = new SunOrbitSystem;
	mp_sunOrbitSystem->Initialize(this);

	// HealthManager
	mp_healthSystem = new HealthSystem;
	mp_healthSystem->Initialize(this);

	// MouvementManager
	mp_movementManager = new MovementManager;
	mp_movementManager->Initialize(this);

	// CameraManager
	mp_cameraManager = new CameraSystem;
	mp_cameraManager->Initialize(this);

	// LifeTimeManager
	mp_lifeTimeManager = new LifeTimeManager;
	mp_lifeTimeManager->Initialize(this);

	// Scene
	//SandBoxScene* scene = new SandBoxScene;
	//GameScene* scene = new GameScene;
	//SceneTest* scene = new SceneTest;
	SetScene(scene);
	mp_scene->Initialize(this);
	mp_scene->OnInitialize();

	if (!InitConstantBuffer())
	{
		MessageBox(0, L"echec du chargement des constant buffer !", L"Erreur", MB_OK);
		return false;
	}

	/*m_gameIsPaused = false;*/
	return true;
}

bool InitDirect3DApp::InitConstantBuffer()
{
	// Pour la lumiere :
	// Calculez la taille necessaire pour le constant buffer
	UINT passCBSize = (sizeof(PassConstants) + 255) & ~255;

	// Creez un resource upload buffer pour le constant buffer
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(passCBSize);

	HRESULT hr = m_d3DDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_passConstantBuffer)
	);
	if (FAILED(hr))
	{
		// Gestion d'erreur
	}

	// Mappez le constant buffer et obtenez un pointeur qui y est associe
	hr = m_passConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mp_mappedPassCB));
	if (FAILED(hr))
	{
		// Gestion d'erreur
	}

	// Initialiser le buffer a zero
	memset(mp_mappedPassCB, 0, passCBSize);

	return true;
}

bool InitDirect3DApp::InitTexture()
{
	// Creation du TextureManager
	mp_textureManager = new TextureManager(m_d3DDevice.Get(), m_commandList.Get());
	// On cree un heap pour le nombre total de textures
	mp_textureManager->CreateDescriptorHeap(60);

	// Chargement des textures en appelant LoadTexture pour chaque ressource
	if (!mp_textureManager->LoadTexture(L"MetalTexture", L"../../../src/LyonPlex/metalWall.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture metalWall.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"StickyTexture", L"../../../src/LyonPlex/stickyV2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture stickyWall.", L"Erreur", MB_OK);
		return false;
	}	
	if (!mp_textureManager->LoadTexture(L"LeafTexture", L"../../../src/LyonPlex/leaf.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture leaf.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"GroundTexture", L"../../../src/LyonPlex/ground.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture ground.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"JumpadTexture", L"../../../src/LyonPlex/jumper.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture jumper.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"WoodTexture", L"../../../src/LyonPlex/wood.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture wood.", L"Erreur", MB_OK);
		return false;
	}	
	if (!mp_textureManager->LoadTexture(L"PauseTexture", L"../../../src/LyonPlex/pausemenu.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture pausemenu.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"PlayerTexture", L"../../../src/LyonPlex/slime.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Player.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"WallTexture", L"../../../src/LyonPlex/stone.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Wall.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BoxTexture", L"../../../src/LyonPlex/WoodCrate.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Box.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"IceTexture", L"../../../src/LyonPlex/ice.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Ice.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"FireTexture", L"../../../src/LyonPlex/fire.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Fire.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DroneTexture", L"../../../src/LyonPlex/Drone.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Ice.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"SkyBox", L"../../../src/LyonPlex/SkyBoxTexture.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Ice.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"SkyBox2", L"../../../src/LyonPlex/SkyBox2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture SkyBox2.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DefaultTexture", L"../../../src/LyonPlex/defaultTexture.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture DefaultTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BlueBeamTexture", L"../../../src/LyonPlex/blueBeam.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture BlueBeamTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"RedBeamTexture", L"../../../src/LyonPlex/redBeam.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture RedBeamTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"PlayerBeamTexture", L"../../../src/LyonPlex/BeamPlayerV2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture PlayerBeamTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"EnnemyBeamTexture", L"../../../src/LyonPlex/BeamEnnemyV2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture EnnemyBeamTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"MeteorTexture", L"../../../src/LyonPlex/meteor.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture MeteorTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"TileTexture", L"../../../src/LyonPlex/tile.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture TileTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"PnjTexture", L"../../../src/LyonPlex/robot.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Pnj.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"TextBox1Texture", L"../../../src/LyonPlex/text1.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture TextBox.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"TextBox2Texture", L"../../../src/LyonPlex/text2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture TextBox.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"TextBox3Texture", L"../../../src/LyonPlex/text3.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture TextBox.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"TextBoxTutoDoorTexture", L"../../../src/LyonPlex/txtTutoDoor.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture txtTutoDoor.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"AppleTexture", L"../../../src/LyonPlex/apple.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Apple.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"GrassTexture", L"../../../src/LyonPlex/grass.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture TileTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"SkyboxTexture", L"../../../src/LyonPlex/skyboxDay.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture Skybox.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"SkyboxApoTexture", L"../../../src/LyonPlex/skyboxApo.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture skyboxApo.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"ReturnBtnTexture", L"../../../src/LyonPlex/returnBtn.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture returnBtn.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"WaterTexture", L"../../../src/LyonPlex/water.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture water.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DoorTexture", L"../../../src/LyonPlex/door.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture DoorTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"ButtonTexture", L"../../../src/LyonPlex/button.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture ButtonTexture.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DolemTexture", L"../../../src/LyonPlex/dolem.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture dolem.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"QuestOrbCompleteTexture", L"../../../src/LyonPlex/textCompletedOrbQuest.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture textCompletedOrbQuest.", L"Erreur", MB_OK);
		return false;
	}

	if (!mp_textureManager->LoadTexture(L"DolemRedTexture", L"../../../src/LyonPlex/dolemRed.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture dolemRed.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DolemGreenTexture", L"../../../src/LyonPlex/dolemGreen.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture dolemGreen.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"DolemChiefTexture", L"../../../src/LyonPlex/dolemChief.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture dolemChief.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossTexture", L"../../../src/LyonPlex/boss.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture boss.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossBallTexture", L"../../../src/LyonPlex/bossBall.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossBall.", L"Erreur", MB_OK);
		return false;
	}

	if (!mp_textureManager->LoadTexture(L"BossHpTexture6", L"../../../src/LyonPlex/bossHp6.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp6.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture5", L"../../../src/LyonPlex/bossHp5.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp5.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture4", L"../../../src/LyonPlex/bossHp4.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp4.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture3", L"../../../src/LyonPlex/bossHp3.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp3.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture2", L"../../../src/LyonPlex/bossHp2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp2.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture1", L"../../../src/LyonPlex/bossHp1.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp1.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"BossHpTexture0", L"../../../src/LyonPlex/bossHp0.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bossHp0.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"blueQuestTexture", L"../../../src/LyonPlex/blueQuest.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture blueQuest.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"redQuestTexture", L"../../../src/LyonPlex/redQuest.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture redQuest.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"greenQuestTexture", L"../../../src/LyonPlex/greenQuest.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture greenQuest.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"quest1Texture", L"../../../src/LyonPlex/quest1.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture quest1.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"quest2Texture", L"../../../src/LyonPlex/quest2.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture quest2.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"quest3Texture", L"../../../src/LyonPlex/quest3.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture quest3.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"projectileRockTexture", L"../../../src/LyonPlex/projectileRock.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture projectileRock.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"altarTexture", L"../../../src/LyonPlex/altar.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture altar.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"wallV2Texture", L"../../../src/LyonPlex/wall.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture wall.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"bridgeTexture", L"../../../src/LyonPlex/bridge.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture bridge.", L"Erreur", MB_OK);
		return false;
	}
	if (!mp_textureManager->LoadTexture(L"winTexture", L"../../../src/LyonPlex/win.dds"))
	{
		MessageBox(0, L"echec du chargement de la texture win.", L"Erreur", MB_OK);
		return false;
	}
	return true;
}

bool InitDirect3DApp::IsCursorOnUIEntity(Entity* entity, HWND hWnd)
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	TransformComponent* transform = nullptr;
	MeshComponent* mesh = nullptr;
	// Recherche du TransformComponent de l'entite
	for (auto* comp : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
	{
		if (comp->ID == Mesh_ID)
		{
			mesh = static_cast<MeshComponent*>(comp);
		}
		if (comp->ID == Transform_ID)
		{
			transform = static_cast<TransformComponent*>(comp);
		}
	}

	float entityX = transform->m_transform.vPosition.x * 2;
	float entityY = transform->m_transform.vPosition.y * 2;

	float halfW = mesh->m_mesh->width/2; // Quad de 100x100 donc moitié
	float halfH = mesh->m_mesh->height/2;

	float left = entityX - halfW;
	float right = entityX + halfW;
	float top = entityY - halfH;
	float bottom = entityY + halfH;

	//char buf[256];
	//sprintf_s(buf, "Cursor: (%d,%d) | Entity: left=%.1f, right=%.1f, top=%.1f, bottom=%.1f\n",
	//	cursorPos.x, cursorPos.y, left, right, top, bottom);
	//OutputDebugStringA(buf);

	return (cursorPos.x >= left && cursorPos.x <= right &&
		cursorPos.y >= top && cursorPos.y <= bottom);
}
void InitDirect3DApp::CreateButtonUI()
{
	// Pause Menu UI
	Entity* entityButton = mp_entityManager->CreateEntity();

	mp_entityManager->AddComponent<UIComponent>(entityButton);
	mp_entityManager->AddComponent<TransformComponent>(entityButton);
	mp_entityManager->AddComponent<MeshComponent>(entityButton);

	entityButton->m_tag = Pause_TAG;
	//entityButton->m_tag = ButtonPause_TAG;

	for (auto& component : mp_entityManager->GetComponentToAddTab()[entityButton->tab_index]->vec_components)
	{
		if (component->ID == UI_ID)
		{
			UIComponent* ui = static_cast<UIComponent*>(component);
			ui->isButton = true;
		}
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = GetFactory()->CreateQuadUI(300,100);
			mesh->m_textureID = L"ReturnBtnTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.vPosition = { (float)GetScreenWidth() / 4, (float)GetScreenHeight() / 4, 0.0f };  // Centre sur (400,300) en divisant par 4 non par 2
			transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
		}
	}
}
void InitDirect3DApp::CreatePauseMenu() 
{
	// Pause Menu UI
	Entity* entityPauseScreen = mp_entityManager->CreateEntity();

	mp_entityManager->AddComponent<UIComponent>(entityPauseScreen);
	mp_entityManager->AddComponent<TransformComponent>(entityPauseScreen);
	mp_entityManager->AddComponent<MeshComponent>(entityPauseScreen);
	
	entityPauseScreen->m_tag = Pause_TAG;

	for (auto& component : mp_entityManager->GetComponentToAddTab()[entityPauseScreen->tab_index]->vec_components)
	{
		if (component->ID == Mesh_ID)
		{
			MeshComponent* mesh = static_cast<MeshComponent*>(component);
			mesh->m_mesh = GetFactory()->CreateQuadUI(450,400);
			mesh->m_textureID = L"PauseTexture";
		}
		if (component->ID == Transform_ID)
		{
			TransformComponent* transform = static_cast<TransformComponent*>(component);
			transform->m_transform.vPosition = {(float)GetScreenWidth()/4, (float)GetScreenHeight()/4, 0.0f};  // Centre sur (400/2,300/2) en divisant par 4 non par 2
			transform->m_transform.vScale = { 1.0f, 1.0f, 1.0f };
		}
	}

	CreateButtonUI();

	m_pauseScreenCreated = true;
}
void InitDirect3DApp::DestroyPauseMenu()
{
	// Pause Menu UI
	for (auto* entity : mp_entityManager->GetEntityTab())
	{
		// Check si l'entity dans la table est null
		if (entity == nullptr)
		{
			continue;
		}
		if (entity->m_tag == Pause_TAG)
		{
			mp_entityManager->ToDestroy(entity);
		}
	}

	m_pauseScreenCreated = false;
}


void InitDirect3DApp::Update()
{
	if (m_gameIsPaused == true)
	{
		ShowCursor(true);
		if (!m_pauseScreenCreated) 
		{
			CreatePauseMenu();
			UpdatePhysics();
			ShowCursor(true);
		}
		if (InputManager::GetKeyDown(VK_ESCAPE))
		{
			m_gameIsPaused = false;
			UpdateTimer();
			//ShowCursor(FALSE);
			InputManager::SetCursorLockedAndInvisible(true);
			InputManager::SetCursorOnTheCenterOfTheScreen(GetActiveWindow());

			ShowCursor(false);
		}
		for (auto* entity : mp_entityManager->GetEntityTab())
		{
			if (entity && mp_entityManager->HasComponent(entity, COMPONENT_UI))
			{
				TransformComponent* transform = nullptr;
				MeshComponent* mesh = nullptr;
				UIComponent* ui = nullptr;
				// Recherche du TransformComponent de l'entite
				for (auto* comp : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
				{
					if (comp->ID == Mesh_ID)
					{
						mesh = static_cast<MeshComponent*>(comp);
					}
					if (comp->ID == Transform_ID)
					{
						transform = static_cast<TransformComponent*>(comp);
					}
					if (comp->ID == UI_ID)
					{
						ui = static_cast<UIComponent*>(comp);
					}
				}
				if (ui && ui->isButton) 
				{
					if (transform && IsCursorOnUIEntity(entity, GetActiveWindow()) && InputManager::GetKeyDown(VK_LBUTTON))
					{
						OutputDebugStringA(" Click on BTN ");
						m_gameIsPaused = false;
						InputManager::SetCursorLockedAndInvisible(true);
						InputManager::SetCursorOnTheCenterOfTheScreen(GetActiveWindow());
					}
					if (transform && IsCursorOnUIEntity(entity, GetActiveWindow()) && !m_mouseIsOnButton)
					{
						OutputDebugStringA("Ayeee");
						transform->m_transform.vPosition = { transform->m_transform.vPosition.x + 5,transform->m_transform.vPosition.y,0.0f };
						m_mouseIsOnButton = true;
					}
					if (transform && !IsCursorOnUIEntity(entity, GetActiveWindow()) && m_mouseIsOnButton)
					{
						transform->m_transform.vPosition = { transform->m_transform.vPosition.x - 5,transform->m_transform.vPosition.y,0.0f };
						m_mouseIsOnButton = false;
					}
				}
			}
		}

	}
	else
	{
		//SetCursor(LoadCursor(NULL, IDC_CROSS));
		ShowCursor(false);
		DestroyPauseMenu();
		if (InputManager::GetKeyDown(VK_ESCAPE))
		{
			m_gameIsPaused = true;
			// ShowCursor(TRUE);
			InputManager::SetCursorLockedAndInvisible(false);
		}

		// UPDATE DU JEU
		UpdateTimer();
		UpdatePhysics();
	}
}

void InitDirect3DApp::UpdateTimer()
{
	DWORD currentTime = timeGetTime();
	m_deltaTime = (currentTime - m_lastTime) / 1000.0f; // conversion en secondes
	m_lastTime = currentTime;
}

void InitDirect3DApp::UpdatePhysics()
{
	// UPDATE SCENE
	if (mp_entityManager->GetNbEntity() > 0 && mp_entityManager->GetEntityTab()[0] != nullptr)
	{
		// Update (En gros gestion des input)
		mp_scene->OnUpdate();

		// Ennemies
		mp_ennemyManager->Update();

		// MovementSystem
		mp_movementManager->Update(m_deltaTime);

		// CollisionsSystem
		mp_colliderManager->Update();

		// cameraSystem
		mp_cameraManager->Update();

		// attacksystem
		mp_attackSystem->Update(m_deltaTime);

		// HealthSystem
		mp_healthSystem->Update(m_deltaTime);

		// LightSystem
		mp_lightSystem->Update(m_deltaTime);

		// DoorSystem
		mp_doorSystem->Update(m_deltaTime);

		// SunOrbitSystem
		mp_sunOrbitSystem->Update(m_deltaTime);

		// LifeTimeSystem
		mp_lifeTimeManager->Update(m_deltaTime);
	}

	// DESTROY ENTITIES
	for (auto& entityToDestroy : mp_entityManager->GetToDestroyTab())
	{
		mp_entityManager->DestroyEntity(&entityToDestroy);
	}
	mp_entityManager->GetToDestroyTab().clear();

	// ADD ENTITIES
	for (auto& entityToAdd : mp_entityManager->GetEntityToAddTab())
	{
		mp_entityManager->AddEntityToTab(entityToAdd, mp_entityManager->GetComponentToAddTab()[entityToAdd->tab_index]);
	}
	mp_entityManager->GetEntityToAddTab().clear();
	mp_entityManager->GetComponentToAddTab().clear();
	mp_entityManager->ResetEntitiesToAdd();
}


void InitDirect3DApp::Render()
{
	// Si il ya des entitees
	if (mp_entityManager->GetNbEntity() > 0)
	{
		// Configure le root signature et le pipeline
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		m_commandList->SetPipelineState(m_pipelineState.Get());

		// On doit lier le heap de textures (celui du TextureManager)
		ID3D12DescriptorHeap* descriptorHeaps[] = { mp_textureManager->GetSrvHeap() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		PassConstants passConstants = mp_lightSystem->GetPassConstants();
		memcpy(mp_mappedPassCB, &passConstants, sizeof(PassConstants));
		m_commandList->SetGraphicsRootConstantBufferView(2, m_passConstantBuffer->GetGPUVirtualAddress());

		DirectX::XMMATRIX view = mp_cameraManager->GetViewMatrix();
		DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 1.0f, 1.0f, 500000.0f);

		// Mes a jour le constant buffer et dessiner chaque cube
		for (auto* entity : mp_entityManager->GetEntityTab())
		{
			// Check si l'entity dans la table est null
			if (entity == nullptr || mp_entityManager->HasComponent(entity, COMPONENT_UI))
			{
				continue;
			}
			MeshComponent* entityMesh = nullptr;
			TransformComponent* entityTransform = nullptr;
			UIComponent* entityUI = nullptr;

			for (auto& component : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
			{
				if (!entityUI && component->ID == UI_ID)
				{
					entityUI = static_cast<UIComponent*>(component);
				}
				if (!entityMesh && component->ID == Mesh_ID)
				{
					entityMesh = static_cast<MeshComponent*>(component);
				}
				if (!entityTransform && component->ID == Transform_ID)
				{
					entityTransform = static_cast<TransformComponent*>(component);
				}
				if (entityMesh && entityTransform)
				{
					break; // On arrive la boucle des qu'on a trouve les deux composants
				}

			}
			if (!entityMesh || !entityTransform || !entityMesh->m_drawable)
			{
				continue;
			}

			// Calculer la matrice World-View-Projection
			DirectX::XMMATRIX world = XMLoadFloat4x4(&entityTransform->m_transform.GetMatrix());
			DirectX::XMMATRIX wvp = world * view * proj;
			TransformConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.WorldViewProj, DirectX::XMMatrixTranspose(wvp));

			objConstants.highlightActive = 0.0f;
			objConstants.highlightIntensity = 5.0f;

			TexturePropertiesComponent* texProps = nullptr;
			for (auto* comp : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
			{
				if (comp->ID == TextureProperties_ID)
				{
					texProps = static_cast<TexturePropertiesComponent*>(comp);
					break;
				}
			}
			if (texProps)
			{
				objConstants.tilingFactor = texProps->tilingFactor;
			}
			else
			{
				// Valeurs par d�faut en l'absence du composant
				objConstants.tilingFactor = 1.0f;
			}

			// Verifier si l'entite possede un composant Highlight
			for (auto& comp : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
			{
				if (comp->ID == Highlight_ID)
				{
					HighlightComponent* highlight = static_cast<HighlightComponent*>(comp);
					objConstants.highlightActive = highlight->isHighlighted ? 1.0f : 0.0f;
					objConstants.highlightIntensity = highlight->intensity;
					break;
				}
			}

			// Mise a jour du constant buffer via le mapping persistant (m_mappedData)
			memcpy(entityMesh->m_mesh->m_mappedData, &objConstants, sizeof(objConstants));

			// Attacher le constant buffer a la racine (slot 0)
			m_commandList->SetGraphicsRootConstantBufferView(0, entityMesh->m_mesh->m_constantBuffer->GetGPUVirtualAddress());
			m_commandList->IASetVertexBuffers(0, 1, &entityMesh->m_mesh->m_geometryMesh.m_vertexBufferView);
			m_commandList->IASetIndexBuffer(&entityMesh->m_mesh->m_geometryMesh.m_indexBufferView);

			if (entityMesh->m_textureID.empty())
			{
				// Si aucun ID n'est defini, utilisez par defaut "PlayerTexture"
				entityMesh->m_textureID = L"PlayerTexture";
			}
			// Recuperation du handle de la texture a utiliser selon l'identifiant contenu dans le MeshComponent
			D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = mp_textureManager->GetTextureHandle(entityMesh->m_textureID);
			// Mise a jour du slot 1 de la root signature (table de descripteurs) pour pointer sur la texture
			m_commandList->SetGraphicsRootDescriptorTable(1, textureHandle);

			// Dessiner le cube (36 indices)
			m_commandList->DrawIndexedInstanced(entityMesh->m_mesh->m_geometryMesh.m_meshIndex, 1, 0, 0, 0);
		}
	}
}

void InitDirect3DApp::RenderUI()
{
	if (mp_entityManager->GetNbEntity() > 0)
	{
		// Configure le pipeline et la root signature de l'UI
		m_commandList->SetGraphicsRootSignature(m_rootSignatureUI.Get());
		m_commandList->SetPipelineState(m_pipelineStateUI.Get());

		// Lier le heap de textures (si l'UI en utilise)
		ID3D12DescriptorHeap* descriptorHeaps[] = { mp_textureManager->GetSrvHeap() };
		m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// On parcourt toutes les entit�s et on affiche uniquement celles identifi�es comme UI
		for (auto* entity : mp_entityManager->GetEntityTab())
		{
			if (entity == nullptr)
				continue;
			if (mp_entityManager->HasComponent(entity, COMPONENT_UI))
			{

				UIComponent* entityUI = nullptr;
				MeshComponent* entityMesh = nullptr;
				TransformComponent* entityTransform = nullptr;

				for (auto& component : mp_entityManager->GetComponentsTab()[entity->tab_index]->vec_components)
				{
					if (!entityUI && component->ID == UI_ID)
						entityUI = static_cast<UIComponent*>(component);
					if (!entityMesh && component->ID == Mesh_ID)
						entityMesh = static_cast<MeshComponent*>(component);
					if (!entityTransform && component->ID == Transform_ID)
						entityTransform = static_cast<TransformComponent*>(component);
					if (entityUI && entityMesh && entityTransform)
						break;
				}
				if (!entityUI || !entityMesh || !entityTransform)
					continue;

				// Ici, au lieu d'utiliser une projection perspective,
				// utilise une matrice orthographique pour le rendu UI.
				DirectX::XMMATRIX ortho = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, GetScreenWidth(), GetScreenHeight(), 0.0f, 0.0f, 1.0f);

				//DirectX::XMMATRIX world = XMLoadFloat4x4(&entityTransform->m_transform.GetMatrix());
				DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(entityTransform->m_transform.vScale.x, entityTransform->m_transform.vScale.y, entityTransform->m_transform.vScale.z);
				DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(entityTransform->m_transform.vPosition.x, entityTransform->m_transform.vPosition.y, entityTransform->m_transform.vPosition.z);
				DirectX::XMMATRIX world = scale * translation;

				DirectX::XMMATRIX wvp = world * ortho;
				TransformConstants objConstants;
				XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
				XMStoreFloat4x4(&objConstants.WorldViewProj, DirectX::XMMatrixTranspose(wvp));

				// Pas besoin de gestion de highlight pour l'UI, sauf si tu le souhaites

				memcpy(entityMesh->m_mesh->m_mappedData, &objConstants, sizeof(objConstants));

				// Attacher le constant buffer de l'entit�E
				m_commandList->SetGraphicsRootConstantBufferView(0, entityMesh->m_mesh->m_constantBuffer->GetGPUVirtualAddress());
				m_commandList->IASetVertexBuffers(0, 1, &entityMesh->m_mesh->m_geometryMesh.m_vertexBufferView);
				m_commandList->IASetIndexBuffer(&entityMesh->m_mesh->m_geometryMesh.m_indexBufferView);

				// Lier la texture associe a l'UI (si necessaire)
				if (entityMesh->m_textureID.empty())
					entityMesh->m_textureID = L"DefaultTexture"; // ou une texture UI par d�faut
				D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = mp_textureManager->GetTextureHandle(entityMesh->m_textureID);
				m_commandList->SetGraphicsRootDescriptorTable(1, textureHandle);

				// Dessiner l'entit�EUI
				m_commandList->DrawIndexedInstanced(entityMesh->m_mesh->m_geometryMesh.m_meshIndex, 1, 0, 0, 0);
			}
		}
	}
}

void InitDirect3DApp::CreatePipelineState()
{
	// Definition d'un parametre root pour un Constant Buffer (CBV)
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
	// Parametre 0 : constant buffer (transformation)
	slotRootParameter[0].InitAsConstantBufferView(0);
	// Parametre 1 : table de descripteurs pour la texture (register t0)
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	// Parametre 2 : Pour l'eclairage (PassConstants)
	slotRootParameter[2].InitAsConstantBufferView(1);

	// Declarez le static sampler pour register(s0) 
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc(0); // register s0
	samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	// Creation de la Root Signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &error);
	if (FAILED(hr))
	{
		if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "RootSignature Error", MB_OK);
		return;
	}

	hr = m_d3DDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(hr))
	{
		MessageBox(0, L"CreateRootSignature failed!", L"Error", MB_OK);
		return;
	}

	// Load and compile shaders
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error);
	if (FAILED(hr))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"D3DCompileFromFileVertex failed with HRESULT 0x%08X.", hr);
		MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
	}

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/PixelShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error);
	if (FAILED(hr))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"D3DCompileFromFilePixel failed with HRESULT 0x%08X.", hr);
		MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
	}

	// Define the input layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Create the pipeline state object
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = m_depthStencilDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.DepthStencilState = m_depthStencilDesc;

	hr = m_d3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr))
	{
		MessageBox(0, L"CreateGraphicsPipelineState failed.", L"Error", MB_OK);
		return;
	}
}

void InitDirect3DApp::CreateUIPipelineStatev2()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange = {};
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV pour la texture
	descriptorRange.NumDescriptors = 1;
	descriptorRange.BaseShaderRegister = 0;  // Correspond �Et0
	descriptorRange.RegisterSpace = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[2]; // On passe �E2 param�tres

	// CBV 
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;  // Correspond �Eb0
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//SRV pour la texture (binding sur t0)
	//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	//rootParameters[1].Descriptor.ShaderRegister = 0;  // Correspond �Et0
	//rootParameters[1].Descriptor.RegisterSpace = 0;
	//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;  // Correspond �Es0 dans ton shader
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Visible par le Pixel Shader


	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return;
	}

	hr = m_d3DDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatureUI));

	if (FAILED(hr))
	{
		return;
	}


	// Load and compile shaders
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ComPtr<ID3DBlob> error = nullptr;

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/VertexShaderUI.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errorBlob);
	if (FAILED(hr))
	{
		//if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "D3DCompileFromFileVertexUI Error", MB_OK);
		//return;
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"D3DCompileFromFileVertex failed with HRESULT 0x%08X.", hr);
		MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
	}

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/PixelShaderUI.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errorBlob);
	if (FAILED(hr))
	{
		//if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "D3DCompileFromFilePixelUI Error", MB_OK);
		//return;
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"D3DCompileFromFilePixel failed with HRESULT 0x%08X.", hr);
		MessageBox(0, errorMsg, L"D3DCompileFromFile Error", MB_OK);
	}

	// 2. D�finition de l'Input Layout (format des sommets)
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};


	// 3. Configuration du Blend State (activer la transparence)
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 4. D�finition du Rasterizer State (pas de culling pour l'UI)
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;

	// 5. Cr�ation du PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_rootSignatureUI.Get();
	psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.StencilEnable = FALSE;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.SampleMask = UINT_MAX; // Autorise tous les samples


	hr = m_d3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateUI));
	if (FAILED(hr)) return;
}

void InitDirect3DApp::CreateUIPipelineState()
{
	// D�finition des param�tres Root Signature pour l'UI
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Param�tre 0 : CBV pour la matrice orthographique (ou autre transformation UI)
	slotRootParameter[0].InitAsConstantBufferView(0);

	// Param�tre 1 : Table de descripteurs pour la texture (register t0)
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	// D�claration du sampler statique
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc(0);
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	// Cr�ation de la Root Signature pour l'UI
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &error);
	if (FAILED(hr))
	{
		if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "UI RootSignature Error", MB_OK);
		return;
	}

	hr = m_d3DDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatureUI));
	if (FAILED(hr))
	{
		MessageBox(0, L"Create UI RootSignature failed!", L"Error", MB_OK);
		return;
	}

	// Compilation des shaders UI
	ComPtr<ID3DBlob> uiVertexShader;
	ComPtr<ID3DBlob> uiPixelShader;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/VertexShaderUI.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &uiVertexShader, &error);
	if (FAILED(hr))
	{
		if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "D3DCompileFromFileVertexUI Error", MB_OK);
		return;
	}

	hr = D3DCompileFromFile(L"../../../src/LyonPlex/PixelShaderUI.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &uiPixelShader, &error);
	if (FAILED(hr))
	{
		if (error) MessageBoxA(0, (char*)error->GetBufferPointer(), "D3DCompileFromFilePixelUI Error", MB_OK);
		return;
	}

	// D�finition de l'input layout pour l'UI
	D3D12_INPUT_ELEMENT_DESC uiInputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Configuration du pipeline state pour l'UI
	D3D12_GRAPHICS_PIPELINE_STATE_DESC uiPsoDesc = {};
	uiPsoDesc.InputLayout = { uiInputLayout, _countof(uiInputLayout) };
	uiPsoDesc.pRootSignature = m_rootSignatureUI.Get();
	uiPsoDesc.VS = { uiVertexShader->GetBufferPointer(), uiVertexShader->GetBufferSize() };
	uiPsoDesc.PS = { uiPixelShader->GetBufferPointer(), uiPixelShader->GetBufferSize() };
	uiPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// Activation du blending pour transparence UI
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	uiPsoDesc.BlendState = blendDesc;

	// D�sactivation du test de profondeur
	D3D12_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = FALSE;
	dsDesc.StencilEnable = FALSE;
	uiPsoDesc.DepthStencilState = dsDesc;

	uiPsoDesc.SampleMask = UINT_MAX;
	uiPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	uiPsoDesc.NumRenderTargets = 1;
	uiPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	uiPsoDesc.SampleDesc.Count = 1;
	uiPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	hr = m_d3DDevice->CreateGraphicsPipelineState(&uiPsoDesc, IID_PPV_ARGS(&m_pipelineStateUI));
	if (FAILED(hr))
	{
		MessageBox(0, L"CreateGraphicsPipelineState for UI failed.", L"Error", MB_OK);
		return;
	}
}

void InitDirect3DApp::Draw()
{

	// Start des frames
	m_currFrameIndex = (m_currFrameIndex + 1) % gNum_frameResources;
	m_currFrameResource = m_frameResources[m_currFrameIndex].get();

	// Start du lag meter
	DWORD t = timeGetTime();

	// Reinitialise le command allocator et la command list
	m_currFrameResource->m_cmdListAlloc->Reset();
	m_commandList->Reset(m_currFrameResource->m_cmdListAlloc.Get(), nullptr);

	// Definir le viewport et le rectangle de decoupe (scissor) pour le rendu.
	m_commandList->RSSetViewports(1, &m_screenViewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Recuperer le handle du back buffer pour le rendu.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_currBackBuffer, m_rtvDescriptorSize);

	// Recuperer le handle du Depth Stencil View.
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// Transitionner le back buffer de l'etat PRESENT a RENDER_TARGET.
	CD3DX12_RESOURCE_BARRIER barrierStart = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrierStart);

	// Effacer le Render Target avec une couleur de fond
	FLOAT clearColor[] = { 0.7f, 0.5f, 0.7f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Effacer le Depth Buffer pour reinitialiser les valeurs de profondeur (1.0 = loin).
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// Attacher le Render Target et le Depth Buffer a l'Output Merger.
	m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	// Appeler le renderer des objets --Rendu 3D--
	Render();
	RenderUI();

	// Transitionner le back buffer de RENDER_TARGET a PRESENT pour la presentation.
	CD3DX12_RESOURCE_BARRIER barrierStop = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrierStop);

	// Fermer la command list et l'executer sur la GPU.
	m_commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Soumettre les commandes au GPU
	m_swapChain->Present(0, 0);
	m_currBackBuffer = (m_currBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	m_currFrameResource->m_fence = ++m_currentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);

	// Attendre que la frame precedente soit terminee
	if (m_fence->GetCompletedValue() < m_currFrameResource->m_fence)
	{
		HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		m_fence->SetEventOnCompletion(m_currFrameResource->m_fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	DWORD dt = timeGetTime() - t;
}