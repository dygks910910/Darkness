#pragma once
#include "Overlapped_Ex.h"

// Extents = XMFLOAT3(0.678452, 2.44222, 2.96222);

class Object
{
public:
	mutex cs;

protected:


	bool isRender;
	Overlapped_Ex over_ex;

	XMFLOAT4X4 worldMatrix;	

	XMFLOAT3 look;

	float checkAngle;
	float realAngle;

	UINT animationState;
	UINT id;
	UINT model;
	Sphere sphereForObstacle;
	Sphere sphereForPlayer;

	float speed;
	float speedSave;

	bool bIsAlive;

public:
	Object();
	virtual ~Object();

public:
	void SetWorldMatrix(const XMFLOAT4X4& ma) { worldMatrix = ma; }
	void SetIsRender(const bool& b) { isRender = b; }
	void SetCheckAngle(const float& an) { checkAngle = an; }
	void SetRealAngle(const float& an) { realAngle = an; }
	void SetLook(const XMFLOAT3& l) { look = l; }
	OVERLAPPED_EX& GetOverEx() { return over_ex; }
	void SetID(const UINT& _id) { id = _id; }
	void SetModel(const UINT& m) { model = m; }
	void SetSpeed(const float& s) { speed = s; }
	void SetSpeedSave(const float& s) { speedSave = s; }
	void SetAnimationState( const UINT& a) { animationState = a; }
	void SetIsAlive(const bool& b) { bIsAlive = b; }

public:
	bool& GetIsRender() { return isRender; }
	XMFLOAT4X4& GetWorldMatrix() { return worldMatrix; }
	float& GetCheckAngle() { return checkAngle; }
	float& GetRealAngle() { return realAngle; }
	XMFLOAT3& GetLook() { return look; }
	Sphere& GetSphereForObstacle() { return sphereForObstacle; }
	Sphere& GetSphereForPlayer() { return sphereForPlayer; }
	UINT& GetID() { return id; }
	UINT& GetModel() { return model; }
	float& GetSpeed() { return speed; }
	float& GetSpeedSave() { return speedSave; }
	bool& GetIsAlive() { return bIsAlive; }

	UINT& GetAnimationState() { return animationState; }




public:
	void InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation);

	void UpdateSphereForObstacle();
	void UpdateSphereForPlayer();

};

