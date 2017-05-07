#pragma once
#include "Overlapped_Ex.h"


class Object
{
	
protected:
	bool isRender;
	Overlapped_Ex over_ex;

	XMFLOAT4X4 worldMatrix;	
	XMFLOAT3 look;

	float checkAngle;
	float realAngle;

	UINT animationState;
	AABB aabb;
	UINT id;
	UINT model;





public:
	Object();
	virtual ~Object();

public:
	void SetWorldMatrix(const XMFLOAT4X4& ma) { worldMatrix = ma; }
	void SetIsRender(const bool& b) { isRender = b; }
	void SetCheckAngle(const float& an) { checkAngle = an; }
	void SetRealAngle(const float& an) { realAngle = an; }
	void SetLook(const XMFLOAT3& l) { look = l; }
	void SetAABB(const AABB& _aabb) { aabb = _aabb; }
	OVERLAPPED_EX& GetOverEx() { return over_ex; }
	void SetID(const UINT& _id) { id = _id; }
	void SetModel(const UINT& m) { model = m; }

public:
	bool& GetIsRender() { return isRender; }
	XMFLOAT4X4& GetWorldMatrix() { return worldMatrix; }
	float& GetCheckAngle() { return checkAngle; }
	float& GetRealAngle() { return realAngle; }
	XMFLOAT3& GetLook() { return look; }
	AABB& GetAABB() { return aabb; }
	UINT& GetID() { return id; }
	UINT& GetModel() { return model; }

public:
	void UpdateCenter();
	void InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation);

};

