#include "ModelMgr.h"
#include <fstream>
#include<string>
#include<vector>
#include "ModelMgr.h"
ModelMgr::ModelMgr()
{
	//m_NomalVB = nullptr;
	m_Basic_VB = nullptr;
	m_IB = nullptr;

	//CFbxLoader loader;
	//GeometryGenerator::MeshData tmep;
	//loader.LoadFBX("Darkness fbx\\house 1.fbx", tmep, 0.1f);
	//XMFLOAT4X4 tempMtx;
	//XMStoreFloat4x4(&tempMtx, XMMatrixIdentity());
	//
	//
	////BuildFBXBasic32Buffers(tmep);
	//BuildFBXNormalBuffers(tmep);

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
}


ModelMgr::~ModelMgr()
{
	ReleaseCOM(m_Normal_VB);
	ReleaseCOM(m_Basic_VB);
	ReleaseCOM(m_IB);

	
}

void ModelMgr::DrawAll(const Camera& cam)
{
	for (auto p = models.begin(); p != models.end(); ++p)
	{
		p->second->Draw(cam, this);
	}
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
	vtxOff= 0;

	// Cache the index count of each object.
	idxCnt = box.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	idxOff = 0;

	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount =idxCnt;

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
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_IB));
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
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_IB));
}

void ModelMgr::ReadMapData(char* fileName)
{
	CFbxLoader loader;
	GeometryGenerator::MeshData mesh;

	int preIdxCnt=0;
	int preIdxOffset = 0;
	int preVtxOffset = 0;
	int preVtxCnt = 0;

	int totalBasicVertexCount=0;

	int totalNormalVertexCount=0;
	int totalIndexCount=0;

	std::ifstream in;
	in.open(fileName, std::ios::in);
	std::string ignore;
	std::string objName;

	std::vector<Vertex::Basic32> basicVtxArr;
	std::vector<Vertex::PosNormalTexTan> normalVtxArr;
	std::vector<UINT> idxArr;

	std::string textureFilePath = "Textures\\";
	std::string fbxFilePath = "Darkness fbx\\";

	std::string objDiffuseFileName;
	std::string objNormalFileName;


	NormalModel* norModel;
	BasicModel* basicModel;
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
		if (models.end() != models.find(objName + ".fbx"))
		{
			//모델이 있으면?
			tmpS = XMVectorSet(1, 1, 1, 1);
			tmpR = XMLoadFloat4(&R);
			tmpT = XMLoadFloat3(&T);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

			models[objName + ".fbx"]->AddInstance(M);
		}
		else
		{
			loader.LoadFBX(std::string(objName + ".fbx").c_str(), mesh, S.x);
			//모델이 없으면?
			if (objNormalFileName == "normalIsNull")
			{

				basicModel = new BasicModel;
				modelInfo.mIndexCount = mesh.Indices.size();
				modelInfo.mVertexOffset = preVtxOffset + preVtxCnt;
				modelInfo.mIndexOffset = preIdxCnt + preIdxOffset;
				basicModel->SetInfo(modelInfo);
				basicModel->SetTexture(txtureMgr.CreateTexture(textureFilePath + objDiffuseFileName));
				tmpS = XMVectorSet(1, 1, 1, 1);
				tmpR = XMLoadFloat4(&R);
				tmpT = XMLoadFloat3(&T);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

				basicModel->AddInstance(M);
				models[objName + ".fbx"] = basicModel;
				totalBasicVertexCount += mesh.Vertices.size();
				totalIndexCount += mesh.Indices.size();

				//메쉬마다 정점복사.
				for (size_t i = 0; i < mesh.Vertices.size(); ++i)
				{
					basicVtx.Pos = mesh.Vertices[i].Position;
					basicVtx.Normal = mesh.Vertices[i].Normal;
					basicVtx.Tex = mesh.Vertices[i].TexC;
					basicVtxArr.push_back(basicVtx);
// 
// 					basicVtxArr[k].Pos = mesh.Vertices[i].Position;
// 					basicVtxArr[k].Normal = mesh.Vertices[i].Normal;
// 					basicVtxArr[k].Tex = mesh.Vertices[i].TexC;
				}
				idxArr.insert(idxArr.end(), mesh.Indices.begin(), mesh.Indices.end());

			}
			else
			{
				norModel = new NormalModel;
				modelInfo.mIndexCount = mesh.Indices.size();
				modelInfo.mVertexOffset = preVtxOffset + preVtxCnt;
				modelInfo.mIndexOffset = preIdxCnt + preIdxOffset;
				norModel->SetInfo(modelInfo);
				norModel->SetTexture(txtureMgr.CreateTexture(textureFilePath + objDiffuseFileName));
				norModel->SetTextNormalSRV(txtureMgr.CreateTexture(textureFilePath + objNormalFileName));
				tmpS = XMVectorSet(1, 1, 1, 1);
				tmpR = XMLoadFloat4(&R);
				tmpT = XMLoadFloat3(&T);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(tmpS, zero, tmpR, tmpT));

				norModel->AddInstance(M);
				models[objName + ".fbx"] = norModel;

				totalBasicVertexCount += mesh.Vertices.size();
				totalIndexCount += mesh.Indices.size();

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
					normalVtxArr.push_back(posNormalTexTanVtx);
					idxArr.insert(idxArr.end(), mesh.Indices.begin(), mesh.Indices.end());
// 
// 					normalVtxArr[k].Pos = mesh.Vertices[i].Position;
// 					normalVtxArr[k].Normal = mesh.Vertices[i].Normal;
// 					normalVtxArr[k].Tex = mesh.Vertices[i].TexC;
// 					normalVtxArr[k].TangentU = XMFLOAT4(
// 						mesh.Vertices[i].TangentU.x,
// 						mesh.Vertices[i].TangentU.y,
// 						mesh.Vertices[i].TangentU.z,
// 						1.0f
// 					);
				}
			}
		}
		
		//직전에 값들업데이트
		preIdxCnt += mesh.Indices.size();
		preIdxOffset += modelInfo.mIndexOffset;
		preVtxCnt += mesh.Vertices.size();
		preVtxOffset += modelInfo.mVertexOffset;
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

	
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalNormalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vinitData.pSysMem = &normalVtxArr[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_Normal_VB));
	//
	// Pack the indices of all the meshes into one index buffer.
	//

	

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &idxArr[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_IB));

}
