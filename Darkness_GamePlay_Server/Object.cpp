#include "stdafx.h"
#include "Object.h"



Object::Object()
	: isRender(false), checkAngle(0), realAngle(0), animationState(0), id(-1)
{
	over_ex.operation = OP_RECV;

	look.x = 0;
	look.y = 0;
	look.z = 1;

	aabb.center = XMFLOAT3(0, 0, 0);
	aabb.extent = XMFLOAT3(0, 0, 0);
}


Object::~Object()
{
}

void Object::UpdateCenter()
{
	aabb.center = XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43);
}

void Object::InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation)
{
	XMVECTOR S = XMLoadFloat3(&scale);
	XMVECTOR P = XMLoadFloat3(&position);
	XMVECTOR Q = XMLoadFloat4(&rotation);
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&worldMatrix, XMMatrixAffineTransformation(S, zero, Q, P));
}