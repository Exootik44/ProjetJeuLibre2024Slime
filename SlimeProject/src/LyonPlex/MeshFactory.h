#pragma once

#include "Transform.h"
#include "EntityManager.h"
#include "Components.h"
#include "WindowDX.h"

using namespace Microsoft::WRL;

struct VertexMesh
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT2 TexCoord;
};

struct TransformConstants
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 WorldViewProj;

	float highlightActive;
	float highlightIntensity;
	float tilingFactor;
	float pad[2];
};

class MeshFactory
{
public:
	MeshFactory();

	void Initialize(ID3D12Device* device, EntityManager* entityManager, WindowDX* windowDx);

	Mesh* CreateCube();
	Mesh* CreateSphere();
	Mesh* CreateQuadUI(float width, float height);

	Mesh* CreateSkyBoxCube();
	Mesh* CreateSkyBoxSphere();

private:
	// Type of mesh geometry
	void CreateCubeGeometry(GeometryMesh& geometry);
	void CreateSphereGeometry(GeometryMesh& geometry, int latitudeBands, int longitudeBands);
	void CreateCubeModelForSkyBox(Mesh* cube);
	void CreateSphereGeometryForSkybox(GeometryMesh& geometry, int latitudeBands, int longitudeBands);
	void CreateQuadUIGeometry(GeometryMesh& geometry, float width, float height);

	// Alloue et configure le constant buffer pour un cube
	void CreateCubeConstantBuffer(Mesh* cube);

	// App
	ComPtr<ID3D12Device> m_Device;

	EntityManager* mp_entityManager;

	GeometryMesh m_cubeGeometry, m_sphereGeometry, m_sphereSkyboxGeometry;
};