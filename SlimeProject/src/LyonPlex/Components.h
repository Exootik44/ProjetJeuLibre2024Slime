#pragma once

#include "Transform.h"
#include "MeshFactory.h"

class Entity;

using namespace Microsoft::WRL;

using ComponentMask = uint32_t;

enum ComponentType {
	COMPONENT_NONE = 0,
	COMPONENT_CAMERA = 1 << 0,
	COMPONENT_MESH = 1 << 1,
	COMPONENT_TRANSFORM = 1 << 2,
	COMPONENT_VELOCITY = 1 << 3,
	COMPONENT_HEALTH = 1 << 4,
	COMPONENT_ATTACK = 1 << 5,
	COMPONENT_COLLIDER = 1 << 6,
	COMPONENT_PARTICLE = 1 << 7,
	COMPONENT_ENNEMY = 1 << 8,
	COMPONENT_PLAYER = 1 << 9,
	COMPONENT_LIGHT = 1 << 10,
	COMPONENT_LIFETIME = 1 << 11,
	COMPONENT_HIGHLIGHT = 1 << 12,
	COMPONENT_SCENEOBJECT = 1 << 13,
	COMPONENT_UI = 1 << 14,
	COMPONENT_TEXTUREPROPERTIES = 1 << 15,
	COMPONENT_GRAVITY = 1 << 16,
	COMPONENT_BUTTON = 1 << 17,
	COMPONENT_STATEMACHINE = 1 << 18,
	COMPONENT_DOOR = 1 << 19,
	COMPONENT_SUNORBIT = 1 << 20,
	COMPONENT_PNJ = 1 << 21,
};

enum ComponentID
{
	Camera_ID,
	Mesh_ID,
	Transform_ID,

	Velocity_ID,
	Health_ID,
	Attack_ID,
	Collider_ID,
	Particle_ID,
	Ennemy_ID,
	Player_ID,
	Light_ID,
	LifeTime_ID,
	Highlight_ID,
	SceneObject_ID,
	UI_ID,
	TextureProperties_ID,
	Gravity_ID,
	Button_ID,
	StateMachine_ID,
	Door_ID,
	SunOrbit_ID,
	Pnj_ID,

	TotalComponentsNumber
};

enum ObjetsTAG
{
	Player_TAG,
	Enemy_TAG,
	EnemyBoss_TAG,
	SceneObject_TAG,
	EatableObject_TAG,
	UnEatableObject_TAG,	// Retirer ?
	StickySurface_TAG,
	Projectile_TAG,
	ProjectileAttack_TAG,
	Pnj_TAG,
	UI_TAG,
	BouncySurface_TAG,
	PowerJumping_TAG,
	Skybox_TAG,
	QuestObject_TAG,
	Button_TAG,
	StateMachine_TAG,
	Door_TAG,
	Pause_TAG,
	ButtonPause_TAG,
	DeathZone_TAG,
	Altar_TAG,
	Boss_TAG,


	TotalObjectTag
};

struct GeometryMesh
{
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	// Nombre d'indices pour le rendu
	UINT m_meshIndex;
};

struct Mesh
{
	// typeOf Geometry
	GeometryMesh m_geometryMesh;

	// Buffer de constantes (matrices, couleurs, etc.)
	ComPtr<ID3D12Resource> m_constantBuffer;

	float width, height;

	void* m_mappedData = nullptr;
};

struct InventoryItem
{
	DirectX::XMFLOAT3 vScale;
	std::wstring m_textureID = L"";
	int m_index = -1;
};
struct InventoryItem2
{
	Entity* m_itemEntity;
	int m_index = -1;
};

struct Component
{
	Component(int id, ComponentMask m) : ID(id), mask(m) {}
	int ID = -1;
	ComponentMask mask = 0;
};

struct CameraComponent : public Component
{
	CameraComponent() : Component(Camera_ID, COMPONENT_CAMERA) {}
	Transform m_cameraTransform;
	DirectX::XMMATRIX m_cameraView;

	bool freeCam = false;
};

struct MeshComponent : public Component
{
	MeshComponent() : Component(Mesh_ID, COMPONENT_MESH) {}
	~MeshComponent() {
		delete m_mesh;
	}
	Mesh* m_mesh;
	std::wstring m_textureID = L"";
	bool m_drawable = true;
};

struct ColliderComponent : public Component
{
	ColliderComponent() : Component(Collider_ID, COMPONENT_COLLIDER) {}
	
	bool m_isDrawable = true;
	bool m_isColliding = false;
	bool m_isSolid = false;
	bool m_isDynamic = false;
	bool m_destructable = false;
	bool m_isDestructable = false;
	bool m_isDestroyed = false;
};

struct ParticleComponent : public Component
{
	ParticleComponent() : Component(Particle_ID, COMPONENT_PARTICLE) {}
	float m_lifeTime = 0.0f;
};

struct EnnemyComponent : public Component
{
	EnnemyComponent() : Component(Ennemy_ID, COMPONENT_ENNEMY) {}
};

struct PlayerComponent : public Component
{
	PlayerComponent() : Component(Player_ID, COMPONENT_PLAYER) {}

	std::vector<InventoryItem2> m_inventory;
	std::vector<InventoryItem2> m_throwables;/*
	std::vector<SceneObject> m_inventory2;
	std::vector<Projectile> m_throwables2;*/
	int m_inventoryCount = 0;
	int m_throwablesCount = 0;
	int m_bridgeConditionCount = 0;

	bool m_projectileHasCollided = false;
	//std::stack<InventoryItem> m_inventory;
	//std::stack<InventoryItem> m_throwables;
};

struct LifeTimeComponent : public Component
{
	LifeTimeComponent() : Component(LifeTime_ID, COMPONENT_LIFETIME) {}
	float m_lifeTime = 0.0f;
};

struct SceneObjectComponent : public Component
{
	SceneObjectComponent() : Component(SceneObject_ID, COMPONENT_SCENEOBJECT) {}
	float speedRotX = 0.f;
	float speedRotY = 0.f;
	float speedRotZ = 0.f;
};

struct UIComponent : public Component
{
	UIComponent() : Component(UI_ID, COMPONENT_UI) {}
	bool isButton = false;
};

struct PnjComponent : public Component
{
	PnjComponent() : Component(Pnj_ID, COMPONENT_PNJ) {}
	std::string m_name = "";
	std::string m_textTexture = "";
	std::string m_compleQuestTextTexture = "";
	std::vector<std::string> m_textTextureList;
	int m_actualIdText = 0;
	int m_nbCollectible = 0;
	bool m_hasQuest = false;
};

struct TransformComponent : public Component
{
	TransformComponent() : Component(Transform_ID, COMPONENT_TRANSFORM) {}
	Transform m_transform;
	bool m_isJumping = false;
	bool m_isFalling = false;
};
struct GravityComponent : public Component
{
	GravityComponent() : Component(Gravity_ID, COMPONENT_GRAVITY) {}

	//bool m_gravityApplied = true;
	float m_gravityForce = 9.81f;
	float m_weight = 1;
	float m_jumpForce = 1;
};

struct VelocityComponent : public Component
{
	VelocityComponent() : Component(Velocity_ID, COMPONENT_VELOCITY) {}
	float vz = 0.0f;
	float vx = 0.0f;
	float vy = 0.0f;
};

struct HealthComponent : public Component
{
	HealthComponent() : Component(Health_ID, COMPONENT_HEALTH) {}
	int maxHealth = 100;
	int currentHealth = maxHealth;
};

struct AttackComponent : public Component
{
	AttackComponent() : Component(Attack_ID, COMPONENT_ATTACK) {}

	int damage = 0;
	float attackCooldown = 0.f;
	float timeSinceLastAttack = 0.0f;
	bool attackRequested = false;

	float projectileSpeed = 0.f;
	float projectileSizeX = 0.f;
	float projectileSizeY = 0.f;
	float projectileSizeZ = 0.f;

	std::wstring projectileTexture = L"DefaultTexture";
};

enum class LightType {
	Point,
	Directional
};
struct LightComponent : public Component {
	LightComponent() : Component(Light_ID, COMPONENT_LIGHT) {}
	LightType type;
	// Pour la lumiere directionnelle
	DirectX::XMFLOAT3 Direction;
	// Pour la lumiere point (omnidirectionnelle)
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
	// Parametres d'attenuation pour une lumiere point
	float ConstantAtt;
	float LinearAtt;
	float QuadraticAtt;
};

struct HighlightComponent : public Component {
	HighlightComponent() : Component(Highlight_ID, COMPONENT_HIGHLIGHT) {}

	bool isHighlighted;
	float intensity;
};

struct TexturePropertiesComponent : public Component {
	TexturePropertiesComponent() : Component(TextureProperties_ID, COMPONENT_TEXTUREPROPERTIES) {}

	float tilingFactor;
};

struct ButtonComponent : public Component
{
	ButtonComponent() : Component(Button_ID, COMPONENT_BUTTON) {}
	Entity* linkedDoor = nullptr;
	DirectX::XMFLOAT3 doorMoveVector = { 0.f, 0.f, 0.f };
	bool activated;
};

struct StateMachineComponent : public Component
{
	StateMachineComponent() : Component(StateMachine_ID, COMPONENT_STATEMACHINE) {} // ou bien choisissez un ID sp�cifique et un mask unique
	
};
struct DoorComponent : public Component
{
	DoorComponent() : Component(Door_ID, COMPONENT_DOOR) {}
	bool isMoving; 
	DirectX::XMFLOAT3 targetPosition; 
	float moveSpeed;
};

struct SunOrbitComponent : public Component
{
	SunOrbitComponent() : Component(SunOrbit_ID, COMPONENT_SUNORBIT) {}

	DirectX::XMFLOAT3 orbitCenter;
	float orbitRadius;
	float angularSpeed;
	float currentAngle;
	float tiltAngle = 0;
	Entity* entityOrbitAround = nullptr;
};
