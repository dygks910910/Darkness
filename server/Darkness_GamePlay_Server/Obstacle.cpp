#include "stdafx.h"
#include "Obstacle.h"



Obstacle::Obstacle()
{
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


