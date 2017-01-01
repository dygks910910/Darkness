#pragma once
#include <fbxsdk.h>
/*
2017 / 1 / 1 / 21:38
�ۼ���:�ڿ���(dygks910910@daum.net)
����:FBX�δ� Ŭ����,���������͸� ���;� �ϴµ� ������..
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

