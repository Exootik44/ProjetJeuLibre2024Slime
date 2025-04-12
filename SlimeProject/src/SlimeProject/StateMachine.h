#pragma once

//#include <string>

#include "InitDirect3DApp.h"
#include "InputManager.h"

class Player;
class Projectile;
class Enemy;
class SceneObject;

class StateMachine;
class StateMachineObject;
class StateMachineEnemy;

class PlayerState
{
public:
	enum StateType
	{
		Idle,
		Jumping,
		Eating,
		Falling,
		ChargingJump,
		Moving,
		Sticky,
		Attacking,
		Bouncing,
		PowerJumping,
		Talking,
		Placing,

		StateCount
	};

	PlayerState();
	~PlayerState();

	void Update(float deltaTime);

	void StayIdle();
	void Jump();
	void Eat();
	void Fall();
	void ChargeJump();
	void Move();
	void Stick();
	void Talk();

	bool TransitionTo(StateType newState);

	std::string DisplayPlayerState();

	float m_jumpTime, m_jumpProgress, m_jumpForce,m_fallProgress, m_chargeProgress;
	int m_questTextNb, m_questTextNbMax, m_objectToCollect, m_objectCollected, m_bossHp;
	bool m_questIsCompleted,m_questHasStarted, m_pnjHasTalked, m_startBoss, m_bossIsVanquished;
	//StateType mStateType;

	StateMachine* m_State;

	static const int STATE_COUNT = (int)StateType::StateCount;
	int m_Transition[STATE_COUNT][STATE_COUNT] =
	{
		//Idle,Jumping,Eating,Falling,ChargingJump,Moving , Sticky , Attacking, Bouncing, PowerJumping, talking, placing
		{	0,	1,	1,	1,	1,	1,  1,  1,  1,  1,  1,  1	}, //Idle
		{	1,	1,	1,	0,	0,	0,  1,  1,  1,  1,  1,  1	}, //Jumping
		{	1,	1,	0,	1,	0,	0,  0,  0,  0,  0,  0,  0	}, //Eating
		{	1,	1,	0,	0,	0,	1,  1,  1,  1,  1,  1,  1	}, //Falling
		{	0,	1,	0,	0,	0,	0,  0,  0,  0,  0,  0,  0	}, //ChargingJump
		{	1,	1,	1,	1,	1,	0,  1,  1,  0,  0,  1,  1	}, //Moving
		{	1,	1,	0,	0,	0,	0,  0,  1,  0,  0,  0,  1	}, //Sticky
		{	1,	0,	0,	0,	0,	0,  0,  0,  0,  0,  0,  0	}, //Attacking
		{	1,	1,	1,	0,	0,	0,  1,  1,  1,  1,  0,  1	}, //Bouncing
		{	1,	1,	1,	0,	0,	0,  1,  1,  1,  1,  0,  1	}, //PowerJumping
		{	1,	1,	0,	0,	0,	1,  0,  0,	0, 	0,  0,  0	}, //Talking
		{	1,	0,	0,	0,	0,	0,  0,  0,	0, 	0,  0,  0	} //Placing

	};
	StateMachine* m_States[STATE_COUNT];


	InitDirect3DApp* mp_gameManager;
	Entity* mp_playerEntity;
	Player* mp_player;

	Entity* GetPlayerEntity() { return mp_playerEntity; };
};

class ObjectState
{
public:
	enum StateType
	{
		Placing,
		Shooting,
		Fall,

		StateCount
	};

	ObjectState();

	void Update(float deltaTime);

	void Place();
	void Shoot();

	bool TransitionTo(StateType newState);

	//std::string DisplayPlayerState();

	float m_jumpTime, m_jumpProgress, m_jumpForce, m_fallProgress, m_chargeProgress;

	bool m_toDestroy = false;

	//StateType mStateType;

	StateMachineObject* m_State;

	static const int STATE_COUNT = (int)StateType::StateCount;
	int m_Transition[STATE_COUNT][STATE_COUNT] =
	{
		//Place,shoot, fall
		{	0,	1,	1	}, //Place
		{	1,	0,	1	}, //shoot
		{	1,	0,	1	} //fall
	};
	StateMachineObject* m_States[STATE_COUNT];

	InitDirect3DApp* mp_gameManager;
	Entity* mp_objectEntity;
	Entity* mp_playerEntity;
	Projectile* mp_projectile;
	SceneObject* mp_object;
};


class EnemyState
{
public:
	enum StateType
	{
		Sleeping,
		Chasing,
		Colliding,


		StateCount
	};

	EnemyState();

	void Update(float deltaTime);

	//void Chase();
	//void Collide();

	bool TransitionTo(StateType newState);

	//std::string DisplayPlayerState();

	float m_jumpTime, m_jumpProgress, m_jumpForce, m_fallProgress, m_chargeProgress;

	//StateType mStateType;

	StateMachineEnemy* m_State;

	static const int STATE_COUNT = (int)StateType::StateCount;
	int m_Transition[STATE_COUNT][STATE_COUNT] =
	{
		//Idle, Chase, Collide
		{	0,	1,	1	}, //Idle
		{	1,	0,	1	}, //Chase
		{	1,	0,	1	}  //Collide
	};
	StateMachineEnemy* m_States[STATE_COUNT];

	InitDirect3DApp* mp_gameManager;
	Entity* mp_enemyEntity;
	Entity* mp_playerEntity;
	//Projectile* mp_projectile;
};

//STATE MACHINE Player

class StateMachine
{
public:
	virtual void Start(PlayerState& player) = 0;
	virtual void Update(float deltaTime, PlayerState& player) = 0;
	virtual PlayerState::StateType GetStateType() = 0;
};

//STATE MACHINE Object

class StateMachineObject
{
public:
	virtual void Start(ObjectState& object) = 0;
	virtual void Update(float deltaTime, ObjectState& object) = 0;
	virtual ObjectState::StateType GetStateType() = 0;
};

//STATE MACHINE Enemy

class StateMachineEnemy
{
public:
	virtual void Start(EnemyState& enemy) = 0;
	virtual void Update(float deltaTime, EnemyState& enemy) = 0;
	virtual EnemyState::StateType GetStateType() = 0;
};



//PLAYER STATE

class IdleState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Idle; }
};
class JumpingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Jumping; }
};
class EatingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Eating; }
};
class FallingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Falling; }
};
class ChargingJumpState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::ChargingJump; }
};
class MovingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Moving; }
};
class StickyState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Sticky; }
};
class AttackingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Sticky; }
};
class BouncingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Jumping; }
};
class PowerJumpingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Jumping; }
};
class TalkingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Talking; }
};
class PlacingState : public StateMachine
{
	void Start(PlayerState& player) override;
	void Update(float deltaTime, PlayerState& player) override;
	PlayerState::StateType GetStateType() override { return PlayerState::StateType::Placing; }
};



// OBJECT STATE

class PlaceState : public StateMachineObject
{
	void Start(ObjectState& object) override;
	void Update(float deltaTime, ObjectState& object) override;
	ObjectState::StateType GetStateType() override { return ObjectState::StateType::Placing; }
};
class ShootState : public StateMachineObject
{
	void Start(ObjectState& object) override;
	void Update(float deltaTime, ObjectState& object) override;
	ObjectState::StateType GetStateType() override { return ObjectState::StateType::Shooting; }
};
class FallState : public StateMachineObject
{
	void Start(ObjectState& object) override;
	void Update(float deltaTime, ObjectState& object) override;
	ObjectState::StateType GetStateType() override { return ObjectState::StateType::Fall; }
};



// ENEMY STATE

class SleepingState : public StateMachineEnemy
{
	void Start(EnemyState& enemy) override;
	void Update(float deltaTime, EnemyState& enemy) override;
	EnemyState::StateType GetStateType() override { return EnemyState::StateType::Sleeping; }
};

class ChasingState : public StateMachineEnemy
{
	void Start(EnemyState& enemy) override;
	void Update(float deltaTime, EnemyState& enemy) override;
	EnemyState::StateType GetStateType() override { return EnemyState::StateType::Chasing; }
};

class CollidingState : public StateMachineEnemy
{
	void Start(EnemyState& enemy) override;
	void Update(float deltaTime, EnemyState& enemy) override;
	EnemyState::StateType GetStateType() override { return EnemyState::StateType::Colliding; }
};

