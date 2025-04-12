#pragma once
#include "Player.h"

class EntityManager;
class InitDirect3DApp;
class SceneDemo;

class PrefabHelper
{
public:
	PrefabHelper(EntityManager* entityManager, InitDirect3DApp* initDirect3DApp, SceneDemo* scene)
		: mpEntityManager(entityManager), mp_gameManager(initDirect3DApp), mp_scene(scene)
	{
	}

	Player* GetPlayer() { return mp_player; }
	Entity* CreatePlayer(float size, float posX, float posY, float posZ, int health, float jumpForce = 300.f, float speed = 200.f, float weight = 50.f, float highlightIntensity = 1.7f);
	
	void CreatePnj(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity = 1);

	Entity* CreateSun(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity);
	Entity* CreateBoss(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity);
	void CreateBossBody(float sizeX, float sizeY, float sizeZ, float intensity, float rotAngle, float rotSpeed, float rotStart, Entity* boss);
	Entity* CreateSkyBox(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity, std::string textureName);
	void CreateAltar(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity = 1);


	void CreateBoxBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health = 1, float intensity = 1);
	void CreateBoxEatable(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity = 2);

	void CreateWater(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float tilingFactor = 1.0f, std::string textureName ="");
	void CreateFloor2(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float tilingFactorGrass = 3.0f, float tilingFactorFloor = 4.0f);
	void CreateWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateDolem(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, std::string name, std::string textureName, std::vector<std::string>);
	void CreateDolem(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, std::string name, std::string textureName,std::vector<std::string> chattingText, bool hasQuest, int collectibleNB, std::string completQuestText);

	void CreateTreeTrunk(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateTreeBranch(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateGreenCube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity);
	void CreateGrass(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity, float tilingFactorGrass = 3.f);

	void CreateStickyWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	Enemy* CreateEnnemy(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void CreateProjectile(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateSphere(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateSphereArtefact(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float intensity = 4.0f);

	void CreateJumpPad(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateBouncySurface(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void CreateButton(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, Entity* doorToLink);
	Entity* CreateDoor(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, DirectX::XMFLOAT3 targetPosition);
	Entity* CreateBridge(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, DirectX::XMFLOAT3 targetPosition);

	void CreateTree1(float scale, float posX, float posY, float posZ);
	void CreateTree2(float scale, float posX, float posY, float posZ);
	void CreateTree3(float scale, float posX, float posY, float posZ);

private:
	EntityManager* mpEntityManager;
	InitDirect3DApp* mp_gameManager;
	Player* mp_player = nullptr;

	SceneDemo* mp_scene;

};