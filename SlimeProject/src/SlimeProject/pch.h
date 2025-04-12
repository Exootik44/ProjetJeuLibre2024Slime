#pragma once

// Mettre include generaux en premier
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <memory>
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <DirectXMath.h>
#include <assert.h>
#include <algorithm>
#include <d3d11_1.h>
#include <stdint.h>

// Debug
#include <cassert> 
#include <timeapi.h>

#include "d3dx12.h"

#include "TextureLoaderDuLivre.h"
#include "resource.h"



#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DEBUG_NEW
#endif


#include "MeshFactory.h"
#include "EntityManager.h"