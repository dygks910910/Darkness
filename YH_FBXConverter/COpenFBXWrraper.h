#pragma once
#include"OpenFBX/src/ofbx.h"
#include<vector>
#include<string>
#include"DXUT.h"
#include"CModelClass.h"
class VertexType;
class COpenFBXWrraper
{
public:
	COpenFBXWrraper();
	COpenFBXWrraper(COpenFBXWrraper& other);
	~COpenFBXWrraper();
public:
	bool LoadFBXFile(const char* fbxPath);
	bool SaveAsOBJ(const char* path);
	bool GetVertices(std::vector<CModelClass::VertexType>& container);
	bool GetIndices(std::vector<UINT>& container);
	ofbx::IScene* GetScene() { return m_pScene; };
	std::string GetFBXInfo();
private:
	ofbx::IScene* m_pScene;
};

