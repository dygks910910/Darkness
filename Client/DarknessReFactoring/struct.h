#pragma once
#include <vector>
#include "d3dUtil.h"
struct ModelInfo
{
	std::vector<XMFLOAT4X4> mInstanceWorld;
	int mIndexCount;
	int mVertexOffset;
	int mIndexOffset;
};
