#pragma once

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

	void Render(XMFLOAT3 lookat = XMFLOAT3(0,0,1));
	void GetViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
};

