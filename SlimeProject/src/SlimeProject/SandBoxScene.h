#pragma once

#include "Scene.h"

class SandBoxScene : public Scene
{
public:

	void OnInitialize() override;
	void OnUpdate() override;
	void OnClose() override;

protected:

	void CreateBoxBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health, float intensity = 1);

	void CreateIceBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void CreateMeteorBlock(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, int health, bool rotation);

	void CreateEnnemy(float posX, float posY, float posZ);

	Entity* mp_playerEntity = nullptr;
	Entity* mp_playerBody = nullptr;
};

