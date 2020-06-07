#pragma once
#include <d3dx10math.h>
#include<xnamath.h>
class CCameraClass
{
public:
	CCameraClass();
	CCameraClass(const CCameraClass&);
	~CCameraClass();
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
};

