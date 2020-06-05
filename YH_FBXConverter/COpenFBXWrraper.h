#pragma once
#include<vector>
#include<string>
#include"DXUT.h"
#include"CModelClass.h"
#include"FbxExporter/FBXExporter.h"
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
	bool GetIndices(std::vector<int>& container);
	//ofbx::IScene* GetScene() { return m_pScene; };
	FBXExporter* GetExporter() { return m_pExporter; }
	std::string GetFBXInfo();
private:
	//ofbx::IScene* m_pScene;
	bool isLoaded = false;
	FBXExporter* m_pExporter;
	
};

