#pragma once
class Obstacle
{

private:
	XMFLOAT4X4 worldMatrix;
	OrientedBox obb;
	string name;
public:
	Obstacle();
	virtual ~Obstacle();

public:
	XMFLOAT4X4& GetWorldMatrix() { return worldMatrix; }
	OrientedBox& GetObb() { return obb; }
	string& GetName() { return name; }

public:
	void InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation);
	void SetWorldMatrix(const XMFLOAT4X4& ma) { worldMatrix = ma; }
	void SetName(const string& n) { name = n; }
	
};

