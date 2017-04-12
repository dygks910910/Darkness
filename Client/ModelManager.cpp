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
	SafeDelete(mCharacterModel);
	for (auto p : mInstanceModels)
	{
		p.Destroy();
	}
	for (auto p = mclipAnimbuf.begin(); p != mclipAnimbuf.end(); ++p)
	{
		delete[](p->second);
	}

}

void CModelManager::Init(TextureMgr& texMgr, Camera* cam, ID3D11Device* device)
{
	mDevice = device;
	mGridMat.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mGridMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mBoxMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mBoxMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	int animTotalCounts[4];
	std::string clipname[4] = { "Idle", "Walk", "Attack1", "Run" };


	mCharacterModel = new SkinnedModel(mDevice, texMgr, "Models/Clown.txt", L"Textures\\");
	std::ifstream fin("Models/ClownAnimationData.txt");
	std::vector<std::vector<XMFLOAT4X4>> testfinalTransform;
	int total = 0;

	for (int k = 0; k < 4; ++k)
	{
		fin >> total;
		animTotalCounts[k] = total;
		auto TestFinalTransforms = new std::vector<XMFLOAT4X4>[total];
		//testfinalTransform.resize(total);
		for (int i = 0; i < total; ++i)
		{
			TestFinalTransforms[i].resize(mCharacterModel->SkinnedData.BoneCount());
			for (int j = 0; j < mCharacterModel->SkinnedData.BoneCount(); ++j)
			{
				fin >> TestFinalTransforms[i][j]._11 >> TestFinalTransforms[i][j]._12 >> TestFinalTransforms[i][j]._13 >> TestFinalTransforms[i][j]._14
					>> TestFinalTransforms[i][j]._21 >> TestFinalTransforms[i][j]._22 >> TestFinalTransforms[i][j]._23 >> TestFinalTransforms[i][j]._24
					>> TestFinalTransforms[i][j]._31 >>TestFinalTransforms[i][j]._32 >> TestFinalTransforms[i][j]._33 >> TestFinalTransforms[i][j]._34
					>> TestFinalTransforms[i][j]._41 >> TestFinalTransforms[i][j]._42 >> TestFinalTransforms[i][j]._43 >> TestFinalTransforms[i][j]._44;
			}
		}
		mclipAnimbuf.insert(std::pair<std::string, std::vector<XMFLOAT4X4>*>(clipname[k], TestFinalTransforms));

		testfinalTransform.clear();
	}
	fin.close();

	for (int i = 0; i < 4; ++i)
	{
		mClipnameAndTotalCounts[i] = std::make_pair(clipname[i], animTotalCounts[i]);
	}


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

void CModelManager::DrawStaticSsaoNormalModels(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{
	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT  offset = 0;

	dc->IASetInputLayout(InputLayouts::PosNormalTexTan);
	dc->IASetVertexBuffers(0, 1, &mStaticNormalMappingObjectVB, &stride, &offset);
	dc->IASetIndexBuffer(mStaticNormalMappingObjectIB, DXGI_FORMAT_R32_UINT, 0);

	for (auto p : mStaticNormalModels)
	{
		p.DrawSsao(dc, tech, shadowTransform, cam);
		//p.Draw(dc, tech, shadowTransform, cam);
	}

	UINT stride2 = sizeof(Vertex::Basic32);
	UINT  offset2 = 0;
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->IASetVertexBuffers(0, 1, &mStaticBasicObjectVB, &stride2, &offset2);
	dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
	for (auto p : mStaticBasicModels)
	{
		p.DrawSsao(dc, tech, shadowTransform, cam);
	}

	//dc->IASetInputLayout(InputLayouts::InstancedBasic32);

	//UINT instanceStride[2] = { sizeof(Vertex::Basic32), sizeof(XMFLOAT4X4) };
	//UINT instanceOffset[2] = { 0,0 };
	//ID3D11Buffer* vbs[2] = { mStaticBasicObjectVB,mInstanceModels[0].GetInstanceBuffer() };


	//for (int i = 0; i < mInstanceModels.size(); ++i)
	//{
	//	dc->IASetVertexBuffers(0, 2, vbs, instanceStride, instanceOffset);
	//	dc->IASetIndexBuffer(mStaticBasicObjectIB, DXGI_FORMAT_R32_UINT, 0);
	//	//Effects::InstancedBasicFX->Light3TexTech->GetDesc(&techDesc);


	//	mInstanceModels[i].DrawSsao(dc, tech, shadowTransform, cam);
	//}


}

void CModelManager::DrawSkinnedModels(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, const XMMATRIX & shadowTransform, const Camera & cam)
{
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	tech->GetDesc(&techDesc);
	for (int i = 0; i < mSkinnedModelInstance.size(); ++i)
	{
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			world = XMLoadFloat4x4(&mSkinnedModelInstance[i].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*cam.View()*cam.Proj();

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetBoneTransforms(
				&mSkinnedModelInstance[i].FinalTransforms[0],
				mSkinnedModelInstance[i].FinalTransforms.size());

			for (UINT subset = 0; subset < mSkinnedModelInstance[i].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(mSkinnedModelInstance[i].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(mSkinnedModelInstance[i].Model->DiffuseMapSRV[subset]);
				Effects::NormalMapFX->SetNormalMap(mSkinnedModelInstance[i].Model->NormalMapSRV[subset]);

				tech->GetPassByIndex(p)->Apply(0, dc);
				mSkinnedModelInstance[i].Model->ModelMesh.Draw(dc, subset);
			}
		}
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

void CModelManager::UpdateModel(const float & dt)
{
	for (int i = 0; i < mSkinnedModelInstance.size(); ++i)
	{
		mSkinnedModelInstance[i].Update(dt);
	}
}

void CModelManager::BuildShapeGeometryBuffers()
{




	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;





	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(10.0f, 10.0f, 11, 11, grid);
	CFbxLoader loader;




	boxVertexOffset = 0;
	gridVertexOffset = box.Vertices.size();



	boxIndexCount = box.Indices.size();
	gridIndexCount = grid.Indices.size();




	boxIndexOffset = 0;
	gridIndexOffset = boxIndexCount;



	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size();

		UINT totalIndexCount =
		boxIndexCount +
		gridIndexCount;

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
	//indices.insert(indices.end(), clown.Indices.rbegin(), clown.Indices.rend());


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
	GeometryGenerator::MeshData fence, house1, house2, house3, house4, house5, house6,angelStatue;
	CFbxLoader loader;

	loader.LoadFBX("Darkness fbx\\fence1.FBX", fence);
	loader.LoadFBX("Darkness fbx\\house 1.fbx", house1);
	loader.LoadFBX("Darkness fbx\\house 2.fbx", house2);
	loader.LoadFBX("Darkness fbx\\house 3.fbx", house3);
	loader.LoadFBX("Darkness fbx\\house 4.fbx", house4);
	loader.LoadFBX("Darkness fbx\\house 5.fbx", house5);
	loader.LoadFBX("Darkness fbx\\house 6.fbx", house6);
	loader.LoadFBX("Darkness fbx\\angelStatue.fbx", angelStatue);

//////////////////////////////////////////////////////////////////////////
	//VertexOffset설정

	house1VertexOffset = 0;
	house2VertexOffset = house1.Vertices.size();
	house3VertexOffset = house2VertexOffset + house2.Vertices.size();
	house4VertexOffset = house3VertexOffset + house3.Vertices.size();
	house5VertexOffset = house4VertexOffset + house4.Vertices.size();
	house6VertexOffset = house5VertexOffset + house5.Vertices.size();
	fenceVertexOffset = house6VertexOffset + house6.Vertices.size();
	angelStatueVertexOffset = fenceVertexOffset + fence.Vertices.size();

	//////////////////////////////////////////////////////////////////////////
	//indexCount설정.
	house1IndexCount = house1.Indices.size();
	house2IndexCount = house2.Indices.size();
	house3IndexCount = house3.Indices.size();
	house4IndexCount = house4.Indices.size();
	house5IndexCount = house5.Indices.size();
	house6IndexCount = house6.Indices.size();
	fenceIndexCount = fence.Indices.size();
	angelStatueIndexCount = angelStatue.Indices.size();
	//////////////////////////////////////////////////////////////////////////
	//indexOffset설정

	house1IndexOffset = 0;
	house2IndexOffset = house1IndexCount;
	house3IndexOffset = house2IndexOffset + house2IndexCount;
	house4IndexOffset = house3IndexOffset + house3IndexCount;
	house5IndexOffset = house4IndexOffset + house4IndexCount;
	house6IndexOffset = house5IndexOffset + house5IndexCount;
	fenceIndexOffset = house6IndexOffset + house6IndexCount;
	angelStatueIndexOffset = fenceIndexOffset + fenceIndexCount;

	UINT totalVertexCount =
		house1.Vertices.size() +
		house2.Vertices.size() +
		house3.Vertices.size() +
		house4.Vertices.size() +
		house5.Vertices.size() +
		house6.Vertices.size() +
		fence.Vertices.size() + 
		angelStatue.Vertices.size();

	UINT totalIndexCount =
		house1IndexCount +
		house2IndexCount +
		house3IndexCount +
		house4IndexCount +
		house5IndexCount +
		house6IndexCount +
		fenceIndexCount + 
		angelStatueIndexCount;

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
	for (size_t i = 0; i < angelStatue.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = angelStatue.Vertices[i].Position;
		vertices[k].Normal = angelStatue.Vertices[i].Normal;
		vertices[k].Tex = angelStatue.Vertices[i].TexC;
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
	indices.insert(indices.end(), angelStatue.Indices.rbegin(), angelStatue.Indices.rend());


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

void CModelManager::ReadMapData(TextureMgr& texMgr, Camera* cam)
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
	
// 		else if (!strcmp(objectName, "MainCamera"))
// 		{
// 			cam.SetPosition(position.x, position.y, position.z);
// 		}
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
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
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
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
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
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
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
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
				"house4"
			));
		}
		else if (!strcmp(objectName, "house_5"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			//ID3D11ShaderResourceView* tempSRV = texMgr.CreateTexture(L"Textures\\house diffuse.dds");
			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mGridMat,
				house5IndexCount,
				house5VertexOffset,
				house5IndexOffset,
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
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
				texMgr.CreateTexture(L"Textures\\house diffuse.dds"),
				"house6"
			));
		}
		else if (!strcmp(objectName, "angelStatue"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mBoxMat,
				angelStatueIndexCount,
				angelStatueVertexOffset,
				angelStatueIndexOffset,
				texMgr.CreateTexture(L"Textures\\angelStatue.png"),
				"angelStatue"
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
 		//ZeroMemory(&cIgnore, sizeof(cIgnore));
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
 	tempInstanceModel.SetSRV(texMgr.CreateTexture(L"Textures\\diff_fence_gate.dds"));
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
				SkinnedModelInstance tempSkinnedModelInstanced;
				tempSkinnedModelInstanced.Model = mCharacterModel;
				tempSkinnedModelInstanced.TimePos = 0;
				tempSkinnedModelInstanced.FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());
				tempSkinnedModelInstanced.World = M;
				tempSkinnedModelInstanced.mClipAnimbuf = &mclipAnimbuf;
				tempSkinnedModelInstanced.mClipnameAndTotalCount = mClipnameAndTotalCounts[0];//idle;
				tempSkinnedModelInstanced.mAnimCnt = 0;
				tempSkinnedModelInstanced.isPlayer = false;

				ifs >> objectName;
				ifs >> cIgnore >> position.x >> position.y >> position.z;

				tempSkinnedModelInstanced.camPos = XMFLOAT3(position.x, position.y, position.z);
				mSkinnedModelInstance.push_back(tempSkinnedModelInstanced);
			}
	}
// 	int randomIndex = rand() % mSkinnedModelInstance.size();
// 	cam->SetPosition(mSkinnedModelInstance[randomIndex].camPos);
// 	mSkinnedModelInstance[randomIndex].isPlayer = true;
// 	mSkinnedModelInstance[randomIndex].cam = cam;
	
	ifs.close();

}

