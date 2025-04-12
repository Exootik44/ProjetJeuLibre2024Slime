#include "pch.h"
#include "main.h"

#include "InitDirect3DApp.h"
#include "GameScene.h"
#include "SandBoxScene.h"
#include "SceneTest.h"
#include "SceneMVP.h"
#include "SceneDemo.h"
#include "SceneDemo2.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//SandBoxScene* scene = new SandBoxScene;
	//GameScene* scene = new GameScene;
	//SceneTest* scene = new SceneTest;
	//SceneMVP* scene = new SceneMVP;
	SceneDemo* scene = new SceneDemo;
	//SceneDemo2* scene = new SceneDemo2;

	InitDirect3DApp theApp(hInstance);
	if (!theApp.Initialize(scene))
		return 0;
	theApp.Run();
	_CrtDumpMemoryLeaks();
}