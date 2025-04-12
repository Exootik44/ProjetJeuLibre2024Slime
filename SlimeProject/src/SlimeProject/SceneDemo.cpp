#include "pch.h"
#include "SceneDemo.h"

#include "EntityManager.h"
#include "InitDirect3DApp.h"
#include "InputManager.h"
#include "CameraSystem.h"
#include "AssetManager.h"
#include "PrefabHelper.h"
#include "Enemy.h"
#include "SceneObject.h"

void SceneDemo::OnInitialize()
{
	//Boss
	m_bossBattleStarted = false;
	m_bossHasSpawned = false;
	m_mapHasChanged = false;
	
	//Assets
	std::string basePath = AssetManager::GetExecutablePath();

	// Sound
	std::string beamPath = basePath + "res\\bubblebeam.wav";
	std::string beamPlayerPath = basePath + "res\\BeamPlayer.wav";
	std::string pnjTalk1Path = basePath + "res\\pnjTalk1.wav";
	std::string pnjTalk2Path = basePath + "res\\pnjTalk2.wav";
	std::string popPath = basePath + "res\\pop.wav";
	std::string doorPath = basePath + "res\\DoorOpening.wav";
	std::string jumpPath = basePath + "res\\JumpSound.wav";
	std::string jumpPath2 = basePath + "res\\JumpSound2.wav";
	std::string movePath = basePath + "res\\deplacements1.wav";
	std::string shootPath = basePath + "res\\ShootSound.wav";
	std::string eatSoundPath = basePath + "res\\EatSound.wav";
	std::string hitPath = basePath + "res\\LateralHitSound.wav";
	std::string fallImpactPath = basePath + "res\\FallimpactSound.wav";
	std::string stickyPath = basePath + "res\\StickySound.wav";
	std::string jumpFstickyPath = basePath + "res\\JumpFromStickySound.wav";

	AssetManager::AddSound("beam", beamPath);
	AssetManager::AddSound("beamPlayer", beamPlayerPath);
	AssetManager::AddSound("pnjTalk1", pnjTalk1Path);
	AssetManager::AddSound("pnjTalk2", pnjTalk2Path);
	AssetManager::AddSound("pop", popPath);
	AssetManager::AddSound("DoorOpening", doorPath);
	AssetManager::AddSound("Jump", jumpPath);
	AssetManager::AddSound("Jump2", jumpPath2);
	AssetManager::AddSound("Move1", movePath);
	AssetManager::AddSound("Shoot", shootPath);
	AssetManager::AddSound("Eat", eatSoundPath);
	AssetManager::AddSound("LateralHit", hitPath);
	AssetManager::AddSound("Fallimpact", fallImpactPath);
	AssetManager::AddSound("Sticky", stickyPath);
	AssetManager::AddSound("JumpFromSticky", jumpFstickyPath);

	prefabHelper = new PrefabHelper(mpEntityManager, mp_gameManager, this);
	
	// Musics
	std::string odysseyPath = basePath + "res\\Pixel_Odyssey.wav";
	std::string albatorPath = basePath + "res\\AlbatorOST.wav";
	std::string bossMusPath = basePath + "res\\finalShowdown.mp3";
	AssetManager::AddMusic("odyssey", odysseyPath);
	AssetManager::AddMusic("albator", albatorPath);
	AssetManager::AddMusic("boss", bossMusPath);

	AssetManager::GetMusic("odyssey").play();
	AssetManager::GetMusic("odyssey").setVolume(18);
	AssetManager::GetMusic("boss").setVolume(18);

	// prefabHelper = new PrefabHelper(mpEntityManager, mp_gameManager);
	mp_playerEntity = prefabHelper->CreatePlayer(30.f, 250.f, 310.f, 1500.f, 50);
	// Spawn Zone du boss :
	//mp_playerEntity = prefabHelper->CreatePlayer(30.f, 250.f, 510.f, 5100.f, 50);

	mp_player = prefabHelper->GetPlayer();

	std::vector<std::string> chiefGardianTalks{ "quest1Texture","quest2Texture" };
	prefabHelper->CreateDolem(40.f, 40.f, 40.f, 250.f, 320.f, 1700.f,"ChiefDolem","DolemChiefTexture", chiefGardianTalks, true, 3, "quest3Texture");

	prefabHelper->CreateSun(1000.f, 1000.f, 1000.f, 5000.f, 5000.f, 5000.f, 50.f);
	Entity* skyboxDay = prefabHelper->CreateSkyBox(560000, 560000, 560000, 0.f, 0.f, 0.f, 2.f,"SkyboxTexture");
	m_skyboxList.push_back(skyboxDay);

	//Sea
	prefabHelper->CreateWater(100000.f, 200.f, 100000.f, 0.f, -200.f, 0.f, 30.0f, "WaterTexture");

	// === Platforme de depart ===
	prefabHelper->CreateFloor2(500.f, 100.f, 1000.f, 0.f, -50.f, 0.f);

	// === Platforme1 en face ===
	prefabHelper->CreateFloor2(1000.f, 150.f, 500.f, 250.f, -25.f, 750.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 100.f, 500.f, 100.f, 750.f);

	// === Platforme1 a droite ===
	prefabHelper->CreateFloor2(500.f, 200.f, 500.f, 500.f, 0.f, 0.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 725.f, 125.f, -50.f);
	prefabHelper->CreateStickyWall(20.f, 150.f, 100.f, 755.f, 175.f, 50.f);

	// Platforme2 a droite (la grande large)
	prefabHelper->CreateFloor2(500.f, 350.f, 2000.f, 1000.f, 75.f, 650.f, 4.f, 5.f);

	// Platforme3 a droite
	prefabHelper->CreateFloor2(500.f, 250.f, 1500.f, 1500.f, 25.f, 650.f);

	std::vector<std::string> greendGardianTalks{ "greenQuestTexture" };
	prefabHelper->CreateDolem(50.f, 50.f, 50.f, 1500.f, 175.f, 650.f,"GreenGardian","DolemGreenTexture", greendGardianTalks);

	prefabHelper->CreateWall(40.f, 300.f, 730.f, 1730.f, 300.f, 265.f);
	prefabHelper->CreateWall(40.f, 300.f, 730.f, 1730.f, 300.f, 1035.f);
	prefabHelper->CreateWall(40.f, 270.f, 40.f, 1730.f, 315.f, 650.f);
	prefabHelper->CreateBoxEatable(25.f, 25.f, 25.f, 1700.f, 162.5f, 650.f);
	prefabHelper->CreateBoxEatable(15.f, 15.f, 15.f, 1640.f, 157.5f, 100.f);
	prefabHelper->CreateBoxEatable(15.f, 15.f, 15.f, 1600.f, 157.5f, 150.f);
	prefabHelper->CreateBoxEatable(15.f, 15.f, 15.f, 1500.f, 157.5f, 190.f);
	prefabHelper->CreateBoxEatable(15.f, 15.f, 15.f, 1480.f, 157.5f, 300.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1490.f, 160.f, 0.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1610.f, 160.f, 0.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1310.f, 160.f, 80.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1280.f, 160.f, 200.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1400.f, 160.f, 340.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1430.f, 160.f, 310.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1390.f, 160.f, 440.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1510.f, 160.f, 120.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1490.f, 160.f, 240.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 1340.f, 160.f, 40.f);

	// Platforme4 a droite (final platrforme)
	prefabHelper->CreateFloor2(800.f, 250.f, 350.f, 2150.f, 25.f, 650.f);
	
	//Green artifact
	prefabHelper->CreateSphereArtefact(30.f, 30.f, 30.f, 2300.f, 180.f, 650.f);

	prefabHelper->CreateTree3(1.5f, 2400.f, 150.f, 750.f);
	prefabHelper->CreateTree3(1.5f, 2400.f, 150.f, 550.f);
	prefabHelper->CreateTree3(1.5f, 2150.f, 150.f, 750.f);
	prefabHelper->CreateTree3(1.5f, 2150.f, 150.f, 550.f);
	prefabHelper->CreateTree3(1.5f, 1900.f, 150.f, 750.f);
	prefabHelper->CreateTree3(1.5f, 1900.f, 150.f, 550.f);
	prefabHelper->CreateTree1(1.f, 2500.f, 150.f, 650.f);

	prefabHelper->CreateTree2(3.f, 0.f, 0.f, -400.f);
	prefabHelper->CreateTree1(3.f, -200.f, 0.f, 400.f);
	prefabHelper->CreateTree1(2.f, 350.f, 100.f, 200.f);
	prefabHelper->CreateTree1(2.f, 1150.f, 250.f, -150.f);
	prefabHelper->CreateTree1(1.f, 1100.f, 250.f, 50.f);
	prefabHelper->CreateTree1(3.f, 850.f, 250.f, 250.f);
	prefabHelper->CreateTree2(3.f, 1050.f, 250.f, 880.f);
	prefabHelper->CreateTree1(2.f, 980.f, 250.f, 1250.f);


	// === Plaforme1 a gauche ===
	prefabHelper->CreateFloor2(800.f, 80.f, 500.f, -650.f, -60.f, 0.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, -800.f, 5.f, 225.f);
	prefabHelper->CreateTree1(2.f, -950.f, -25.f, -50.f);
	prefabHelper->CreateTree2(1.f, -550.f, -25.f, -200.f);

	// Platforme2 a gauche
	prefabHelper->CreateFloor2(300.f, 200.f, 500.f, -800.f, 0.f, 500.f);

	// Platforme3 a gauche (la grande) 
	prefabHelper->CreateFloor2(1700.f, 500.f, 500.f, -1500.f, 150.f, 1000.f);
	prefabHelper->CreateStickyWall(700.f, 200.f, 20.f, -1150.f, 250.f, 755.f);
	prefabHelper->CreateTree1(2.f, -850.f, 400.f, 900.f);
	prefabHelper->CreateTree1(3.f, -1450.f, 400.f, 1100.f);
	prefabHelper->CreateTree2(4.f, -2000.f, 400.f, 900.f);

	// Platforme4 a gauche
	prefabHelper->CreateFloor2(1000.f, 200.f, 500.f, -1850.f, 0.f, 500.f);
	prefabHelper->CreateJumpPad(20.f, 10.f, 20.f, -2000.f, 105.f, 350.f);
	prefabHelper->CreateTree1(2.f, -1900.f, 100.f, 600.f);

	// Platforme5 a gauche (la grande)
	prefabHelper->CreateFloor2(500.f, 500.f, 1000.f, -2100.f, 150.f, -250.f);
	prefabHelper->CreateTree1(1.f, -2000.f, 400.f, 100.f);
	prefabHelper->CreateTree2(2.f, -2200.f, 400.f, -100.f);
	prefabHelper->CreateTree1(4.f, -2100.f, 400.f, -400.f);
	prefabHelper->CreateTree1(1.f, -2250.f, 400.f, -600.f);

	// Platforme6 a gauche (platforme final)
	prefabHelper->CreateFloor2(500.f, 200.f, 500.f, -1600.f, 0.f, -500.f);
	prefabHelper->CreateSphereArtefact(30.f, 30.f, 30.f, -1600.f, 130.f, -500.f);
	prefabHelper->CreateTree2(2.f, -1700.f, 100.f, -700.f);
	prefabHelper->CreateTree1(2.f, -1500.f, 100.f, -400.f);

	// Platforme7,8,9 a gauche (tres grande autour de la platforme final)
	prefabHelper->CreateFloor2(500.f, 800.f, 500.f, -1600.f, 300.f, 0.f);
	prefabHelper->CreateFloor2(500.f, 800.f, 500.f, -1100.f, 300.f, -500.f);
	prefabHelper->CreateFloor2(500.f, 800.f, 700.f, -1300.f, 300.f, -1100.f);

	// Platforme10 a gauche 
	prefabHelper->CreateFloor2(800.f, 150.f, 700.f, -1950.f, -25.f, -1100.f);

	std::vector<std::string> redGardianTalks{ "redQuestTexture" };
	prefabHelper->CreateDolem(50.f, 50.f, 50.f, -1700.f, 75.f, -1200.f,"RedGardian","DolemRedTexture", redGardianTalks);

	prefabHelper->CreateJumpPad(20.f, 10.f, 20.f, -2000.f, 55.f, -1200.f);
	prefabHelper->CreateTree1(3.f, -2200.f, 50.f, -1100.f);
	prefabHelper->CreateTree2(2.f, -1750.f, 50.f, -1300.f);

	// Platforme11 a gauche (tres grande autour de la 10)
	prefabHelper->CreateFloor2(800.f, 800.f, 300.f, -1950.f, 300.f, -1600.f);

	// Platforme12 a gauche (tres grande autour de la 5 et 10)
	prefabHelper->CreateFloor2(300.f, 800.f, 2700.f, -2500.f, 300.f, -100.f, 4.f, 8.f);
	prefabHelper->CreateFloor2(600.f, 800.f, 200.f, -2050.f, 300.f, -850.f);

	// === Platforme1 derriere ===
	prefabHelper->CreateFloor2(700.f, 100.f, 500.f, 0.f, -50.f, -750.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -150.f, 16.f, -750.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -200.f, 16.f, -750.f);
	//Pnj tuto

	std::vector<std::string> blueGardianTalks{ "TextBoxTutoDoorTexture" };
	prefabHelper->CreateDolem(50.f, 50.f, 50.f, -250.f, 25.f, -680.f,"BlueGardian","DolemTexture", blueGardianTalks);

	Entity* door1 = prefabHelper->CreateDoor(300.f, 220.f, 10.f, 0.f, 110.f, -1050.f, { -290.f, 110.f, -1050.f });
	prefabHelper->CreateButton(100.f, 100.f, 10.f, -250.f, 60.f, -1000.f, door1);

	// Platforme2,3 derriere (haute bordure de porte)
	prefabHelper->CreateFloor2(200.f, 400.f, 300.f, 250.f, 100.f, -1150.f, 2.f, 2.f);
	prefabHelper->CreateFloor2(200.f, 400.f, 300.f, -250.f, 100.f, -1150.f, 2.f, 2.f);

	// Platforme4 derriere (tres haute sur la droite)
	prefabHelper->CreateFloor2(700.f, 600.f, 700.f, -700.f, 200.f, -1100.f);

	// Platforme5 derriere (passage de la porte)
	prefabHelper->CreateFloor2(300.f, 100.f, 500.f, 0.f, -50.f, -1250.f);

	// Platforme6 derriere
	prefabHelper->CreateFloor2(600.f, 50.f, 300.f, 150.f, -75.f, -1650.f);
	prefabHelper->CreateBouncySurface(50.f, 10.f, 300.f, 0.f, -5.f, -1650.f);
	prefabHelper->CreateBoxBlock(40.f, 40.f, 40.f, 430.f, -25.f, -1650.f);

	// Platforme7 derriere
	prefabHelper->CreateFloor2(800.f, 100.f, 400.f, 50.f, -50.f, -2000.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 100.f, 300.f, 50.f, -2100.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 50.f, 10.f, -2000.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -150.f, 10.f, -1850.f);

	// Platforme derriere (Haut)
	prefabHelper->CreateFloor2(800.f, 500.f, 200.f, 50.f, 150.f, -2300.f);

	// Platforme derriere (a droite)
	prefabHelper->CreateFloor2(500.f, 100.f, 400.f, -600.f, -50.f, -2000.f);
	prefabHelper->CreateTree1(1.f, -700.f, 0.f, -2000.f);
	Entity* door2 = prefabHelper->CreateDoor(10.f, 300.f, 400.f, -400.f, 150.f, -2000.f, { -400.f, 150.f, -1610.f });
	prefabHelper->CreateButton(100.f, 100.f, 10.f, -250.f, 100.f, -2195.f, door2);
	// Platformes derriere (tres haute a droite)
	prefabHelper->CreateFloor2(500.f, 600.f, 350.f, -600.f, 200.f, -1625.f);
	prefabHelper->CreateFloor2(500.f, 600.f, 200.f, -600.f, 200.f, -2300.f);
	prefabHelper->CreateFloor2(200.f, 600.f, 400.f, -950.f, 200.f, -2000.f);

	// Platforme derriere (sur la droite un peu haute)
	prefabHelper->CreateFloor2(200.f, 200.f, 500.f, -250.f, 0.f, -1550.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -250.f, 110.f, -1550.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -300.f, 110.f, -1700.f);

	// Platforme8 derriere (sur la gauche)
	prefabHelper->CreateFloor2(300.f, 100.f, 200.f, 300.f, -50.f, -1400.f);

	// Platforme9 derriere (sur la gauche)
	prefabHelper->CreateFloor2(300.f, 150.f, 700.f, 600.f, -25.f, -1650.f);
	prefabHelper->CreateTree1(2.f, 700.f, 50.f, -1350.f);

	// Platforme10 derriere (plateau sur la gauche)
	prefabHelper->CreateFloor2(500.f, 150.f, 700.f, 700.f, -25.f, -2350.f);
	prefabHelper->CreateTree2(2.f, 600.f, 50.f, -2650.f);
	prefabHelper->CreateTree1(1.f, 800.f, 50.f, -2600.f);

	// Platforme derriere (l arche)
	prefabHelper->CreateFloor2(1600.f, 200.f, 200.f, 1250.f, 300.f, -2300.f);
	prefabHelper->CreateStickyWall(1600.f, 20.f, 100.f, 1250.f, 205.f, -2300.f);
	prefabHelper->CreateFloor2(300.f, 200.f, 200.f, 300.f, 500.f, -2300.f, 2.f, 2.f);

	// Platforme11 derriere (haute coller a l arche
	prefabHelper->CreateFloor2(600.f, 650.f, 600.f, 2350.f, 225.f, -2450.f);
	prefabHelper->CreateTree1(6.f, 2250.f, 550.f, -2550.f);

	// Platformes derriere (autour de la 11)
	prefabHelper->CreateFloor2(200.f, 500.f, 400.f, 1950.f, 150.f, -2700.f, 3.f, 4.f);
	prefabHelper->CreateFloor2(300.f, 375.f, 200.f, 2200.f, 87.5f, -2850.f, 2.f, 4.f); 
	prefabHelper->CreateFloor2(300.f, 250.f, 200.f, 2500.f, 25.f, -2850.f, 2.f, 4.f); 

	// Platforme12 (apres l'arche)
	prefabHelper->CreateFloor2(300.f, 150.f, 900.f, 1900.f, -25.f, -2050.f);

	std::vector<std::string> blueGardianTalks2{ "blueQuestTexture" };
	prefabHelper->CreateDolem(50.f, 50.f, 50.f, 1900.f, 75.f, -1950.f, "BlueGardian2","DolemTexture", blueGardianTalks2);

	prefabHelper->CreateProjectile(30.f, 30.f, 30.f, 1900.f, 66.f, -1650.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1950.f, 61.f, -1650.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1850.f, 61.f, -1650.f);
	prefabHelper->CreateProjectile(30.f, 30.f, 30.f, 2000.f, 66.f, -1650.f);
	prefabHelper->CreateProjectile(30.f, 30.f, 30.f, 1800.f, 66.f, -1650.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1900.f, 61.f, -1700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1950.f, 61.f, -1700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1850.f, 61.f, -1700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 2000.f, 61.f, -1700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1800.f, 61.f, -1700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1900.f, 61.f, -1750.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1950.f, 56.f, -1750.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1850.f, 56.f, -1750.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 2000.f, 61.f, -1750.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1800.f, 61.f, -1750.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1900.f, 56.f, -1800.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1950.f, 56.f, -1800.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1850.f, 56.f, -1800.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 2000.f, 56.f, -1800.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 1800.f, 56.f, -1800.f);

	// Platforme13 (passage vers la platforme final)
	prefabHelper->CreateFloor2(600.f, 150.f, 300.f, 2350.f, -25.f, -2000.f);
	Entity* door3 = prefabHelper->CreateDoor(10.f, 300.f, 300.f, 2550.f, 200.f, -2000.f, { 2550.f, 200.f, -2295.f });
	prefabHelper->CreateButton(100.f, 100.f, 10.f, 2350.f, 150.f, -2145.f, door3);

	//Platforme (autour de la porte)
	prefabHelper->CreateFloor2(200.f, 650.f, 600.f, 2550.f, 225.f, -1550.f);
	prefabHelper->CreateTree2(4.f, 2550.f, 550.f, -1550.f);
	prefabHelper->CreateFloor2(200.f, 200.f, 300.f, 2550.f, 450.f, -2000.f);

	// Platforme derriere final
	prefabHelper->CreateFloor2(1500.f, 150.f, 1500.f, 3400.f, -25.f, -2200.f);
	// prefabHelper->CreateFloor2(30.f, 40.f, 30.f, 3500.f, 70.f, -2300.f);
	Enemy* enemy1 = prefabHelper->CreateEnnemy(100.f, 100.f, 100.f, 3500.f, 105.f, -2300.f);
	m_enemyList.push_back(enemy1);
	
	//Blue artifact
	prefabHelper->CreateSphereArtefact(30.f, 30.f, 30.f, 3700.f, 70.f, -2300.f);
	
	prefabHelper->CreateTree1(6.f, 3000.f, 50.f, -2850.f);
	prefabHelper->CreateTree1(3.f, 3500.f, 50.f, -2900.f);
	prefabHelper->CreateTree2(5.f, 4000.f, 50.f, -2900.f);
	prefabHelper->CreateTree1(3.f, 3800.f, 50.f, -2600.f);
	prefabHelper->CreateTree1(4.f, 3900.f, 50.f, -2300.f);
	prefabHelper->CreateTree2(6.f, 4000.f, 50.f, -1900.f);
	prefabHelper->CreateTree1(3.f, 3700.f, 50.f, -1600.f);
	prefabHelper->CreateTree1(2.f, 3300.f, 50.f, -1600.f);
	prefabHelper->CreateTree2(5.f, 2900.f, 50.f, -1600.f);
	prefabHelper->CreateTree1(1.f, 3000.f, 50.f, -2550.f);
	prefabHelper->CreateTree1(2.f, 3200.f, 50.f, -2550.f);
	prefabHelper->CreateTree2(2.f, 3500.f, 50.f, -2650.f);
	prefabHelper->CreateTree1(2.f, 3800.f, 50.f, -2450.f);
	prefabHelper->CreateTree1(2.f, 3800.f, 50.f, -2100.f);
	prefabHelper->CreateTree2(1.f, 3800.f, 50.f, -1850.f);
	prefabHelper->CreateTree1(1.f, 3600.f, 50.f, -1900.f);
	prefabHelper->CreateTree1(1.f, 3400.f, 50.f, -1800.f);
	prefabHelper->CreateTree2(1.f, 3100.f, 50.f, -1650.f);

	// Bridge Platform
	prefabHelper->CreateFloor2(700.f, 400.f, 1000.f, 250.f, 100.f, 1500.f);
	prefabHelper->CreateStickyWall(100.f, 250.f, 20.f, 250.f, 175.f, 1005.f);
	prefabHelper->CreateTree1(2.f, 150.f, 300.f, 1350.f);
	prefabHelper->CreateTree2(2.f, 450.f, 300.f, 1050.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, -50.f, 315.f, 1800.f);
	prefabHelper->CreateBoxEatable(20.f, 20.f, 20.f, 550.f, 315.f, 1800.f);

	Entity* bridge1 = prefabHelper->CreateBridge(300.f, 400.f, 3000.f, 250.f, -310.f, 3500.f, { 250.f, 100.f, 3500.f });
	prefabHelper->CreateFloor2(100.f, 400.f, 100.f, 550.f, 100.f, 2050.f);
	prefabHelper->CreateButton(99.f, 10.f, 99.f, 550.f, 300.f, 2050.f, nullptr);
	prefabHelper->CreateFloor2(100.f, 400.f, 100.f, -50.f, 100.f, 2050.f);
	prefabHelper->CreateButton(99.f, 10.f, 99.f, -50.f, 300.f, 2050.f, nullptr);
	prefabHelper->CreateButton(99.f, 10.f, 99.f, 250.f, 300.f, 1900.f, bridge1);

	// === Boss Platforme ===
	//center line
	prefabHelper->CreateAltar(30.f, 70.f, 30.f, 250.f, 335.f, 5170.f);

	prefabHelper->CreateFloor2(3300.f, 600.f, 500.f, 250.f, 0.f, 5100.f, 5.f, 8.f); //front line
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 100.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 50.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 0.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, -50.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -100.f, 310.f, 5170.f);
	prefabHelper->CreateProjectile(30.f, 30.f, 30.f, -150.f, 315.f, 5170.f);

	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 400.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 450.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 500.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(10.f, 10.f, 10.f, 550.f, 305.f, 5170.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 600.f, 310.f, 5170.f);
	prefabHelper->CreateProjectile(30.f, 30.f, 30.f, 650.f, 315.f, 5170.f);
	
	prefabHelper->CreateFloor2(3300.f, 600.f, 500.f, 250.f, 0.f, 8900.f, 5.f, 8.f); //back line

	prefabHelper->CreateFloor2(500.f, 600.f, 3300.f, -1650.f, 0.f, 7000.f), 5.f, 8.f; //left line
	prefabHelper->CreateTree3(4.f, -1650.f, 300.f, 5500.f);
	prefabHelper->CreateTree3(5.f, -1650.f, 300.f, 6000.f);
	prefabHelper->CreateTree3(4.f, -1650.f, 300.f, 6500.f);
	prefabHelper->CreateTree3(5.f, -1650.f, 300.f, 7000.f);
	prefabHelper->CreateTree3(4.f, -1650.f, 300.f, 7500.f);
	prefabHelper->CreateTree3(5.f, -1650.f, 300.f, 8000.f);
	prefabHelper->CreateTree3(4.f, -1650.f, 300.f, 8500.f);
	
	prefabHelper->CreateFloor2(500.f, 600.f, 3300.f, 2150.f, 0.f, 7000.f, 5.f, 8.f); //right line
	prefabHelper->CreateTree3(4.f, 2150.f, 300.f, 5500.f);
	prefabHelper->CreateTree3(5.f, 2150.f, 300.f, 6000.f);
	prefabHelper->CreateTree3(4.f, 2150.f, 300.f, 6500.f);
	prefabHelper->CreateTree3(5.f, 2150.f, 300.f, 7000.f);
	prefabHelper->CreateTree3(4.f, 2150.f, 300.f, 7500.f);
	prefabHelper->CreateTree3(5.f, 2150.f, 300.f, 8000.f);
	prefabHelper->CreateTree3(4.f, 2150.f, 300.f, 8500.f);

	// Angles
	prefabHelper->CreateFloor2(500.f, 700.f, 500.f, -1650.f, 50.f, 5100.f); //front left
	prefabHelper->CreateTree3(7.f, -1650.f, 400.f, 5100.f);
	prefabHelper->CreateFloor2(500.f, 700.f, 500.f, 2150.f, 50.f, 5100.f); //front right
	prefabHelper->CreateTree3(7.f, 2150.f, 400.f, 5100.f);
	prefabHelper->CreateFloor2(500.f, 700.f, 500.f, -1650.f, 50.f, 8900.f); //back left
	prefabHelper->CreateTree3(7.f, -1650.f, 400.f, 8900.f);
	prefabHelper->CreateFloor2(500.f, 700.f, 500.f, 2150.f, 50.f, 8900.f); //back right
	prefabHelper->CreateTree3(7.f, 2150.f, 400.f, 8900.f);

	// Platformes au centre en croix au milieu
	prefabHelper->CreateFloor2(1500.f, 500.f, 1500.f, -650.f, -50.f, 6100.f, 5.f); // front Left
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -250.f, 197.5f, 6500.f);
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -550.f, 197.5f, 6400.f);
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -950.f, 197.5f, 6700.f);
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -300.f, 197.5f, 6000.f);
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -500.f, 197.5f, 5800.f);
	prefabHelper->CreateJumpPad(150.f, 10.f, 150.f, -900.f, 197.5f, 6150.f);
	prefabHelper->CreateFloor2(150.f, 100.f, 150.f, -850.f, 250.f, 5900.f, 2.f, 2.f);
	prefabHelper->CreateBouncySurface(500.f, 10.f, 100.f, -1175.f, 295.f, 5900.f);
	prefabHelper->CreateBouncySurface(100.f, 10.f, 500.f, -850.f, 295.f, 5575.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 5600.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 5700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 5800.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -800.f, 210.f, 6800.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -750.f, 210.f, 6800.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -700.f, 210.f, 6800.f);

	prefabHelper->CreateFloor2(1500.f, 500.f, 1500.f, 1150.f, -50.f, 6100.f, 5.f); // front right
	prefabHelper->CreateBoxBlock(150.f, 150.f, 150.f, 700.f, 275.f, 6550.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 150.f, 700.f, 250.f, 6350.f);
	prefabHelper->CreateBoxBlock(150.f, 100.f, 100.f, 900.f, 250.f, 6550.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 100.f, 950.f, 250.f, 6300.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1100.f, 225.f, 6400.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1200.f, 225.f, 6250.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 800.f, 225.f, 6100.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1000.f, 225.f, 6050.f);
	prefabHelper->CreateBoxBlock(150.f, 150.f, 150.f, 1500.f, 275.f, 6350.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 150.f, 1400.f, 250.f, 6150.f);
	prefabHelper->CreateBoxBlock(150.f, 100.f, 100.f, 1350.f, 250.f, 6300.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 100.f, 1200.f, 250.f, 6000.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1700.f, 225.f, 6400.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1650.f, 225.f, 6150.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1600.f, 225.f, 5850.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1600.f, 225.f, 6450.f);
	prefabHelper->CreateBoxBlock(150.f, 150.f, 150.f, 1400.f, 275.f, 5600.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 150.f, 1150.f, 250.f, 5500.f);
	prefabHelper->CreateBoxBlock(150.f, 100.f, 100.f, 750.f, 250.f, 5650.f);
	prefabHelper->CreateBoxBlock(100.f, 100.f, 100.f, 1250.f, 250.f, 5800.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 850.f, 225.f, 5850.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 650.f, 225.f, 6050.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 700.f, 225.f, 5450.f);
	prefabHelper->CreateBoxBlock(50.f, 50.f, 50.f, 1000.f, 225.f, 5550.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 5600.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 5700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 5800.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1300.f, 210.f, 6800.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1250.f, 210.f, 6800.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1200.f, 210.f, 6800.f);

	prefabHelper->CreateFloor2(1500.f, 500.f, 1500.f, -650.f, -50.f, 7900.f, 5.f); //back left
	prefabHelper->CreateTree1(3, -350.f, 200.f, 7400.f);
	prefabHelper->CreateTree1(2, -250.f, 200.f, 7700.f); 
	prefabHelper->CreateTree1(2, -750.f, 200.f, 7300.f); 
	prefabHelper->CreateTree1(1.5f, -350.f, 200.f, 7950.f);
	prefabHelper->CreateTree1(1.5f, -1000.f, 200.f, 7450.f);	
	prefabHelper->CreateTree1(1.5f, -500.f, 200.f, 7850.f);
	prefabHelper->CreateTree1(1.5f, -600.f, 200.f, 7600.f);
	prefabHelper->CreateTree1(1.5f, -900.f, 200.f, 7600.f);

	prefabHelper->CreateTree1(3, -950.f, 200.f, 8300.f);
	prefabHelper->CreateTree1(2, -450.f, 200.f, 8200.f);
	prefabHelper->CreateTree1(2, -1250.f, 200.f, 7700.f);
	prefabHelper->CreateTree1(1.5f, -1100.f, 200.f, 7900.f);
	prefabHelper->CreateTree1(1.5f, -1300.f, 200.f, 8100.f);
	prefabHelper->CreateTree1(1.5f, -900.f, 200.f, 7950.f);
	prefabHelper->CreateTree1(1.5f, -600.f, 200.f, 7600.f);
	prefabHelper->CreateTree1(1.5f, -900.f, 200.f, 7600.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 7600.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 7700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -50.f, 210.f, 7800.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -800.f, 210.f, 7200.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -750.f, 210.f, 7200.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, -700.f, 210.f, 7200.f);

	prefabHelper->CreateBouncySurface(50.f, 10.f, 300.f, -700.f, 195.f, 7000.f);
	prefabHelper->CreateBouncySurface(50.f, 10.f, 300.f, 1200.f, 195.f, 7000.f);

	prefabHelper->CreateFloor2(1500.f, 500.f, 1500.f, 1150.f, -50.f, 7900.f, 5.f); //back right
	prefabHelper->CreateJumpPad(100.f, 10.f, 100.f, 1150.f, 197.5f, 8050.f);
	prefabHelper->CreateFloor2(150.f, 500.f, 150.f, 1450.f, 450.f, 8200.f);
	prefabHelper->CreateStickyWall(100.f, 450.f, 20.f, 1450.f, 450.f, 8130.f);
	prefabHelper->CreateStickyWall(100.f, 450.f, 20.f, 1450.f, 450.f, 8270.f);
	prefabHelper->CreateStickyWall(20.f, 450.f, 100.f, 1380.f, 450.f, 8200.f);
	prefabHelper->CreateStickyWall(20.f, 450.f, 100.f, 1520.f, 450.f, 8200.f);

	prefabHelper->CreateFloor2(150.f, 800.f, 150.f, 1100.f, 600.f, 8400.f);
	prefabHelper->CreateStickyWall(100.f, 500.f, 20.f, 1100.f, 700.f, 8330.f);
	prefabHelper->CreateProjectile(50.f, 50.f, 50.f, 1100.f, 1025.f, 8400.f);

	prefabHelper->CreateFloor2(150.f, 500.f, 150.f, 800.f, 450.f, 8100.f);
	prefabHelper->CreateStickyWall(100.f, 250.f, 20.f, 800.f, 550.f, 8030.f);
	prefabHelper->CreateStickyWall(20.f, 250.f, 100.f, 870.f, 550.f, 8100.f);

	prefabHelper->CreateFloor2(150.f, 700.f, 150.f, 1300.f, 500.f, 7750.f);
	prefabHelper->CreateStickyWall(100.f, 350.f, 20.f, 1300.f, 650.f, 7820.f);
	prefabHelper->CreateStickyWall(20.f, 350.f, 100.f, 1230.f, 650.f, 7750.f);

	prefabHelper->CreateFloor2(150.f, 300.f, 150.f, 750.f, 350.f, 7500.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 7600.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 7700.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 550.f, 210.f, 7800.f);

	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1300.f, 210.f, 7200.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1250.f, 210.f, 7200.f);
	prefabHelper->CreateProjectile(20.f, 20.f, 20.f, 1200.f, 210.f, 7200.f);

	prefabHelper->CreateFloor2(150.f, 300.f, 150.f, 750.f, -15000.f, 7500.f);
	prefabHelper->CreateFloor2(150.f, 300.f, 150.f, 750.f, -15000.f, 7500.f);

	//Entity* boss = prefabHelper->CreateBoss(250.f, 250.f, 250.f, 250.f, 600.f, 7000.f, 1.f);

	mp_camera->SetTPS();
}

void SceneDemo::OnUpdate()
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

	//if (InputManager::GetKeyIsPressed(MK_LBUTTON))
	//{
	//	camComponent->m_cameraTransform.Rotation(0.0f, deltaY * sensitivity, deltaX * sensitivity);
	//	camComponent->freeCam = true;
	//}
	//else
	//{
	//	camComponent->freeCam = false;
	//	// Mettre a jour la rotation de la camera en fonction du delta
	//	transform->m_transform.Rotation(0.0f, 0.0f, deltaX * sensitivity);
	//	//camComponent->m_cameraTransform.matrix = transform->m_transform.matrix;
	//}

	


	//StateMachine
	mp_player->Update(mp_gameManager->GetDeltaTime());
	for (auto& sceneObject : mp_sceneObjectsList)
	{
		sceneObject->Update(mp_gameManager->GetDeltaTime());
	}

	for (auto& enemy : m_enemyList)
	{
		enemy->Update(mp_gameManager->GetDeltaTime());
	}

	// Suppr ScenObj a suppr
	auto& sceneObjects = mp_sceneObjectsList;
	sceneObjects.erase(std::remove_if(sceneObjects.begin(), sceneObjects.end(),
		[this](SceneObject* obj) {
			if (obj->GetObjectState().m_toDestroy)
			{
				delete obj;
				IncrementeSceneObjCount(-1);
				return true;
			}
			return false;
		}), sceneObjects.end());

	/*wchar_t title[256];
	swprintf_s(title, 256, L"player state: %f", mp_gameManager->GetDeltaTime());
	SetWindowText(GetActiveWindow(), title);*/

	TransformComponent* transform2 = nullptr;

	if (transform2)
	{
		transform2->m_transform.vPosition.x = transform->m_transform.vPosition.x;
		transform2->m_transform.vPosition.y = transform->m_transform.vPosition.y - ((transform2->m_transform.vScale.y - transform->m_transform.vScale.y)) - 1;
		transform2->m_transform.vPosition.z = transform->m_transform.vPosition.z;
	}

	if (InputManager::GetKeyDown('B'))
	{
		m_bossBattleStarted = true;
	}
	if (InputManager::GetKeyDown('A'))
	{
		mp_camera->ChangeView();
	}
	if (InputManager::GetKeyIsPressed('X'))
	{
		mp_camera->SetTPS_Lock(true);
	}
	else mp_camera->SetTPS_Lock(false);

	//Boss
	if (m_bossBattleStarted || mp_player->GetPlayerState().m_startBoss)
	{
		if (!m_mapHasChanged) 
		{
			Entity* skyboxDoom = prefabHelper->CreateSkyBox(500000, 500000, 500000, 0.f, 0.f, 0.f, 2.f, "SkyboxApoTexture");
			m_skyboxList.push_back(skyboxDoom);
			skyboxSpeedRot = 0.01f;

			prefabHelper->CreateWater(100000.f, 250.f, 100000.f, 0.f, -200.f, 0.f, 30.0f,"RedBeamTexture");//WaterTexture
			m_mapHasChanged = true;
		}
		if (!m_bossHasSpawned) 
		{
			Entity* boss = prefabHelper->CreateBoss(250.f, 250.f, 250.f, 250.f, 600.f, 7000.f, 2.5f);
			//prefabHelper->CreateBossBody(700, 700, 700, 0, 0, 0, 1.f, boss);
			mp_boss = boss;

			AssetManager::GetMusic("odyssey").stop();
			AssetManager::GetMusic("boss").play();

			m_bossHasSpawned = true;
		}
		if (m_bossHasSpawned)
		{
			for (auto* component : mpEntityManager->GetComponentsTab()[mp_boss->tab_index]->vec_components)
			{
				if (component->ID == Transform_ID)
				{
					TransformComponent* transformBoss = static_cast<TransformComponent*>(component);
					//transformBoss->m_transform.LookAt(transform->m_transform.vPosition);
					transformBoss->m_transform.Rotation(0.01f, 0.01f, 0.01f);
					//followPlayer
					//transformBoss->m_transform.FollowTarget(transform->m_transform.vPosition, 2);
				}
			}
		}
	}

	//Skybox
	for (auto sky : m_skyboxList) 
	{
		for (auto* component : mpEntityManager->GetComponentsTab()[sky->tab_index]->vec_components)
		{
			if (component->ID == Transform_ID)
			{
				TransformComponent* transformBoss = static_cast<TransformComponent*>(component);
				transformBoss->m_transform.Rotation(0.00f, 0.00f, skyboxSpeedRot);
			}
		}
	}
}

void SceneDemo::OnClose()
{
	Close();
}