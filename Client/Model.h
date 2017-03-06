#pragma once
#include "Vertex.h"
#include<map>
#include<vector>
class CModel
{
	static ID3D11Buffer* mModelBuff;
	bool bHasAnim = false;
	std::map<std::string, std::vector<Vertex::Basic32>*> mAnimData;
	std::vector<UINT> mIndeces;
	int mIndexOffset;
public:
	const int& GetIndexOffset() { return mIndexOffset; }
	std::vector<UINT>* GetIndex() { return &mIndeces; };
	ID3D11Buffer* GetModelBuff() { return mModelBuff; }

	const bool& HasAnim() { return bHasAnim; }
	void SetIndexOffset(const int& offset) { mIndexOffset = offset; };
	static void SetBuffer(ID3D11Buffer* buff) { mModelBuff = buff; }

	CModel();
	~CModel();
};

