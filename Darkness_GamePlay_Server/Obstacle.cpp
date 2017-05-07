#include "stdafx.h"
#include "Obstacle.h"



Obstacle::Obstacle()
{
	aabb.center = XMFLOAT3(0, 0, 0);
	aabb.extent = XMFLOAT3(0, 0, 0);
}


Obstacle::~Obstacle()
{
}

void Obstacle::InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation)
{
	XMVECTOR S = XMLoadFloat3(&scale);
	XMVECTOR P = XMLoadFloat3(&position);
	XMVECTOR Q = XMLoadFloat4(&rotation);
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&worldMatrix, XMMatrixAffineTransformation(S, zero, Q, P));
}

void Obstacle::UpdateCenter()
{
	XMFLOAT3 temp;
	XMVECTOR tempVector;

	aabb.center = XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43);

	temp.x = aabb.center.x;
	temp.y = aabb.center.y;
	temp.z = aabb.center.z;
	
	tempVector = XMLoadFloat3(&temp);
	XMMATRIX tempMtx = XMLoadFloat4x4(&worldMatrix);
	
	XMVECTOR trasformV = XMVector3TransformCoord(tempVector, tempMtx);
	XMStoreFloat3(&temp, trasformV);

	aabb.center = temp;
	
}
