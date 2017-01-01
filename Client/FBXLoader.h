#pragma once
#include <fbxsdk.h>
/*
2017 / 1 / 1 / 21:38
작성자:박요한(dygks910910@daum.net)
설명:FBX로더 클래스,정점데이터를 얻어와야 하는데 연구중..
*/
class CFBXLoader
{
	/* Tab character ("\t") counter */
	int m_iNumTabs = 0;
	FbxManager* lSdkManager;
	FbxIOSettings *ios;
	FbxImporter* lImporter;
	FbxScene* lScene;
public:
	CFBXLoader();
	~CFBXLoader();
	/**
	* Return a string-based representation based on the attribute type.
	*/
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pAttribute);
	void PrintNode(FbxNode* pNode);
	
	/**
	* Print the required number of tabs.
	*/
	void PrintTabs();
	void Init();
	void LoadFile(const char* lFilename);
	void PrintNode();
	
};

