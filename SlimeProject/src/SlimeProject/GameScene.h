#pragma once

#include "Scene.h" 

class GameScene : public Scene
{
public:

	void OnInitialize() override;

	void OnUpdate() override;

	void OnClose() override;

protected:

	void CreateDefaultBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health = 0);

	void CreateWallBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health = 0);

	Entity* mp_playerEntity = nullptr;
};

