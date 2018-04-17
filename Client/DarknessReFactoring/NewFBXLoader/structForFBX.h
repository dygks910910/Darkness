#pragma once
#include<DirectXMath.h>
#include<vector>
struct VtxIdx;
struct Model;
struct Vertex;



inline bool cmpf(const float& a, const float& b, const float& epsilon = 0.0005f)
{
	return fabs(a - b) < epsilon;
}



struct Model
{
	std::vector<VtxIdx> models;
};
struct VtxIdx
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT4 tangent;

	bool operator==(const Vertex& oth);
};


class VertexHash
{
public:
	std::size_t operator()(const Vertex& c)const
	{
		return std::hash<float>()(c.pos.x) + std::hash<float>()(c.pos.y) + std::hash<float>()(c.pos.z);
	}
};

class VertexCompare
{
public:
	bool operator()(const Vertex&a, const Vertex& b)const;
};