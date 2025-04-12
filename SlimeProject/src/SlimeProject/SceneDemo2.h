#pragma once

#include "Scene.h"
#include "Player.h"
//#include "PrefabHelper.h"

class SceneDemo2 : public Scene
{
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnClose() override;

protected:
	void CreateBoxBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health, float intensity = 1);
	void CreateFloor(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateStickyWall(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateEnnemy(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void CreateProjectile(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void CreateSphere(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void CreateTree1(float scale, float posX, float posY, float posZ);

	Entity* mp_playerEntity = nullptr;
	Player* mp_player = nullptr;
	//PrefabHelper* prefabHelper = nullptr;

	int test = 0;
};