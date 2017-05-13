#include "stdafx.h"
#include "Object.h"



Object::Object()
	: isRender(false), checkAngle(0), realAngle(0), animationState(0), id(-1), speed(1.0), speedSave(2.5)
{
	over_ex.operation = OP_RECV;

	look.x = 0;
	look.y = 0;
	look.z = 1;

	bIsAlive = true;
	

}


Object::~Object()
{

}


void Object::UpdateSphereForObstacle()
{
	//sphere.Center = XMFLOAT3(worldMatrix._41 - (0.678452 / 2), worldMatrix._42 + 2.44222 , worldMatrix._43);
	sphereForObstacle.Center = XMFLOAT3(worldMatrix._41 - (0.678452 / 2), worldMatrix._42  , worldMatrix._43);
	sphereForObstacle.Radius = 0.5;
}

void Object::UpdateSphereForPlayer()
{
	sphereForPlayer.Center = XMFLOAT3(worldMatrix._41 - (0.678452 / 2), worldMatrix._42, worldMatrix._43);
	sphereForPlayer.Radius = 0.4;
}
void Object::InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation)
{
	XMVECTOR S = XMLoadFloat3(&scale);
	XMVECTOR P = XMLoadFloat3(&position);
	XMVECTOR Q = XMLoadFloat4(&rotation);
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&worldMatrix, XMMatrixAffineTransformation(S, zero, Q, P));
}