#pragma once
#include<iostream>
#include<fbxsdk.h>
#include<vector>
#include "Vertex.h"
#define MAX_VERTICES 40000
class CLoadFBX
{


	
	char Name[25];

	std::vector<Vertex::Basic32> vertices;

	float *normals;
	int numNormals;

	int* indices;
	int numIndices;

public:
	CLoadFBX(char* fileName);
	~CLoadFBX();

	void ShowDetails();

	char* GetModelName();
	void  GetFbxInfo(FbxNode*);
};

