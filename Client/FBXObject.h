#pragma once
#include "StaticObject.h"
class CFBXObject :
	public CStaticObject
{
	std::string mFilename;
	std::wstring mDiffuseMapName;
	std::wstring mNormalMapName;

#if defined(DEBUG) || defined(_DEBUG)  
	CCordinate mCoord;
#endif
public:
	void SetNormalMapName(const std::wstring& filename) { mNormalMapName = filename; bHasNormalMap = true; }
	void SetFilename(const char* filename) { mFilename = filename;}
	void SetDiffuseFileName(const std::wstring& filename) { mDiffuseMapName = filename; }

	virtual void SetWorld(const XMFLOAT4X4& worldmtx);
	virtual void Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr);
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam);
	virtual void DrawToShadowMap(ID3D11DeviceContext* dc, Camera mCam);
	CFBXObject();
	~CFBXObject();
};

