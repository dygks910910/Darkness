#include "ModelManager.h"



CModelManager::CModelManager()

{


}


CModelManager::~CModelManager()
{
	ReleaseCOM(mStaticNormalMappingObjectVB);
	ReleaseCOM(mStaticNormalMappingObjectIB);
	ReleaseCOM(mStaticBasicObjectVB);
	ReleaseCOM(mStaticBasicObjectIB);
	for (auto p : mInstanceModels)
	{
		p.Destroy();
	}

}

void CModelManager::Init(TextureMgr & texMgr, Camera & cam, ID3D11Device* device)
{
	mGridMat.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mGridMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mBoxMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mBoxMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);


	mDevice = device;
	BuildBasicGeometryBuffer();
	BuildShapeGeometryBuffers();
	ReadMapData(texMgr, cam);
}

void CModelManager::DrawStaticNormalModels(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{
	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT  offset = 0;

	dc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	dc->IASetVertexBuffers(0, 1, &mStaticNormalMappingObjectVB, &stride, &offset);
	dc->IASetIndexBuffer(mStaticNormalMappingObjectIB, DXGI_FORMAT_R32_UINT, 0);

	for (auto p : mStaticNormalModels)
	{
		p.Draw(dc, tech, shadowTransform, cam);
	}


}

void CModelManager::DrawStaticBasicModels(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{
	UINT stride = sizeof(Vertex::Basic32);
	UINT  offset = 0;
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetVertexBuffers(0, 1, &mStaticBasicObjectVB, &stride, &offset);
	dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
	for (auto p : mStaticBasicModels)
	{
		p.Draw(dc, tech, shadowTransform, cam);
	}
}

void CModelManager::DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj)
{
	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;
	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	dc->IASetVertexBuffers(0, 1, &mStaticNormalMappingObjectVB, &stride, &offset);
	dc->IASetIndexBuffer(mStaticNormalMappingObjectIB, DXGI_FORMAT_R32_UINT, 0);
	for (auto p : mStaticNormalModels)
	{
		p.DrawToShadowMap(dc, tech, lightView, lightProj);
	}

	stride = sizeof(Vertex::Basic32);
	offset = 0;

	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetVertexBuffers(0, 1, &mStaticBasicObjectVB, &stride, &offset);
	dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
	for (auto p : mStaticBasicModels)
	{
		p.DrawToShadowMap(dc, tech, lightView, lightProj);
	}
// 	UINT instanceStride[2] = { sizeof(Vertex::Basic32), sizeof(XMFLOAT4X4) };
// 	UINT instanceOffset[2] = { 0,0 };
// 	for (int i = 0; i < mInstanceModels.size(); ++i)
// 	{
// 		ID3D11Buffer* vbs[2] = { mStaticBasicObjectVB,mInstanceModels[i].GetInstanceBuffer() };
// 		dc->IASetVertexBuffers(0, 2, vbs, instanceStride, instanceOffset);
// 		dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
// 
// 		mInstanceModels[i].DrawToShadowMap(dc, tech, lightView, lightProj);
// 	}
}

void CModelManager::DrawInstancedModel(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{
	dc->IASetInputLayout(InputLayouts::InstancedBasic32);

	UINT instanceStride[2] = { sizeof(Vertex::Basic32), sizeof(XMFLOAT4X4) };
	UINT instanceOffset[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { mStaticBasicObjectVB,mInstanceModels[0].GetInstanceBuffer() };
	

	for (int i = 0; i < mInstanceModels.size(); ++i)
	{
		dc->IASetVertexBuffers(0, 2, vbs, instanceStride, instanceOffset);
		dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
		//Effects::InstancedBasicFX->Light3TexTech->GetDesc(&techDesc);
		

		mInstanceModels[i].Draw(dc, tech, shadowTransform, cam);
	}

}

void CModelManager::BuildShapeGeometryBuffers()
{




	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData clown;





	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(10.0f, 10.0f, 11, 11, grid);
	CFbxLoader loader;


	loader.LoadFBX("true_clownTri.FBX", clown);




	boxVertexOffset = 0;
	gridVertexOffset = box.Vertices.size();
	clownVertexOffset = gridVertexOffset + grid.Vertices.size();



	boxIndexCount = box.Indices.size();
	gridIndexCount = grid.Indices.size();
	clownIndexCount = clown.Indices.size();




	boxIndexOffset = 0;
	gridIndexOffset = boxIndexCount;
	clownIndexOffset = gridIndexOffset + gridIndexCount;



	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		clown.Vertices.size();

	UINT totalIndexCount =
		boxIndexCount +
		gridIndexCount +
		clownIndexCount;

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
			1.0f);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex = grid.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			grid.Vertices[i].TangentU.x,
			grid.Vertices[i].TangentU.y,
			grid.Vertices[i].TangentU.z,
			1.0f);
	}

	for (size_t i = 0; i < clown.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = clown.Vertices[i].Position;
		vertices[k].Normal = clown.Vertices[i].Normal;
		vertices[k].Tex = clown.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			clown.Vertices[i].TangentU.x,
			clown.Vertices[i].TangentU.y,
			clown.Vertices[i].TangentU.z,
			1.0f);
	}



	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(mDevice->CreateBuffer(&vbd, &vinitData, &mStaticNormalMappingObjectVB));

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	//////////////////////////////////////////////////////////////////////////
	/*
	2017 / 3 / 26 / 1:55
	작성자:박요한(dygks910910@daum.net)
	설명:fbx로더에 문제가 있어서 와인딩오더를 반대로 해줌.fbx로더에서 인덱스를 잘못 뽑는거같다.
	따라서 앞으로 FBX는 무조건 와인딩오더 반대로 하길바람.
	*/
	indices.insert(indices.end(), clown.Indices.rbegin(), clown.Indices.rend());


	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(mDevice->CreateBuffer(&ibd, &iinitData, &mStaticNormalMappingObjectIB));


	//////////////////////////////////////////////////////////////////////////
	//basic데이터 추출
	BuildBasicGeometryBuffer();

	//////////////////////////////////////////////////////////////////////////
	//맵 데이터 읽기!!!!


}

void CModelManager::BuildBasicGeometryBuffer()
{
	GeometryGenerator::MeshData fence, house1, house2, house3, house4, house5, house6;
	CFbxLoader loader;

	loader.LoadFBX("fence1.FBX", fence);
	loader.LoadFBX("house 1.fbx", house1);
	loader.LoadFBX("house 2.fbx", house2);
	loader.LoadFBX("house 3.fbx", house3);
	loader.LoadFBX("house 4.fbx", house4);
	loader.LoadFBX("house 5.fbx", house5);
	loader.LoadFBX("house 6.fbx", house6);

	house1VertexOffset = 0;
	house2VertexOffset = house1.Vertices.size();
	house3VertexOffset = house2VertexOffset + house2.Vertices.size();
	house4VertexOffset = house3VertexOffset + house3.Vertices.size();
	house5VertexOffset = house4VertexOffset + house4.Vertices.size();
	house6VertexOffset = house5VertexOffset + house5.Vertices.size();
	fenceVertexOffset = house6VertexOffset + house6.Vertices.size();

	house1IndexCount = house1.Indices.size();
	house2IndexCount = house2.Indices.size();
	house3IndexCount = house3.Indices.size();
	house4IndexCount = house4.Indices.size();
	house5IndexCount = house5.Indices.size();
	house6IndexCount = house6.Indices.size();
	fenceIndexCount = fence.Indices.size();

	house1IndexOffset = 0;
	house2IndexOffset = house1IndexCount;
	house3IndexOffset = house2IndexOffset + house2IndexCount;
	house4IndexOffset = house3IndexOffset + house3IndexCount;
	house5IndexOffset = house4IndexOffset + house4IndexCount;
	house6IndexOffset = house5IndexOffset + house5IndexCount;
	fenceIndexOffset = house6IndexOffset + house6IndexCount;

	UINT totalVertexCount =
		house1.Vertices.size() +
		house2.Vertices.size() +
		house3.Vertices.size() +
		house4.Vertices.size() +
		house5.Vertices.size() +
		house6.Vertices.size() +
		fence.Vertices.size();

	UINT totalIndexCount =
		house1IndexCount +
		house2IndexCount +
		house3IndexCount +
		house4IndexCount +
		house5IndexCount +
		house6IndexCount +
		fenceIndexCount;

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < house1.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house1.Vertices[i].Position;
		vertices[k].Normal = house1.Vertices[i].Normal;
		vertices[k].Tex = house1.Vertices[i].TexC;

	}
	for (size_t i = 0; i < house2.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house2.Vertices[i].Position;
		vertices[k].Normal = house2.Vertices[i].Normal;
		vertices[k].Tex = house2.Vertices[i].TexC;

	}
	for (size_t i = 0; i < house3.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house3.Vertices[i].Position;
		vertices[k].Normal = house3.Vertices[i].Normal;
		vertices[k].Tex = house3.Vertices[i].TexC;

	}
	for (size_t i = 0; i < house4.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house4.Vertices[i].Position;
		vertices[k].Normal = house4.Vertices[i].Normal;
		vertices[k].Tex = house4.Vertices[i].TexC;

	}
	for (size_t i = 0; i < house5.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house5.Vertices[i].Position;
		vertices[k].Normal = house5.Vertices[i].Normal;
		vertices[k].Tex = house5.Vertices[i].TexC;

	}
	for (size_t i = 0; i < house6.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = house6.Vertices[i].Position;
		vertices[k].Normal = house6.Vertices[i].Normal;
		vertices[k].Tex = house6.Vertices[i].TexC;

	}
	for (size_t i = 0; i < fence.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = fence.Vertices[i].Position;
		vertices[k].Normal = fence.Vertices[i].Normal;
		vertices[k].Tex = fence.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC basicvbd;
	basicvbd.Usage = D3D11_USAGE_IMMUTABLE;
	basicvbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	basicvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	basicvbd.CPUAccessFlags = 0;
	basicvbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA basicvinitData;
	basicvinitData.pSysMem = &vertices[0];
	HR(mDevice->CreateBuffer(&basicvbd, &basicvinitData, &mStaticBasicObjectVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//
	std::vector<UINT> indices;
	indices.insert(indices.end(), house1.Indices.rbegin(), house1.Indices.rend());
	indices.insert(indices.end(), house2.Indices.rbegin(), house2.Indices.rend());
	indices.insert(indices.end(), house3.Indices.rbegin(), house3.Indices.rend());
	indices.insert(indices.end(), house4.Indices.rbegin(), house4.Indices.rend());
	indices.insert(indices.end(), house5.Indices.rbegin(), house5.Indices.rend());
	indices.insert(indices.end(), house6.Indices.rbegin(), house6.Indices.rend());
	indices.insert(indices.end(), fence.Indices.rbegin(), fence.Indices.rend());



	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(mDevice->CreateBuffer(&ibd, &iinitData, &mStaticBasicObjectIB));

}

void CModelManager::ReadMapData(TextureMgr& texMgr, Camera& cam)
{
	std::ifstream ifs;
	ifs.open("MapData.txt");
	XMFLOAT3 scale;
	XMFLOAT4 rotation;
	XMFLOAT3 position;

	CInstanceBasicModel tempInstanceModel;

	char objectName[50];
	std::string cIgnore;
	int StaticObjCount;
	int instancedObjectCount;
	int skinnedObjectCount;
	ifs >> cIgnore;
	ifs >> cIgnore >> StaticObjCount;
	ifs >> cIgnore >> skinnedObjectCount;
	ifs >> cIgnore >> instancedObjectCount;
	//////////////////////////////////////////////////////////////////////////
	///Reading staticObject
	ifs >> cIgnore;
	for (int i = 0; i < StaticObjCount; ++i)
	{
		ifs >> objectName;
		ifs >> cIgnore >> position.x >> position.y >> position.z;
		ifs >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w	;
		ifs >> cIgnore >> scale.x >> scale.y >> scale.z;

		if (!strcmp(objectName, "Cube"))
		{

			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

			mStaticNormalModels.push_back(CStaticNomalModel(M,
				mBoxMat,
				boxIndexCount,
				boxVertexOffset,
				boxIndexOffset,
				texMgr.CreateTexture(L"Textures\\bricks.dds"),
				texMgr.CreateTexture(L"Textures\\bricks_nmap.dds"),
				"box"
			));

		}
	
		else if (!strcmp(objectName, "MainCamera"))
		{
			cam.SetPosition(position.x, position.y, position.z);
		}
		else if (!strcmp(objectName, "Plane"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticNormalModels.push_back(CStaticNomalModel(M,
				mGridMat,
				gridIndexCount,
				gridVertexOffset,
				gridIndexOffset,
				texMgr.CreateTexture(L"Textures\\floor.dds"),
				texMgr.CreateTexture(L"Textures\\floor_nmap.dds"),
				"grid"
			));
		}
		
		else if (!strcmp(objectName, "house_1"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house1IndexCount,
				house1VertexOffset,
				house1IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house1"
			));
		}
		else if (!strcmp(objectName, "house_2"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house2IndexCount,
				house2VertexOffset,
				house2IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house2"
			));
		}
		else if (!strcmp(objectName, "house_3"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house3IndexCount,
				house3VertexOffset,
				house3IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house3"
			));
		}
		else if (!strcmp(objectName, "house_4"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house4IndexCount,
				house4VertexOffset,
				house4IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house4"
			));
		}
		else if (!strcmp(objectName, "house_5"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			ID3D11ShaderResourceView* tempSRV = texMgr.CreateTexture(L"house diffuse.dds");
			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house5IndexCount,
				house5VertexOffset,
				house5IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house5"
			));
		}
		else if (!strcmp(objectName, "house_6"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house6IndexCount,
				house6VertexOffset,
				house6IndexOffset,
				texMgr.CreateTexture(L"house diffuse.dds"),
				"house6"
			));
		}
		else
		{
			std::cout << "찾을수 없음" << std::endl;
		}
// 		std::cout << objectName << std::endl;
// 		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
// 		std::cout << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << std::endl;
// 		std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
 		ZeroMemory(&objectName, sizeof(objectName));
 		ZeroMemory(&cIgnore, sizeof(cIgnore));
 		ZeroMemory(&position, sizeof(position));
 		ZeroMemory(&rotation, sizeof(rotation));
 		ZeroMemory(&scale, sizeof(scale));
	}
	ifs >> cIgnore;
	for (int i = 0; i < instancedObjectCount; ++i)
	{
		ifs >> objectName;
		ifs >> cIgnore >> position.x >> position.y >> position.z;
		ifs >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
		ifs >> cIgnore >> scale.x >> scale.y >> scale.z;

		if (!strcmp(objectName, "fence1"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			//tempInstanceWorld.push_back(M);
			tempInstanceModel.AddInstanceWorld(M);

		}
	}
//	인스턴싱할 객체를 Setting
 	tempInstanceModel.SetDrawInfomation(fenceIndexCount, fenceVertexOffset, fenceIndexOffset);
 	tempInstanceModel.SetMatrial(mBoxMat);
 	tempInstanceModel.SetSRV(texMgr.CreateTexture(L"diff_fence_gate.dds"));
 	tempInstanceModel.BuildInstanceBuffer(mDevice);
 	mInstanceModels.push_back(tempInstanceModel);

	ifs >> cIgnore;
	for (int i = 0; i < skinnedObjectCount; ++i)
	{
		ifs >> objectName;
		ifs >> cIgnore >> position.x >> position.y >> position.z;
		ifs >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
		ifs >> cIgnore >> scale.x >> scale.y >> scale.z;

			if (!strcmp(objectName, "Clown"))
			{
				XMVECTOR S = XMLoadFloat3(&scale);
				XMVECTOR P = XMLoadFloat3(&position);
				XMVECTOR Q = XMLoadFloat4(&rotation);
				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

				XMFLOAT4X4 M;
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
				mStaticNormalModels.push_back(CStaticNomalModel(M,
					mBoxMat,
					clownIndexCount,
					clownVertexOffset,
					clownIndexOffset,
					texMgr.CreateTexture(L"true_clown_diffuse1.png"),
					texMgr.CreateTexture(L"true_clown_normals.png"),
					"clown"
				));

				ifs >> objectName;
				ifs >> cIgnore >> position.x >> position.y >> position.z;
				ifs >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
				ifs >> cIgnore >> scale.x >> scale.y >> scale.z;

				cam.SetPosition(position);
			}
		///skinnedModelRead....
	}
	ifs.close();

}

