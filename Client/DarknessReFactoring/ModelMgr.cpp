#include "ModelMgr.h"
#include <fstream>
#include<string>
#include<vector>
#include "ModelMgr.h"
ModelMgr::ModelMgr()
{
	m_Normal_VB = nullptr;
	m_Basic_VB = nullptr;
	m_Basic_IB = nullptr;


// 	NewFBXLoader loader;
// 	GeometryGenerator::MeshData meshs;
// 	loader.LoadFBX("Darkness fbx\\Building_16.fbx", meshs, 0.1f);
// 	//BuildFBXBasic32Buffers(meshs);
// 	BuildFBXNormalBuffers(meshs);
// 	XMFLOAT4X4 tempMtx;
// 	XMStoreFloat4x4(&tempMtx, XMMatrixIdentity());
// 	a = new NormalModel();
// 
// 	ModelInfo info;
// 	info.mIndexOffset = idxOff;
// 	info.mIndexCount = idxCnt;
// 	info.mVertexOffset = vtxOff;
// 	a->SetInfo(info);
// 	a->SetTexture(txtureMgr.CreateTexture(L"Textures/bricks_albedo.png"));
// 	a->SetTextNormalSRV(txtureMgr.CreateTexture(L"Textures/bricks_normals.png"));
// 	a->AddInstance(tempMtx);
// 	XMStoreFloat4x4(&tempMtx, XMMatrixTranslation(0, 3, 0));
// 	a->AddInstance(tempMtx);

	DirectionalLight mDirLights[4];
	mDirLights[0].Ambient = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	Effects::InstancedNormalFX->SetDirLights(mDirLights);
	Effects::InstancedNormalFX->SetFogColor(Colors::Silver);
	Effects::InstancedNormalFX->SetFogRange(100);
	Effects::InstancedNormalFX->SetFogStart(0);


}


ModelMgr::~ModelMgr()
{
	ReleaseCOM(m_Normal_VB);
	ReleaseCOM(m_Basic_VB);
	ReleaseCOM(m_Basic_IB);


}

void ModelMgr::DrawAll(const Camera& cam)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (auto p = models.begin(); p != models.end(); ++p)
	{
		p->second->Draw(cam, this);
	}
	/*a->Draw(cam, this);*/
	HR(mSwapChain->Present(0, 0));

}

void ModelMgr::Update(const float & dt)
{

}

void ModelMgr::tempLoad()
{

}

void ModelMgr::BuildFBXBasic32Buffers(const GeometryGenerator::MeshData & box)
{
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	vtxOff = 0;

	// Cache the index count of each object.
	idxCnt = box.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	idxOff = 0;

	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount = idxCnt;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_Basic_VB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_Basic_IB));
}

void ModelMgr::BuildFBXNormalBuffers(const GeometryGenerator::MeshData & box)
{
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	vtxOff = 0;

	// Cache the index count of each object.
	idxCnt = box.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	idxOff = 0;

	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount = idxCnt;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			box.Vertices[i].TangentU.x,
			box.Vertices[i].TangentU.y,
			box.Vertices[i].TangentU.z,
			1.0f
		);
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_Basic_VB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_Basic_IB));
}

void ModelMgr::ReadMapData(char* fileName)
{
	GeometryGenerator::MeshData mesh;

	

	unsigned int totalBasicVertexCount = 0;
	unsigned int totalNormalVertexCount = 0;
	unsigned int totalBasicIndexCount = 0;
	unsigned int totalNormalIndexCount = 0;


	NewFBXLoader loader;
	std::ifstream in;
	in.open(fileName, std::ios::in);
	std::string ignore;
	std::string objName;

	std::vector<Vertex::Basic32> basicVtxArr;
	std::vector<Vertex::PosNormalTexTan> normalVtxArr;

	std::vector<UINT> basicIdxArr;
	std::vector<UINT> normalIdxArr;


	std::string textureFilePath = "Textures\\";
	std::string fbxFilePath = "Darkness fbx/";

	std::string objDiffuseFileName;
	std::string objNormalFileName;


	Model* model;
	ModelInfo modelInfo;



	XMFLOAT3 T;
	XMFLOAT4 R;
	XMFLOAT3 S;


	XMVECTOR tmpS;
	XMVECTOR tmpR;
	XMVECTOR tmpT;
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4X4 M;
	//정점복사용k
	UINT k = 0;
	UINT nk = 0;

	//정점복사용 vertex::posnormaltextan
	Vertex::PosNormalTexTan posNormalTexTanVtx;
	Vertex::Basic32 basicVtx;
	unsigned int preIndexCount = 0;
	int objectCount = 0;
	//fileFormat:
	//ObjectCount: 259
	in >> ignore >> objectCount;
	//	tower_round
	for (int i = 0; i < objectCount; ++i)
	{
		in >> objName;
		//	T : -32 0 18.6
		in >> ignore >> T.x >> T.y >> T.z;
		//	R : -0.7071069 0 1.056469E-08 0.7071068
		in >> ignore >> R.x >> R.y >> R.z >> R.w;
		//	S : 1 1 1
		in >> ignore >> S.x >> S.y >> S.z;
		//	Abricks_albedo.jpg
		in >> objDiffuseFileName;
		//	Abricks_normals.jpg
		in >> objNormalFileName;

		//모델이 있으면?
		if (models.end() != models.find(fbxFilePath + objName + ".fbx"))
		{
			tmpS = XMVectorSet(1, 1, 1, 1);
			tmpR = XMLoadFloat4(&R);
			tmpT = XMLoadFloat3(&T);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

			models[fbxFilePath + objName + ".fbx"]->AddInstance(M);
		}
		//모델이 없으면?
		else
		{
			loader.LoadFBX(std::string(fbxFilePath + objName + ".fbx").c_str(), mesh, S.x);
			

			if (objNormalFileName == "normalIsNull")
			{
				model = CreateBasicModel(S, R, T, textureFilePath + objDiffuseFileName,
					mesh.Indices.size(), basicVtxArr.size(), basicIdxArr.size());
				models[fbxFilePath + objName + ".fbx"] = model;

				totalBasicVertexCount += mesh.Vertices.size();

				//메쉬마다 정점복사.
				MeshToArr(basicVtxArr, basicIdxArr, mesh);

				totalBasicIndexCount += mesh.Indices.size();

			}
			else
			{
				//normal모델 생성.
				model = CreateNormalModel(S, R, T, textureFilePath + objDiffuseFileName,
					textureFilePath + objNormalFileName, mesh.Indices.size(),normalVtxArr.size(),
					normalIdxArr.size());
				//모델 삽입.
				models[fbxFilePath + objName + ".fbx"] = model;
				//정점과 인덱스 세팅.
				MeshToArr(normalVtxArr, normalIdxArr, mesh);
				totalNormalVertexCount += mesh.Vertices.size();
				totalNormalIndexCount += mesh.Indices.size();
			}
			//직전에 값들업데이트
		
			
		}


	}




	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalBasicVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &basicVtxArr[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_Basic_VB));


	D3D11_BUFFER_DESC vbdNormal;

	vbdNormal.Usage = D3D11_USAGE_IMMUTABLE;
	vbdNormal.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalNormalVertexCount;
	vbdNormal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdNormal.CPUAccessFlags = 0;
	vbdNormal.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitDataNormal;
	vinitDataNormal.pSysMem = &normalVtxArr[0];
	HR(md3dDevice->CreateBuffer(&vbdNormal, &vinitDataNormal, &m_Normal_VB));


	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalBasicIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &basicIdxArr[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_Basic_IB));

	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalNormalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	iinitData.pSysMem = &normalIdxArr[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_Normal_IB));

	std::cout << "sucessful LoadMapData" << std::endl;
}

Model* ModelMgr::CreateNormalModel(const XMFLOAT3 & s, const XMFLOAT4 & r, const XMFLOAT3 & t,
	const std::string & diffuseFileName, const std::string & normalFileName,
	const UINT& idxCount, const UINT& vtxOffset, const UINT& IdxOffset)
{
	NormalModel* norModel;
	norModel = new NormalModel;
	ModelInfo modelInfo;
	modelInfo.mIndexCount = idxCount;
	modelInfo.mVertexOffset = vtxOffset;
	modelInfo.mIndexOffset = IdxOffset;
	norModel->SetInfo(modelInfo);
	norModel->SetTexture(txtureMgr.CreateTexture(diffuseFileName));
	norModel->SetTextNormalSRV(txtureMgr.CreateTexture(normalFileName));

	XMVECTOR zero = XMVectorSet(0, 0, 0, 0);
	XMVECTOR tmpS, tmpR, tmpT;
	XMFLOAT4X4 M;
	tmpS = XMVectorSet(1, 1, 1, 1);
	tmpR = XMLoadFloat4(&r);
	tmpT = XMLoadFloat3(&t);
	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

	norModel->AddInstance(M);
	return norModel;
}

Model * ModelMgr::CreateBasicModel(const XMFLOAT3 & s, const XMFLOAT4 & r, const XMFLOAT3 & t,
	const std::string & diffuseFileName, const UINT & idxCount, const UINT & vtxOffset, const UINT & IdxOffset)
{
	Model* basicModel;
	basicModel = new BasicModel;
	ModelInfo modelInfo;
	modelInfo.mIndexCount = idxCount;
	modelInfo.mVertexOffset = vtxOffset;
	modelInfo.mIndexOffset = IdxOffset;
	basicModel->SetInfo(modelInfo);
	basicModel->SetTexture(txtureMgr.CreateTexture(diffuseFileName));

	XMVECTOR tmpS, tmpR, tmpT;
	XMFLOAT4X4 M;
	XMVECTOR zero = XMVectorSet(0, 0, 0, 0);
	tmpS = XMVectorSet(1, 1, 1, 1);
	tmpR = XMLoadFloat4(&r);
	tmpT = XMLoadFloat3(&t);
	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

	basicModel->AddInstance(M);
	return basicModel;
}


bool ModelMgr::MeshToArr(std::vector<Vertex::PosNormalTexTan>& vtxArr, std::vector<UINT>& idxArr, const GeometryGenerator::MeshData& mesh)
{
	Vertex::PosNormalTexTan posNormalTexTanVtx;
	//정점과 인덱스 세팅.
	for (size_t i = 0; i < mesh.Vertices.size(); ++i)
	{
		posNormalTexTanVtx.Pos = mesh.Vertices[i].Position;
		posNormalTexTanVtx.Normal = mesh.Vertices[i].Normal;
		posNormalTexTanVtx.Tex = mesh.Vertices[i].TexC;
		posNormalTexTanVtx.TangentU = XMFLOAT4(
			mesh.Vertices[i].TangentU.x,
			mesh.Vertices[i].TangentU.y,
			mesh.Vertices[i].TangentU.z,
			1.0f
		);
		vtxArr.push_back(posNormalTexTanVtx);
	}
	idxArr.insert(idxArr.end(), mesh.Indices.begin(), mesh.Indices.end());

	return true;
}
bool ModelMgr::MeshToArr(std::vector<Vertex::Basic32>& vtxArr, std::vector<UINT>& idxArr, const GeometryGenerator::MeshData& mesh)
{
	Vertex::Basic32 basicVtx;
	//정점과 인덱스 세팅.
	for (size_t i = 0; i < mesh.Vertices.size(); ++i)
	{
		basicVtx.Pos = mesh.Vertices[i].Position;
		basicVtx.Normal = mesh.Vertices[i].Normal;
		basicVtx.Tex = mesh.Vertices[i].TexC;
		vtxArr.push_back(basicVtx);
	}
	idxArr.insert(idxArr.end(), mesh.Indices.begin(), mesh.Indices.end());

	return true;
}
