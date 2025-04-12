#include "pch.h"

#include "MeshFactory.h"

MeshFactory::MeshFactory()
{
}

void MeshFactory::Initialize(ID3D12Device* device, EntityManager* entityManager, WindowDX* windowDx)
{
	m_Device = device;
	mp_entityManager = entityManager;

	// Init all types of geometry used by meshs
	CreateCubeGeometry(m_cubeGeometry);
	CreateSphereGeometry(m_sphereGeometry, 20, 20); // plus le chiffre est elever plus la sphere sera lisse
	CreateSphereGeometryForSkybox(m_sphereSkyboxGeometry, 20, 20);
}

Mesh* MeshFactory::CreateCube()
{
	Mesh* newMesh = new Mesh;

	// Assign the apropriate geometry to the mesh
	newMesh->m_geometryMesh = m_cubeGeometry;

	// Creer le constant buffer pour ce cube
	CreateCubeConstantBuffer(newMesh);

	return newMesh;
}

Mesh* MeshFactory::CreateSphere()
{
	Mesh* newMesh = new Mesh;

	// Assign the apropriate geometry to the mesh
	newMesh->m_geometryMesh = m_sphereGeometry;

	// Creer le constant buffer pour ce cube
	CreateCubeConstantBuffer(newMesh);

	return newMesh;
}

Mesh* MeshFactory::CreateQuadUI(float width, float height)
{
	Mesh* newMesh = new Mesh;

	newMesh->width = width;
	newMesh->height = height;

	CreateQuadUIGeometry(newMesh->m_geometryMesh, width, height);

	CreateCubeConstantBuffer(newMesh);

	return newMesh;
}

Mesh* MeshFactory::CreateSkyBoxCube()
{
	Mesh* newMesh = new Mesh;

	// Assign the apropriate geometry to the mesh
	newMesh->m_geometryMesh = m_sphereGeometry;

	// Creer le constant buffer pour ce cube
	CreateCubeConstantBuffer(newMesh);

	return newMesh;
}

Mesh* MeshFactory::CreateSkyBoxSphere()
{
	Mesh* newMesh = new Mesh;

	// Assign the apropriate geometry to the mesh
	newMesh->m_geometryMesh = m_sphereSkyboxGeometry;

	// Creer le constant buffer comme d'habitude
	CreateCubeConstantBuffer(newMesh);

	return newMesh;
}

void MeshFactory::CreateCubeGeometry(GeometryMesh& geometry)
{
	// Nombre d'indice de la forme (36 points pour un cube)
	geometry.m_meshIndex = 36;

	// On cree ici la geometrie d'un cube unitaire (de taille 1) ; pour obtenir des cubes de tailles differentes,
	// on utilisera la transformation (scaling) dans le constant buffer.
	float halfSize = 0.5f;

	// 6 faces * 4 sommets chacune = 24 sommets
	VertexMesh vertices[24] =
	{
		// Front face (z = +halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Back face (z = -halfSize)
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Left face (x = -halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Right face (x = +halfSize)
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Top face (y = +halfSize)
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Bottom face (y = -halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },
	};

	// Indices : 6 faces * 2 triangles par face * 3 indices par triangle = 36 indices
	uint16_t indices[36] =
	{
		// Front face
		0,  1,  2,  0,  2,  3,
		// Back face
		4,  5,  6,  4,  6,  7,
		// Left face
		8,  9, 10,  8, 10, 11,
		// Right face
		12, 13, 14, 12, 14, 15,
		// Top face
		16, 17, 18, 16, 18, 19,
		// Bottom face
		20, 21, 22, 20, 22, 23
	};

	const UINT vSize = sizeof(vertices);
	const UINT iSize = sizeof(indices);

	// On utilise un heap UPLOAD pour permettre au CPU d'ecrire dans la memoire
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	// Creer le vertex buffer
	ComPtr<ID3D12Resource>& cubeVertexBuffer = geometry.m_vertexBuffer;

	HRESULT hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cubeVertexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Creer l'index buffer
	ComPtr<ID3D12Resource>& cubeIndexBuffer = geometry.m_indexBuffer;

	hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cubeIndexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees dans le vertex buffer
	void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = cubeVertexBuffer->Map(0, &readRange, &pVertexData);
	memcpy(pVertexData, vertices, vSize);
	cubeVertexBuffer->Unmap(0, nullptr);

	// Copier les donnees dans l'index buffer
	void* pIndexData = nullptr;
	hr = cubeIndexBuffer->Map(0, &readRange, &pIndexData);
	memcpy(pIndexData, indices, iSize);
	cubeIndexBuffer->Unmap(0, nullptr);

	// Creer les vues
	D3D12_VERTEX_BUFFER_VIEW& cubeBufferView = geometry.m_vertexBufferView;

	cubeBufferView.BufferLocation = cubeVertexBuffer->GetGPUVirtualAddress();
	cubeBufferView.StrideInBytes = sizeof(VertexMesh);
	cubeBufferView.SizeInBytes = vSize;

	D3D12_INDEX_BUFFER_VIEW& cubeIndexView = geometry.m_indexBufferView;

	cubeIndexView.BufferLocation = cubeIndexBuffer->GetGPUVirtualAddress();
	cubeIndexView.Format = DXGI_FORMAT_R16_UINT;
	cubeIndexView.SizeInBytes = iSize;
}

void MeshFactory::CreateSphereGeometry(GeometryMesh& geometry, int latitudeBands, int longitudeBands)
{
	// Rayon de la sphere unitaire (0.5 pour avoir un diametre = 1.0)
	float radius = 0.5f;

	// On va stocker temporairement nos sommets et indices dans des std::vector
	std::vector<VertexMesh> vertices;
	std::vector<uint16_t> indices; // ou std::vector<UINT> si vous avez plus de 65535 sommets

	vertices.reserve((latitudeBands + 1) * (longitudeBands + 1));
	indices.reserve(latitudeBands * longitudeBands * 6);

	// Generation des sommets (vertices)
	// ------------------------------------------------
	for (int lat = 0; lat <= latitudeBands; lat++)
	{
		// theta varie de 0 a PI
		float theta = (float)lat * DirectX::XM_PI / (float)latitudeBands;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int lon = 0; lon <= longitudeBands; lon++)
		{
			// phi varie de 0 a 2PI
			float phi = (float)lon * 2.0f * DirectX::XM_PI / (float)longitudeBands;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			// Position du sommet sur la sphere
			DirectX::XMFLOAT3 position(
				radius * cosPhi * sinTheta,  // X
				radius * cosTheta,           // Y
				radius * sinPhi * sinTheta   // Z
			);

			// Normale : direction depuis le centre vers la surface
			// (pour une sphere de rayon 0.5, on normalise en multipliant par 2)
			// Mais ici, sinTheta*cosPhi est deja la bonne direction unitaire
			// si on considere radius=1. Pour radius=0.5, la direction unitaire = position * (1/radius).
			DirectX::XMFLOAT3 normal(
				cosPhi * sinTheta,
				cosTheta,
				sinPhi * sinTheta
			);

			// Couleur blanche (exemple)
			DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

			// Coordonnees UV (u,v)
			// Convention : u = 1 - (lon / longitudeBands), v = 1 - (lat / latitudeBands)
			// On peut inverser selon les besoins
			DirectX::XMFLOAT2 uv(
				1.0f - (float)lon / (float)longitudeBands,
				1.0f - (float)lat / (float)latitudeBands
			);

			// Ajout dans la liste des sommets
			vertices.push_back({ position, normal, color, uv });
		}
	}

	// Generation des indices (triangles)
	// ------------------------------------------------
	// Chaque "cellule" lat-lon donne 2 triangles
	for (int lat = 0; lat < latitudeBands; lat++)
	{
		for (int lon = 0; lon < longitudeBands; lon++)
		{
			// Indices du "carre" dans la grille
			int first = (lat * (longitudeBands + 1)) + lon;
			int second = ((lat + 1) * (longitudeBands + 1)) + lon;

			// Triangle 1
			indices.push_back((uint16_t)first);
			indices.push_back((uint16_t)second);
			indices.push_back((uint16_t)(first + 1));

			// Triangle 2
			indices.push_back((uint16_t)second);
			indices.push_back((uint16_t)(second + 1));
			indices.push_back((uint16_t)(first + 1));
		}
	}

	// On enregistre le nombre total d’indices (pour DrawIndexed)
	geometry.m_meshIndex = static_cast<UINT>(indices.size());

	// Creation des ressources GPU (vertex buffer + index buffer)
	// ------------------------------------------------
	const UINT vSize = static_cast<UINT>(vertices.size() * sizeof(VertexMesh));
	const UINT iSize = static_cast<UINT>(indices.size() * sizeof(uint16_t));

	// Ressources en Upload (simplifie) - meme logique que CreateCubeGeometry
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	HRESULT hr = m_Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&vbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&geometry.m_vertexBuffer)
	);
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	hr = m_Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&ibDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&geometry.m_indexBuffer)
	);
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees CPU dans le Vertex Buffer
	void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = geometry.m_vertexBuffer->Map(0, &readRange, &pVertexData);
	memcpy(pVertexData, vertices.data(), vSize);
	geometry.m_vertexBuffer->Unmap(0, nullptr);

	// Copier les donnees CPU dans l’Index Buffer
	void* pIndexData = nullptr;
	hr = geometry.m_indexBuffer->Map(0, &readRange, &pIndexData);
	memcpy(pIndexData, indices.data(), iSize);
	geometry.m_indexBuffer->Unmap(0, nullptr);

	// Creer les vues (VertexBufferView et IndexBufferView)
	geometry.m_vertexBufferView.BufferLocation = geometry.m_vertexBuffer->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.StrideInBytes = sizeof(VertexMesh);
	geometry.m_vertexBufferView.SizeInBytes = vSize;

	geometry.m_indexBufferView.BufferLocation = geometry.m_indexBuffer->GetGPUVirtualAddress();
	geometry.m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	geometry.m_indexBufferView.SizeInBytes = iSize;
}

void MeshFactory::CreateCubeModelForSkyBox(Mesh* cube)
{
	// Nombre d'indice de la forme (36 points pour un cube)
	cube->m_geometryMesh.m_meshIndex = 36;

	// On cree ici la geometrie d'un cube unitaire (de taille 1) ; pour obtenir des cubes de tailles differentes,
	// on utilisera la transformation (scaling) dans le constant buffer.
	float halfSize = 0.5f;
	
	// 6 faces * 4 sommets chacune = 24 sommets
	VertexMesh vertices[24] =
	{
		// Front face (z = +halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Back face (z = -halfSize)
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {0.0f, 0.0f, -1.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Left face (x = -halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {-1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Right face (x = +halfSize)
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {1.0f, 0.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Top face (y = +halfSize)
		{ DirectX::XMFLOAT3(-halfSize,  halfSize,  halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize,  halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize,  halfSize, -halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize,  halfSize, -halfSize), {0.0f, 1.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },

		// Bottom face (y = -halfSize)
		{ DirectX::XMFLOAT3(-halfSize, -halfSize, -halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {0.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize, -halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {1.0f, 1.0f} },
		{ DirectX::XMFLOAT3(halfSize, -halfSize,  halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {1.0f, 0.0f} },
		{ DirectX::XMFLOAT3(-halfSize, -halfSize,  halfSize), {0.0f, -1.0f, 0.0f}, {1,1,1,1}, {0.0f, 0.0f} },
	};

	// Indices : 6 faces * 2 triangles par face * 3 indices par triangle = 36 indices
	// Inverses pour voir depuis l'interieur
	uint16_t indices[36] =
	{
		// Front face
		0,  2,  1,  0,  3,  2,
		// Back face
		4,  6,  5,  4,  7,  6,
		// Left face
		8,  10, 9,  8,  11, 10,
		// Right face
		12, 14, 13, 12, 15, 14,
		// Top face
		16, 18, 17, 16, 19, 18,
		// Bottom face
		20, 22, 21, 20, 23, 22
	};

	const UINT vSize = sizeof(vertices);
	const UINT iSize = sizeof(indices);

	// On utilise un heap UPLOAD pour permettre au CPU d'ecrire dans la memoire
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	// Creer le vertex buffer
	ComPtr<ID3D12Resource>& cubeVertexBuffer = cube->m_geometryMesh.m_vertexBuffer;

	HRESULT hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cubeVertexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Creer l'index buffer
	ComPtr<ID3D12Resource>& cubeIndexBuffer = cube->m_geometryMesh.m_indexBuffer;

	hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cubeIndexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees dans le vertex buffer
	void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = cubeVertexBuffer->Map(0, &readRange, &pVertexData);
	memcpy(pVertexData, vertices, vSize);
	cubeVertexBuffer->Unmap(0, nullptr);

	// Copier les donnees dans l'index buffer
	void* pIndexData = nullptr;
	hr = cubeIndexBuffer->Map(0, &readRange, &pIndexData);
	memcpy(pIndexData, indices, iSize);
	cubeIndexBuffer->Unmap(0, nullptr);

	// Creer les vues
	D3D12_VERTEX_BUFFER_VIEW& cubeBufferView = cube->m_geometryMesh.m_vertexBufferView;

	cubeBufferView.BufferLocation = cubeVertexBuffer->GetGPUVirtualAddress();
	cubeBufferView.StrideInBytes = sizeof(VertexMesh);
	cubeBufferView.SizeInBytes = vSize;

	D3D12_INDEX_BUFFER_VIEW& cubeIndexView = cube->m_geometryMesh.m_indexBufferView;

	cubeIndexView.BufferLocation = cubeIndexBuffer->GetGPUVirtualAddress();
	cubeIndexView.Format = DXGI_FORMAT_R16_UINT;
	cubeIndexView.SizeInBytes = iSize;
}

void MeshFactory::CreateSphereGeometryForSkybox(GeometryMesh& geometry, int latitudeBands, int longitudeBands)
{
	// Rayon de la sphere unitaire (0.5 pour avoir un diametre = 1.0)
	float radius = 0.5f;

	// On va stocker temporairement nos sommets et indices dans des std::vector
	std::vector<VertexMesh> vertices;
	std::vector<uint16_t> indices; // ou std::vector<UINT> si vous avez plus de 65535 sommets

	vertices.reserve((latitudeBands + 1) * (longitudeBands + 1));
	indices.reserve(latitudeBands * longitudeBands * 6);

	// Generation des sommets (vertices)
	// ------------------------------------------------
	for (int lat = 0; lat <= latitudeBands; lat++)
	{
		// theta varie de 0 a PI
		float theta = (float)lat * DirectX::XM_PI / (float)latitudeBands;
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int lon = 0; lon <= longitudeBands; lon++)
		{
			// phi varie de 0 a 2PI
			float phi = (float)lon * 2.0f * DirectX::XM_PI / (float)longitudeBands;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			// Position du sommet sur la sphere
			DirectX::XMFLOAT3 position(
				radius * cosPhi * sinTheta,  // X
				radius * cosTheta,           // Y
				radius * sinPhi * sinTheta   // Z
			);

			// Normale : direction depuis le centre vers la surface
			DirectX::XMFLOAT3 normal(
				cosPhi * sinTheta,
				cosTheta,
				sinPhi * sinTheta
			);

			// Couleur blanche (exemple)
			DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

			// Coordonnees UV (u,v)
			DirectX::XMFLOAT2 uv(
				1.0f - (float)lon / (float)longitudeBands,
				(float)lat / (float)latitudeBands
			);

			vertices.push_back({ position, normal, color, uv });
		}
	}

	// Generation des indices (triangles)
	// ------------------------------------------------
	// Chaque "cellule" lat-lon donne 2 triangles, on inverse l'ordre pour le skybox (vue de l'intérieur)
	for (int lat = 0; lat < latitudeBands; lat++)
	{
		for (int lon = 0; lon < longitudeBands; lon++)
		{
			int first = (lat * (longitudeBands + 1)) + lon;
			int second = ((lat + 1) * (longitudeBands + 1)) + lon;

			// Triangle 1 (ordre inversé)
			indices.push_back((uint16_t)first);
			indices.push_back((uint16_t)second);
			indices.push_back((uint16_t)(first + 1));

			// Triangle 2 (ordre inversé)
			indices.push_back((uint16_t)second);
			indices.push_back((uint16_t)(second + 1));
			indices.push_back((uint16_t)(first + 1));
		}
	}

	geometry.m_meshIndex = static_cast<UINT>(indices.size());

	const UINT vSize = static_cast<UINT>(vertices.size() * sizeof(VertexMesh));
	const UINT iSize = static_cast<UINT>(indices.size() * sizeof(uint16_t));

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	HRESULT hr = m_Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&vbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&geometry.m_vertexBuffer)
	);
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	hr = m_Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&ibDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&geometry.m_indexBuffer)
	);
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees CPU dans le Vertex Buffer
	void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = geometry.m_vertexBuffer->Map(0, &readRange, &pVertexData);
	memcpy(pVertexData, vertices.data(), vSize);
	geometry.m_vertexBuffer->Unmap(0, nullptr);

	// Copier les donnees CPU dans l’Index Buffer
	void* pIndexData = nullptr;
	hr = geometry.m_indexBuffer->Map(0, &readRange, &pIndexData);
	memcpy(pIndexData, indices.data(), iSize);
	geometry.m_indexBuffer->Unmap(0, nullptr);

	geometry.m_vertexBufferView.BufferLocation = geometry.m_vertexBuffer->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.StrideInBytes = sizeof(VertexMesh);
	geometry.m_vertexBufferView.SizeInBytes = vSize;

	geometry.m_indexBufferView.BufferLocation = geometry.m_indexBuffer->GetGPUVirtualAddress();
	geometry.m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	geometry.m_indexBufferView.SizeInBytes = iSize;
}

void MeshFactory::CreateQuadUIGeometry(GeometryMesh& geometry, float width, float height)
{
	// Un quad se dessine avec 2 triangles : 4 sommets, 6 indices
	geometry.m_meshIndex = 6; // 6 indices

	// Taille du quad (unitaire ici, que tu pourras transformer via ton constant buffer)
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	// Définition des sommets centrés (origine au centre du quad)
	VertexMesh vertices[4] =
	{
		// Position                         Normale         Couleur          Coordonnees UV
		{ DirectX::XMFLOAT3(-halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(halfWidth, -halfHeight, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-halfWidth,  halfHeight, 0.0f), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(0.0f, 1.0f) },
	};

	// Definition des indices pour dessiner 2 triangles
	uint16_t indices[6] = { 0, 1, 2, 0, 2, 3 };

	const UINT vSize = sizeof(vertices);
	const UINT iSize = sizeof(indices);

	// On utilise un heap UPLOAD pour creer les ressources GPU
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	// Creation du vertex buffer
	HRESULT hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&geometry.m_vertexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Creation de l'index buffer
	hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
		&ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&geometry.m_indexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees dans le vertex buffer
	void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = geometry.m_vertexBuffer->Map(0, &readRange, &pVertexData);
	memcpy(pVertexData, vertices, vSize);
	geometry.m_vertexBuffer->Unmap(0, nullptr);

	// Copier les donnees dans l'index buffer
	void* pIndexData = nullptr;
	hr = geometry.m_indexBuffer->Map(0, &readRange, &pIndexData);
	memcpy(pIndexData, indices, iSize);
	geometry.m_indexBuffer->Unmap(0, nullptr);

	// Creation des vues pour les buffers
	geometry.m_vertexBufferView.BufferLocation = geometry.m_vertexBuffer->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.StrideInBytes = sizeof(VertexMesh);
	geometry.m_vertexBufferView.SizeInBytes = vSize;

	geometry.m_indexBufferView.BufferLocation = geometry.m_indexBuffer->GetGPUVirtualAddress();
	geometry.m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	geometry.m_indexBufferView.SizeInBytes = iSize;
}

void MeshFactory::CreateCubeConstantBuffer(Mesh* cube)
{
	// Taille alignee du constant buffer
	const UINT alignedSize = (sizeof(TransformConstants) + 255) & ~255;
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize);

	HRESULT hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cube->m_constantBuffer));

	if (FAILED(hr))
	{
		MessageBox(0, L"Failed to create cube constant buffer!", L"Error", MB_OK);
		return;
	}

	// Mappage du buffer constant
	CD3DX12_RANGE readRange(0, 0);
	cube->m_constantBuffer->Map(0, &readRange, &cube->m_mappedData);
}