#pragma once
#include "Vertex.h"
#include<map>
#include<vector>

class CModel
{
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	bool bHasAnim = false;
	std::map<std::string, std::vector<Vertex::Basic32>*> mAnimData;
	int mIndexCount;
public:
	ID3D11Buffer* GetIB() { return mIB; };
	ID3D11Buffer* GetVB() { return mVB; }
	const int& GetIndexCount() { return mIndexCount; }

	const bool& HasAnim() { return bHasAnim; }

	void SetVB(ID3D11Buffer* buff) { mVB = buff; }
	void SetIB(ID3D11Buffer* buff) { mIB = buff; }
	void SetIndexCount(const int& numIndex) { mIndexCount = numIndex; }

	CModel();
	~CModel();
};

