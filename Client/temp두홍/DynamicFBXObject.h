#pragma once
#include "DynamicObject.h"
class CDynamicFBXObject :
	public CDynamicObject
{
	std::string mFilename;
	std::wstring mDiffuseMapName;
	std::wstring mNormalMapName;

#if defined(DEBUG) || defined(_DEBUG)  
	CCordinate mCoord;
#endif
public:
	void SetNormalMapName(const std::wstring& filename) { mNormalMapName = filename; bHasNormalMap = true; }
	void SetFilename(const char* filename) { mFilename = filename; }
	void SetDiffuseFileName(const std::wstring& filename) { mDiffuseMapName = filename; }

	void Update(ID3D11DeviceContext * dc, int num);


	virtual void SetWorld(const XMFLOAT4X4& worldmtx);
	virtual void Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr);
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam);
	virtual void DrawToShadowMap(ID3D11DeviceContext* dc, Camera mCam);
	CDynamicFBXObject();
	~CDynamicFBXObject();

	std::map<std::string, std::vector<Vertex::Basic32>*>::iterator mAnimIter;
	std::string mAnimName[6] = { "idle", "walk", "run", "attack1", "jump", "die" };
private:
	int mAnimationCount = 0;
	int mAnimTimeState;
	int mAnimLimitCount[6];
};

