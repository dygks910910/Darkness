#pragma once
#include"GeometryGenerator.h"



inline bool cmpf(const float& a, const float& b, const float& epsilon = 0.0005f)
{
	return fabs(a - b) < epsilon;
}



class VertexHash
{
public:
	std::size_t operator()(const GeometryGenerator::Vertex & c)const
	{
		return std::hash<float>()(c.Position.x) + std::hash<float>()(c.Position.y) + std::hash<float>()(c.Position.z);
	}
};

class VertexCompare
{
public:
	bool operator()(const GeometryGenerator::Vertex &a, const GeometryGenerator::Vertex & b)const;
};