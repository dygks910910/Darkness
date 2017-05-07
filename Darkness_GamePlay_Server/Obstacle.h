#pragma once
class Obstacle
{

private:
	XMFLOAT4X4 worldMatrix;
	AABB aabb;
	string name;
public:
	Obstacle();
	virtual ~Obstacle();

public:
	XMFLOAT4X4& GetWorldMatrix() { return worldMatrix; }
	AABB& GetAABB() { return aabb; }
	string& GetName() { return name; }

public:
	void InitWorldMatrix(XMFLOAT3 scale, XMFLOAT3 position, XMFLOAT4 rotation);
	void UpdateCenter();
	void SetWorldMatrix(const XMFLOAT4X4& ma) { worldMatrix = ma; }
	void SetAABB(const AABB& _aabb) { aabb = _aabb; }
	void SetName(const string& n) { name = n; }
	
};

