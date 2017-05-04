#include "ModelManager.h"

#define SPEED 0.008
CModelManager* CModelManager::model = nullptr;
XMFLOAT3 bextent;
XMFLOAT3 bcenter;
bool check;

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
	check = false;
	mDevice = device;
	mGridMat.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
	mGridMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mObjectMaterial.Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mObjectMaterial.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mObjectMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
	mObjectMaterial.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
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
					>> TestFinalTransforms[i][j]._31 >> TestFinalTransforms[i][j]._32 >> TestFinalTransforms[i][j]._33 >> TestFinalTransforms[i][j]._34
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
	ReadMapData(texMgr,*cam);
	send_wsa_buf.buf = reinterpret_cast<char*>(send_buf);
	send_wsa_buf.len = MAX_BUF_SIZE;

}

void CModelManager::DrawStaticNormalModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
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

void CModelManager::DrawSkinnedModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
{
	dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

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
			Effects::NormalMapFX->SetShadowTransform(world*XMLoadFloat4x4(&shadowTransform) );
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetBoneTransforms(
				&mSkinnedModelInstance[i].FinalTransforms[0],
				mSkinnedModelInstance[i].FinalTransforms.size());

			for (UINT subset = 0; subset < mSkinnedModelInstance[i].Model->SubsetCount; ++subset)
			{
				Effects::NormalMapFX->SetMaterial(mSkinnedModelInstance[i].Model->Mat[subset]);
				Effects::NormalMapFX->SetDiffuseMap(mSkinnedModelInstance[i].Model->DiffuseMapSRV[mSkinnedModelInstance[i].selectedDiffuseMapIndex]);
				Effects::NormalMapFX->SetNormalMap(mSkinnedModelInstance[i].Model->NormalMapSRV[subset]);

				tech->GetPassByIndex(p)->Apply(0, dc);
				mSkinnedModelInstance[i].Model->ModelMesh.Draw(dc, subset);
			}
		}
	}
}

void CModelManager::DrawStaticBasicModels(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
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

	dc->IASetInputLayout(InputLayouts::Line);
	//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique* lineTech;
	lineTech = Effects::LineFX->mTech;
	UINT stride2 = sizeof(Vertex::Line);
	UINT offset2 = 0;
	lineTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &mboundVB, &stride2, &offset2);
		dc->IASetIndexBuffer(mboundIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&boundworld);
		XMMATRIX worldViewProj = world*cam.View()*cam.Proj();

		Effects::LineFX->SetWorldViewProj(worldViewProj);

		lineTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(24, 0, 0);
	}
}

void CModelManager::DrawToShadowMap(ID3D11DeviceContext * dc, ID3DX11EffectTechnique * tech, 
	const XMFLOAT4X4 & lightView, const XMFLOAT4X4 & lightProj)
{
	ID3DX11EffectTechnique* animatedSmapTech = Effects::BuildShadowMapFX->BuildShadowMapSkinnedTech;

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
	//////////////////////////////////////////////////////////////////////////애니메이션 객체 그림자
	D3DX11_TECHNIQUE_DESC techDesc;
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;
	XMMATRIX view = XMLoadFloat4x4(&lightView);
	XMMATRIX proj = XMLoadFloat4x4(&lightProj);
	animatedSmapTech->GetDesc(&techDesc);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	for (int i = 0; i < mSkinnedModelInstance.size(); ++i)
	{
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			world = XMLoadFloat4x4(&mSkinnedModelInstance[i].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());
			Effects::BuildShadowMapFX->SetBoneTransforms(
				&mSkinnedModelInstance[i].FinalTransforms[0],
				mSkinnedModelInstance[i].FinalTransforms.size());
			animatedSmapTech->GetPassByIndex(p)->Apply(0, dc);

			for (UINT subset = 0; subset < mSkinnedModelInstance[i].Model->SubsetCount; ++subset)
			{
				mSkinnedModelInstance[i].Model->ModelMesh.Draw(dc, subset);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////까지
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

void CModelManager::DrawInstancedModel(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* tech, const XMFLOAT4X4& shadowTransform, const Camera& cam)
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

void CModelManager::UpdateModel(const float & dt, Camera& camera)
{
	XMFLOAT3 campos, charpos, camLook, charlook, camRight;

	for (int i = 0; i < mSkinnedModelInstance.size(); ++i)
	{
		mSkinnedModelInstance[3].mClipnameAndTotalCount = mClipnameAndTotalCounts[mSkinnedModelInstance[3].mAnimstate];
		mSkinnedModelInstance[i].Update(dt);
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (!mOneCheck)
		{
			mSkinnedModelInstance[5].mAnimCnt = 0;
			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[2];
		}
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		charpos.x = mSkinnedModelInstance[5].World._41;
		charpos.y = mSkinnedModelInstance[5].World._42;
		charpos.z = mSkinnedModelInstance[5].World._43;

		campos = camera.GetPosition();

		cs_packet_player_move* move = reinterpret_cast<cs_packet_player_move*>(&send_buf);
		move->size = sizeof(cs_packet_player_move);
		move->type = CS_UP;
		move->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
		move->camlook = camera.GetLook();
		move->campos = campos;
		send_wsa_buf.len = sizeof(cs_packet_player_move);

		DWORD io_byte;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;

		if (!mOneCheck)
		{
			cs_packet_player_anmation_start* anim = reinterpret_cast<cs_packet_player_anmation_start*>(&send_buf);
			anim->size = sizeof(cs_packet_player_anmation_start);
			anim->type = CS_PACKET_START_ANIMATION;
			anim->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
			anim->animationState = 1;
			send_wsa_buf.len = sizeof(cs_packet_player_anmation_start);

			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[1];
			mOneCheck = true;
		}

		camLook.x = charpos.x - campos.x;
		camLook.y = charpos.y - campos.y;
		camLook.z = charpos.z - campos.z;

		XMVECTOR s = XMVectorReplicate(0.5f*SPEED);
		XMVECTOR l = XMLoadFloat3(&camera.GetLook());
		XMVECTOR p = XMLoadFloat3(&charpos);
		XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
		if (!mSkinnedModelInstance[5].mCollision)
		{
			campos.x += charpos.x - mSkinnedModelInstance[5].World._41;
			campos.z += charpos.z - mSkinnedModelInstance[5].World._43;
		}
		else
		{

			//campos.x += 0;
			//campos.z -= 0.5;
		}

		camera.SetPosition(campos);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		charpos.x = mSkinnedModelInstance[5].World._41;
		charpos.y = mSkinnedModelInstance[5].World._42;
		charpos.z = mSkinnedModelInstance[5].World._43;

		campos = camera.GetPosition();

		cs_packet_player_move* move = reinterpret_cast<cs_packet_player_move*>(&send_buf);
		move->size = sizeof(cs_packet_player_move);
		move->type = CS_DOWN;
		move->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
		move->camlook = camera.GetLook();
		move->campos = campos;
		send_wsa_buf.len = sizeof(cs_packet_player_move);

		DWORD io_byte;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;

		if (!mOneCheck)
		{
			cs_packet_player_anmation_start* anim = reinterpret_cast<cs_packet_player_anmation_start*>(&send_buf);
			anim->size = sizeof(cs_packet_player_anmation_start);
			anim->type = CS_PACKET_START_ANIMATION;
			anim->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
			anim->animationState = 1;
			send_wsa_buf.len = sizeof(cs_packet_player_anmation_start);

			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[1];
			mOneCheck = true;

		}

		camLook.x = charpos.x - campos.x;
		camLook.y = charpos.y - campos.y;
		camLook.z = charpos.z - campos.z;

		XMVECTOR s = XMVectorReplicate(0.5f*SPEED);
		XMVECTOR l = XMLoadFloat3(&camera.GetLook());
		XMVECTOR p = XMLoadFloat3(&charpos);
		XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
		if (!mSkinnedModelInstance[5].mCollision)
		{
			campos.x += charpos.x - mSkinnedModelInstance[5].World._41;
			campos.z += charpos.z - mSkinnedModelInstance[5].World._43;
		}
		else
		{
			campos.x += 0;
			campos.z += 0;
		}

		camera.SetPosition(campos);
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{

		charpos.x = mSkinnedModelInstance[5].World._41;
		charpos.y = mSkinnedModelInstance[5].World._42;
		charpos.z = mSkinnedModelInstance[5].World._43;

		campos = camera.GetPosition();

		cs_packet_player_move* move = reinterpret_cast<cs_packet_player_move*>(&send_buf);
		move->size = sizeof(cs_packet_player_move);
		move->type = CS_LEFT;
		move->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
		move->camlook = camera.GetRight();
		move->campos = campos;
		send_wsa_buf.len = sizeof(cs_packet_player_move);

		DWORD io_byte;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;

		if (!mOneCheck)
		{
			cs_packet_player_anmation_start* anim = reinterpret_cast<cs_packet_player_anmation_start*>(&send_buf);
			anim->size = sizeof(cs_packet_player_anmation_start);
			anim->type = CS_PACKET_START_ANIMATION;
			anim->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
			anim->animationState = 1;
			send_wsa_buf.len = sizeof(cs_packet_player_anmation_start);

			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[1];
			mOneCheck = true;

		}

		camLook.x = charpos.x - campos.x;
		camLook.y = charpos.y - campos.y;
		camLook.z = charpos.z - campos.z;

		XMVECTOR s = XMVectorReplicate(0.5f*SPEED);
		XMVECTOR l = XMLoadFloat3(&camera.GetRight());
		XMVECTOR p = XMLoadFloat3(&charpos);
		XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
		if (!mSkinnedModelInstance[5].mCollision)
		{
			campos.x += charpos.x - mSkinnedModelInstance[5].World._41;
			campos.z += charpos.z - mSkinnedModelInstance[5].World._43;
		}
		else
		{
			campos.x += 0;
			campos.z += 0;
		}

		camera.SetPosition(campos);

	}

	else if (GetAsyncKeyState('D') & 0x8000)
	{

		charpos.x = mSkinnedModelInstance[5].World._41;
		charpos.y = mSkinnedModelInstance[5].World._42;
		charpos.z = mSkinnedModelInstance[5].World._43;

		campos = camera.GetPosition();

		cs_packet_player_move* move = reinterpret_cast<cs_packet_player_move*>(&send_buf);
		move->size = sizeof(cs_packet_player_move);
		move->type = CS_RIGHT;
		move->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
		move->camlook = camera.GetRight();
		move->campos = campos;
		send_wsa_buf.len = sizeof(cs_packet_player_move);

		DWORD io_byte;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;

		if (!mOneCheck)
		{
			cs_packet_player_anmation_start* anim = reinterpret_cast<cs_packet_player_anmation_start*>(&send_buf);
			anim->size = sizeof(cs_packet_player_anmation_start);
			anim->type = CS_PACKET_START_ANIMATION;
			anim->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
			anim->animationState = 1;
			send_wsa_buf.len = sizeof(cs_packet_player_anmation_start);

			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[1];
			mOneCheck = true;

		}

		camLook.x = charpos.x - campos.x;
		camLook.y = charpos.y - campos.y;
		camLook.z = charpos.z - campos.z;

		XMVECTOR s = XMVectorReplicate(0.5f*SPEED);
		XMVECTOR l = XMLoadFloat3(&camera.GetRight());
		XMVECTOR p = XMLoadFloat3(&charpos);
		XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
		if (!mSkinnedModelInstance[5].mCollision)
		{
			campos.x += charpos.x - mSkinnedModelInstance[5].World._41;
			campos.z += charpos.z - mSkinnedModelInstance[5].World._43;
		}
		else
		{
			campos.x += 0;
			campos.z += 0;
		}

		camera.SetPosition(campos);

	}
	else
	{
		if (mOneCheck)
		{
			cs_packet_player_anmation_start* anim = reinterpret_cast<cs_packet_player_anmation_start*>(&send_buf);
			anim->size = sizeof(cs_packet_player_anmation_start);
			anim->type = CS_PACKET_START_ANIMATION;
			anim->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId;
			anim->animationState = 0;
			send_wsa_buf.len = sizeof(cs_packet_player_anmation_start);

			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

			mSkinnedModelInstance[5].mAnimCnt = 0;
			mSkinnedModelInstance[5].mClipnameAndTotalCount = mClipnameAndTotalCounts[0];
		}
		mOneCheck = false;
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
	GeometryGenerator::MeshData fence, house1, house2, house3, house4, house5,
		house6, angelStatue, wall, tower_corner, tower_round, building_b, building_c,
		building_d, building_e, building_f, barrel,
		well,food_a,food_b,hay_a,hay_b,hay_c,hay_d,sack_a,sack_b,sewers_entrance,tent,crate;
	CFbxLoader loader;

	if (bextent.z != 0)
	{
		Vertex::Line boundver[24];
		UINT boundind[24];
		for (int i = 0; i < 24; ++i)
			boundind[i] = i;
		//위//
		boundver[0].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z -bextent.z);
		boundver[0].color = XMFLOAT4(255, 0, 0, 0);
		boundver[1].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z +bextent.z);
		boundver[1].color = XMFLOAT4(255, 0, 0, 0);
		boundver[2].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y -bextent.y, bcenter.z +bextent.z);
		boundver[2].color = XMFLOAT4(255, 0, 0, 0);
		boundver[3].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y -bextent.y, bcenter.z -bextent.z);
		boundver[3].color = XMFLOAT4(255, 0, 0, 0);
		
		//왼//
		boundver[4].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z -bextent.z);
		boundver[4].color = XMFLOAT4(255, 0, 0, 0);
		boundver[5].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z +bextent.z);
		boundver[5].color = XMFLOAT4(255, 0, 0, 0);
		boundver[6].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z +bextent.z);
		boundver[6].color = XMFLOAT4(255, 0, 0, 0);
		boundver[7].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z -bextent.z);
		boundver[7].color = XMFLOAT4(255, 0, 0, 0);

		//오//
		boundver[8].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.x -bextent.y, bcenter.x -bextent.z);
		boundver[8].color = XMFLOAT4(255, 0, 0, 0);
		boundver[9].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.x -bextent.y, bcenter.x +bextent.z);
		boundver[9].color = XMFLOAT4(255, 0, 0, 0);
		boundver[10].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.x +bextent.y, bcenter.x +bextent.z);
		boundver[10].color = XMFLOAT4(255, 0, 0, 0);
		boundver[11].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.x +bextent.y, bcenter.x -bextent.z);
		boundver[11].color = XMFLOAT4(255, 0, 0, 0);

		//밑//
		boundver[12].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y +bextent.y, bcenter.z -bextent.z);
		boundver[12].color = XMFLOAT4(255, 0, 0, 0);
		boundver[13].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z -bextent.z);
		boundver[13].color = XMFLOAT4(255, 0, 0, 0);
		boundver[14].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z +bextent.z);
		boundver[14].color = XMFLOAT4(255, 0, 0, 0);
		boundver[15].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y +bextent.y, bcenter.z +bextent.z);
		boundver[15].color = XMFLOAT4(255, 0, 0, 0);
		
		//앞//
		boundver[16].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z -bextent.z);
		boundver[16].color = XMFLOAT4(255, 0, 0, 0);
		boundver[17].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z -bextent.z);
		boundver[17].color = XMFLOAT4(255, 0, 0, 0);
		boundver[18].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y +bextent.y, bcenter.z -bextent.z);
		boundver[18].color = XMFLOAT4(255, 0, 0, 0);
		boundver[19].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y -bextent.y, bcenter.z -bextent.z);
		boundver[19].color = XMFLOAT4(255, 0, 0, 0);

		//뒤//
		boundver[20].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y -bextent.y, bcenter.z +bextent.z);
		boundver[20].color = XMFLOAT4(255, 0, 0, 0);
		boundver[21].Pos = XMFLOAT3(bcenter.x -bextent.x, bcenter.y +bextent.y, bcenter.z +bextent.z);
		boundver[21].color = XMFLOAT4(255, 0, 0, 0);
		boundver[22].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y +bextent.y, bcenter.z +bextent.z);
		boundver[22].color = XMFLOAT4(255, 0, 0, 0);
		boundver[23].Pos = XMFLOAT3(bcenter.x +bextent.x, bcenter.y -bextent.y, bcenter.z +bextent.z);
		boundver[23].color = XMFLOAT4(255, 0, 0, 0);
		///////boundingbox/////////
		D3D11_BUFFER_DESC boundvbd;
		boundvbd.Usage = D3D11_USAGE_IMMUTABLE;
		boundvbd.ByteWidth = sizeof(Vertex::Line) * 24;
		boundvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		boundvbd.CPUAccessFlags = 0;
		boundvbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA boundinitData;
		boundinitData.pSysMem = boundver;
		HR(mDevice->CreateBuffer(&boundvbd, &boundinitData, &mboundVB));

		D3D11_BUFFER_DESC boundib;
		boundib.Usage = D3D11_USAGE_IMMUTABLE;
		boundib.ByteWidth = sizeof(UINT) * 24;
		boundib.BindFlags = D3D11_BIND_INDEX_BUFFER;
		boundib.CPUAccessFlags = 0;
		boundib.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA boundinit;
		boundinit.pSysMem = boundind;
		HR(mDevice->CreateBuffer(&boundib, &boundinit, &mboundIB));

		XMFLOAT3 scale;
		scale.x = 0.01;
		scale.y = 0.01;
		scale.z = 0.01;

		XMFLOAT3 position;
		position.x = 21.5;
		position.y = 0.4565048;
		position.z = 16.10033;
		/*position.x = 0;
		position.y = 0;
		position.z = 0;
*/
		XMFLOAT4 rotation;
		rotation.x = -0.4992557;
		rotation.y = 0.5009;
		rotation.z = 0.5007431;
		rotation.w = 0.4990985;
		//////buildingc////
	/*rotation.x = -0.4992557f;
	rotation.y = 0.5009f;
	rotation.z = 0.5007431f;
	rotation.w = 0.4990985f;*/

		XMVECTOR S = XMLoadFloat3(&scale);
		XMVECTOR P = XMLoadFloat3(&position);
		XMVECTOR Q = XMLoadFloat4(&rotation);
		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMFLOAT4X4 M;
		XMStoreFloat4x4(&boundworld, XMMatrixAffineTransformation(S, zero, Q, P));
		
		/*XMMATRIX modelScale = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		XMMATRIX modelRot = XMMatrixRotationY(MathHelper::Pi);
		XMMATRIX modelOffset = XMMatrixTranslation(3.45f, 0.444f, -39.659f);
		XMStoreFloat4x4(&boundworld, modelScale*modelRot*modelOffset);*/
		/*boundworld._41 += bcenter.y*0.01;
		boundworld._42 += bcenter.z*0.01;
		boundworld._43 += bcenter.x*0.01;*/

		std::cout << boundworld._41 << ' ' << boundworld._42 << ' ' << boundworld._43 << std::endl;

		///////////////////////////
	}
	// 	loader.LoadFBX("Darkness fbx\\fence1.FBX", fence);
	// 	loader.LoadFBX("Darkness fbx\\house 1.fbx", house1);
	// 	loader.LoadFBX("Darkness fbx\\house 2.fbx", house2);
	// 	loader.LoadFBX("Darkness fbx\\house 3.fbx", house3);
	// 	loader.LoadFBX("Darkness fbx\\house 4.fbx", house4);
	// 	loader.LoadFBX("Darkness fbx\\house 5.fbx", house5);
	// 	loader.LoadFBX("Darkness fbx\\house 6.fbx", house6);
	loader.LoadFBX("Darkness fbx\\angelStatue.fbx", angelStatue);
	loader.LoadFBX("Darkness fbx\\wall.fbx", wall);
	loader.LoadFBX("Darkness fbx\\tower_corner.fbx", tower_corner);
	loader.LoadFBX("Darkness fbx\\tower_round.fbx", tower_round);
	loader.LoadFBX("Darkness fbx\\Building_b.fbx", building_b);
	check = true;
	loader.LoadFBX("Darkness fbx\\Building_c.fbx", building_c);
	check = false;
	loader.LoadFBX("Darkness fbx\\Building_d.fbx", building_d);
	loader.LoadFBX("Darkness fbx\\Building_e.fbx", building_e);
	loader.LoadFBX("Darkness fbx\\Well.FBX", well);
	loader.LoadFBX("Darkness fbx\\Food_a.FBX", food_a);
	loader.LoadFBX("Darkness fbx\\Food_b.FBX", food_b);
	loader.LoadFBX("Darkness fbx\\Hay_a.FBX", hay_a);
	loader.LoadFBX("Darkness fbx\\Hay_b.FBX", hay_b);
	loader.LoadFBX("Darkness fbx\\Hay_c.FBX", hay_c);
	loader.LoadFBX("Darkness fbx\\Hay_d.FBX", hay_d);
	loader.LoadFBX("Darkness fbx\\Sack_a.FBX", sack_a);
	loader.LoadFBX("Darkness fbx\\Sack_b.FBX", sack_b);
	loader.LoadFBX("Darkness fbx\\Sewers_entrance.FBX", sewers_entrance);
	loader.LoadFBX("Darkness fbx\\Tent.FBX", tent);
	loader.LoadFBX("Darkness fbx\\Crate.FBX", crate);
	loader.LoadFBX("Darkness fbx\\Building_f.FBX", building_f);
	loader.LoadFBX("Darkness fbx\\Barrel.FBX", barrel);

	//////////////////////////////////////////////////////////////////////////
		//VertexOffset설정

// 	house1VertexOffset = 0;
// 	house2VertexOffset = house1.Vertices.size();
// 	house3VertexOffset = house2VertexOffset + house2.Vertices.size();
// 	house4VertexOffset = house3VertexOffset + house3.Vertices.size();
// 	house5VertexOffset = house4VertexOffset + house4.Vertices.size();
// 	house6VertexOffset = house5VertexOffset + house5.Vertices.size();
// 	fenceVertexOffset = house6VertexOffset + house6.Vertices.size();
	angelStatueVertexOffset = 0;
	wallVertexOffset = angelStatue.Vertices.size();
	tower_conerVertexOffset = wallVertexOffset + wall.Vertices.size();
	tower_roundVertexOffset = tower_conerVertexOffset + tower_corner.Vertices.size();
	building_b_VertexOffset = tower_roundVertexOffset + tower_round.Vertices.size();
	building_c_VertexOffset = building_b_VertexOffset + building_b.Vertices.size();
	building_d_VertexOffset = building_c_VertexOffset + building_c.Vertices.size();
	building_e_VertexOffset = building_d_VertexOffset + building_d.Vertices.size();

	well_VertexOffset = building_e_VertexOffset + building_e.Vertices.size();
	food_a_VertexOffset = well_VertexOffset + well.Vertices.size();
	food_b_VertexOffset = food_a_VertexOffset + food_a.Vertices.size();
	hay_a_VertexOffset = food_b_VertexOffset + food_b.Vertices.size();
	hay_b_VertexOffset = hay_a_VertexOffset + hay_a.Vertices.size();
	hay_c_VertexOffset = hay_b_VertexOffset + hay_b.Vertices.size();
	hay_d_VertexOffset = hay_c_VertexOffset + hay_c.Vertices.size();
	sack_a_VertexOffset = hay_d_VertexOffset + hay_d.Vertices.size();
	sack_b_VertexOffset = sack_a_VertexOffset + sack_a.Vertices.size();
	sewers_entrance_VertexOffset = sack_b_VertexOffset + sack_b.Vertices.size();
	tent_VertexOffset = sewers_entrance_VertexOffset + sewers_entrance.Vertices.size();
	crate_VertexOffset = tent_VertexOffset + tent.Vertices.size();
	building_f_VertexOffset = crate_VertexOffset + crate.Vertices.size();
	barrel_VertexOffset = building_f_VertexOffset + building_f.Vertices.size();
	//////////////////////////////////////////////////////////////////////////
	//indexCount설정.
// 	house1IndexCount = house1.Indices.size();
// 	house2IndexCount = house2.Indices.size();
// 	house3IndexCount = house3.Indices.size();
// 	house4IndexCount = house4.Indices.size();
// 	house5IndexCount = house5.Indices.size();
// 	house6IndexCount = house6.Indices.size();
// 	fenceIndexCount = fence.Indices.size();
	angelStatueIndexCount = angelStatue.Indices.size();
	wallIndexCount = wall.Indices.size();
	tower_conerIndexCount = tower_corner.Indices.size();
	tower_roundIndexCount = tower_round.Indices.size();
	building_b_IndexCount = building_b.Indices.size();
	building_c_IndexCount = building_c.Indices.size();
	building_d_IndexCount = building_d.Indices.size();
	building_e_IndexCount = building_e.Indices.size();
	well_IndexCount = well.Indices.size();
	food_a_IndexCount = food_a.Indices.size();
	food_b_IndexCount = food_b.Indices.size();
	hay_a_IndexCount = hay_a.Indices.size();
	hay_b_IndexCount = hay_b.Indices.size();
	hay_c_IndexCount = hay_c.Indices.size();
	hay_d_IndexCount = hay_d.Indices.size();
	sack_a_IndexCount = sack_a.Indices.size();
	sack_b_IndexCount = sack_b.Indices.size();
	sewers_entrance_IndexCount = sewers_entrance.Indices.size();
	tent_indexCount = tent.Indices.size();
	crate_indexCount = crate.Indices.size();
	building_f_IndexCount = building_f.Indices.size();
	barrel_IndexCount = barrel.Indices.size();
	//////////////////////////////////////////////////////////////////////////
	//indexOffset설정

// 	house1IndexOffset = 0;
// 	house2IndexOffset = house1IndexCount;
// 	house3IndexOffset = house2IndexOffset + house2IndexCount;
// 	house4IndexOffset = house3IndexOffset + house3IndexCount;
// 	house5IndexOffset = house4IndexOffset + house4IndexCount;
// 	house6IndexOffset = house5IndexOffset + house5IndexCount;
// 	fenceIndexOffset = house6IndexOffset + house6IndexCount;
	angelStatueIndexOffset = 0;
	wallIndexOffset = angelStatueIndexCount;
	tower_conerIndexOffset = wallIndexOffset + wallIndexCount;
	tower_roundIndexOffset = tower_conerIndexOffset + tower_conerIndexCount;
	building_b_IndexOffset = tower_roundIndexOffset + tower_roundIndexCount;
	building_c_IndexOffset = building_b_IndexOffset + building_b_IndexCount;
	building_d_IndexOffset = building_c_IndexOffset + building_c_IndexCount;
	building_e_IndexOffset = building_d_IndexOffset + building_d_IndexCount;
	well_IndexOffset = building_e_IndexOffset + building_e_IndexCount;
	food_a_IndexOffset = well_IndexOffset + well_IndexCount;
	food_b_IndexOffset = food_a_IndexOffset + food_a_IndexCount;
	hay_a_IndexOffset = food_b_IndexOffset + food_b_IndexCount;
	hay_b_IndexOffset = hay_a_IndexOffset + hay_a_IndexCount;
	hay_c_IndexOffset = hay_b_IndexOffset + hay_b_IndexCount;
	hay_d_IndexOffset = hay_c_IndexOffset + hay_c_IndexCount;
	sack_a_IndexOffset = hay_d_IndexOffset + hay_d_IndexCount;
	sack_b_IndexOffset = sack_a_IndexOffset + sack_a_IndexCount;
	sewers_entrance_IndexOffset = sack_b_IndexOffset + sack_b_IndexCount;
	tent_indexOffset = sewers_entrance_IndexOffset + sewers_entrance_IndexCount;
	crate_indexOffset = tent_indexOffset + tent_indexCount;
	building_f_IndexOffset = crate_indexOffset + crate_indexCount;
	barrel_IndexOffset = building_f_IndexOffset + building_f_IndexCount;
	//////////////////////////////////////////////////////////////////////////
	UINT totalVertexCount =
		// 		house1.Vertices.size() +
		// 		house2.Vertices.size() +
		// 		house3.Vertices.size() +
		// 		house4.Vertices.size() +
		// 		house5.Vertices.size() +
		// 		house6.Vertices.size() +
		// 		fence.Vertices.size() +
		angelStatue.Vertices.size() +
		wall.Vertices.size() +
		tower_corner.Vertices.size() +
		tower_round.Vertices.size() +
		building_b.Vertices.size() +
		building_c.Vertices.size() +
		building_d.Vertices.size() +
		building_e.Vertices.size() +
		well.Vertices.size() +
		food_a.Vertices.size() +
		food_b.Vertices.size() +
		hay_a.Vertices.size() +
		hay_b.Vertices.size() +
		hay_c.Vertices.size() +
		hay_d.Vertices.size() +
		sack_a.Vertices.size() +
		sack_b.Vertices.size() +
		sewers_entrance.Vertices.size() +
		tent.Vertices.size() +
		crate.Vertices.size() +
		building_f.Vertices.size() +
		barrel.Vertices.size();

	UINT totalIndexCount =
		// 		house1IndexCount +
		// 		house2IndexCount +
		// 		house3IndexCount +
		// 		house4IndexCount +
		// 		house5IndexCount +
		// 		house6IndexCount +
		// 		fenceIndexCount +
		angelStatueIndexCount +
		wallIndexCount +
		tower_conerIndexCount +
		tower_roundIndexCount +
		building_b_IndexCount +
		building_c_IndexCount +
		building_d_IndexCount +
		building_e_IndexCount +
		well_IndexCount +
		food_a_IndexCount +
		food_b_IndexCount +
		hay_a_IndexCount +
		hay_b_IndexCount +
		hay_c_IndexCount +
		hay_d_IndexCount +
		sack_a_IndexCount +
		sack_b_IndexCount +
		sewers_entrance_IndexCount +
		tent_indexCount + crate_indexCount + building_f_IndexCount + barrel_IndexCount;
		;

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;

	for (size_t i = 0; i < angelStatue.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = angelStatue.Vertices[i].Position;
		vertices[k].Normal = angelStatue.Vertices[i].Normal;
		vertices[k].Tex = angelStatue.Vertices[i].TexC;
	}
	for (size_t i = 0; i < wall.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = wall.Vertices[i].Position;
		vertices[k].Normal = wall.Vertices[i].Normal;
		vertices[k].Tex = wall.Vertices[i].TexC;
	}
	for (size_t i = 0; i < tower_corner.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = tower_corner.Vertices[i].Position;
		vertices[k].Normal = tower_corner.Vertices[i].Normal;
		vertices[k].Tex = tower_corner.Vertices[i].TexC;
	}
	for (size_t i = 0; i < tower_round.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = tower_round.Vertices[i].Position;
		vertices[k].Normal = tower_round.Vertices[i].Normal;
		vertices[k].Tex = tower_round.Vertices[i].TexC;
	}
	for (size_t i = 0; i < building_b.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = building_b.Vertices[i].Position;
		vertices[k].Normal = building_b.Vertices[i].Normal;
		vertices[k].Tex = building_b.Vertices[i].TexC;
	}
	for (size_t i = 0; i < building_c.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = building_c.Vertices[i].Position;
		vertices[k].Normal = building_c.Vertices[i].Normal;
		vertices[k].Tex = building_c.Vertices[i].TexC;
	}
	for (size_t i = 0; i < building_d.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = building_d.Vertices[i].Position;
		vertices[k].Normal = building_d.Vertices[i].Normal;
		vertices[k].Tex = building_d.Vertices[i].TexC;
	}
	for (size_t i = 0; i < building_e.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = building_e.Vertices[i].Position;
		vertices[k].Normal = building_e.Vertices[i].Normal;
		vertices[k].Tex = building_e.Vertices[i].TexC;
	}
	for (size_t i = 0; i < well.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = well.Vertices[i].Position;
		vertices[k].Normal = well.Vertices[i].Normal;
		vertices[k].Tex = well.Vertices[i].TexC;
	}
	for (size_t i = 0; i < food_a.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = food_a.Vertices[i].Position;
		vertices[k].Normal = food_a.Vertices[i].Normal;
		vertices[k].Tex = food_a.Vertices[i].TexC;
	}
	for (size_t i = 0; i < food_b.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = food_b.Vertices[i].Position;
		vertices[k].Normal = food_b.Vertices[i].Normal;
		vertices[k].Tex = food_b.Vertices[i].TexC;
	}
	for (size_t i = 0; i < hay_a.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = hay_a.Vertices[i].Position;
		vertices[k].Normal = hay_a.Vertices[i].Normal;
		vertices[k].Tex = hay_a.Vertices[i].TexC;
	}
	for (size_t i = 0; i < hay_b.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = hay_b.Vertices[i].Position;
		vertices[k].Normal = hay_b.Vertices[i].Normal;
		vertices[k].Tex = hay_b.Vertices[i].TexC;
	}
	for (size_t i = 0; i < hay_c.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = hay_c.Vertices[i].Position;
		vertices[k].Normal = hay_c.Vertices[i].Normal;
		vertices[k].Tex = hay_c.Vertices[i].TexC;
	}
	for (size_t i = 0; i < hay_d.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = hay_d.Vertices[i].Position;
		vertices[k].Normal = hay_d.Vertices[i].Normal;
		vertices[k].Tex = hay_d.Vertices[i].TexC;
	}
	for (size_t i = 0; i < sack_a.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sack_a.Vertices[i].Position;
		vertices[k].Normal = sack_a.Vertices[i].Normal;
		vertices[k].Tex = sack_a.Vertices[i].TexC;
	}
	for (size_t i = 0; i < sack_b.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sack_b.Vertices[i].Position;
		vertices[k].Normal = sack_b.Vertices[i].Normal;
		vertices[k].Tex = sack_b.Vertices[i].TexC;
	}
	for (size_t i = 0; i < sewers_entrance.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sewers_entrance.Vertices[i].Position;
		vertices[k].Normal = sewers_entrance.Vertices[i].Normal;
		vertices[k].Tex = sewers_entrance.Vertices[i].TexC;
	}
	for (size_t i = 0; i < tent.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = tent.Vertices[i].Position;
		vertices[k].Normal = tent.Vertices[i].Normal;
		vertices[k].Tex = tent.Vertices[i].TexC;
	}
	for (size_t i = 0; i < crate.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = crate.Vertices[i].Position;
		vertices[k].Normal = crate.Vertices[i].Normal;
		vertices[k].Tex = crate.Vertices[i].TexC;
	}
	for (size_t i = 0; i < building_f.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = building_f.Vertices[i].Position;
		vertices[k].Normal = building_f.Vertices[i].Normal;
		vertices[k].Tex = building_f.Vertices[i].TexC;
	}
	for (size_t i = 0; i < barrel.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = barrel.Vertices[i].Position;
		vertices[k].Normal = barrel.Vertices[i].Normal;
		vertices[k].Tex = barrel.Vertices[i].TexC;
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
// 	indices.insert(indices.end(), house1.Indices.rbegin(), house1.Indices.rend());
// 	indices.insert(indices.end(), house2.Indices.rbegin(), house2.Indices.rend());
// 	indices.insert(indices.end(), house3.Indices.rbegin(), house3.Indices.rend());
// 	indices.insert(indices.end(), house4.Indices.rbegin(), house4.Indices.rend());
// 	indices.insert(indices.end(), house5.Indices.rbegin(), house5.Indices.rend());
// 	indices.insert(indices.end(), house6.Indices.rbegin(), house6.Indices.rend());
// 	indices.insert(indices.end(), fence.Indices.rbegin(), fence.Indices.rend());
	indices.insert(indices.end(), angelStatue.Indices.rbegin(), angelStatue.Indices.rend());
	indices.insert(indices.end(), wall.Indices.rbegin(), wall.Indices.rend());
	indices.insert(indices.end(), tower_corner.Indices.rbegin(), tower_corner.Indices.rend());
	indices.insert(indices.end(), tower_round.Indices.rbegin(), tower_round.Indices.rend());
	indices.insert(indices.end(), building_b.Indices.rbegin(), building_b.Indices.rend());
	indices.insert(indices.end(), building_c.Indices.rbegin(), building_c.Indices.rend());
	indices.insert(indices.end(), building_d.Indices.rbegin(), building_d.Indices.rend());
	indices.insert(indices.end(), building_e.Indices.rbegin(), building_e.Indices.rend());
	indices.insert(indices.end(), well.Indices.rbegin(), well.Indices.rend());
	indices.insert(indices.end(), food_a.Indices.rbegin(), food_a.Indices.rend());
	indices.insert(indices.end(), food_b.Indices.rbegin(), food_b.Indices.rend());
	indices.insert(indices.end(), hay_a.Indices.rbegin(), hay_a.Indices.rend());
	indices.insert(indices.end(), hay_b.Indices.rbegin(), hay_b.Indices.rend());
	indices.insert(indices.end(), hay_c.Indices.rbegin(), hay_c.Indices.rend());
	indices.insert(indices.end(), hay_d.Indices.rbegin(), hay_d.Indices.rend());
	indices.insert(indices.end(), sack_a.Indices.rbegin(), sack_a.Indices.rend());
	indices.insert(indices.end(), sack_b.Indices.rbegin(), sack_b.Indices.rend());
	indices.insert(indices.end(), sewers_entrance.Indices.rbegin(), sewers_entrance.Indices.rend());
	indices.insert(indices.end(), tent.Indices.rbegin(), tent.Indices.rend());
	indices.insert(indices.end(), crate.Indices.rbegin(), crate.Indices.rend());
	indices.insert(indices.end(), building_f.Indices.rbegin(), building_f.Indices.rend());
	indices.insert(indices.end(), barrel.Indices.rbegin(), barrel.Indices.rend());



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
	ifs.open("NewMapData.txt");
	XMFLOAT3 scale;
	XMFLOAT4 rotation;
	XMFLOAT3 position;

	CInstanceBasicModel tempInstanceModelWall;

	char objectName[50];
	std::string cIgnore;
	int staticObjCount;
	int instancedObjectCount;
	int skinnedObjectCount;
	ifs >> cIgnore;
	ifs >> cIgnore >> staticObjCount;
	ifs >> cIgnore >> skinnedObjectCount;
	ifs >> cIgnore >> instancedObjectCount;
	//////////////////////////////////////////////////////////////////////////
	///Reading staticObject
	ifs >> cIgnore;
	for (int i = 0; i < staticObjCount; ++i)
	{
		ifs >> objectName;
		ifs >> cIgnore >> position.x >> position.y >> position.z;
		ifs >> cIgnore >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
		ifs >> cIgnore >> scale.x >> scale.y >> scale.z;

		if (!strcmp(objectName, "Plane"))
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
				texMgr.CreateTexture(L"Textures\\PBC_Texture_RoadsPack_Pebbles_D.png"),
				texMgr.CreateTexture(L"Textures\\PBC_Texture_RoadsPack_Pebbles_NM.png"),
				"grid"
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
				mObjectMaterial,
				angelStatueIndexCount,
				angelStatueVertexOffset,
				angelStatueIndexOffset,
				texMgr.CreateTexture(L"Textures\\angelStatue.png"),
				"angelStatue"
			));
		}
		else if (!strcmp(objectName, "tower_corner"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				tower_conerIndexCount,
				tower_conerVertexOffset,
				tower_conerIndexOffset,
				texMgr.CreateTexture(L"Textures\\bricks_albedo.png"),
				"tower_coner"
			));
		}
		else if (!strcmp(objectName, "tower_round"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				tower_roundIndexCount,
				tower_roundVertexOffset,
				tower_roundIndexOffset,
				texMgr.CreateTexture(L"Textures\\bricks_albedo.png"),
				"tower_round"
			));
		}
		else if (!strcmp(objectName, "Building_b"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				building_b_IndexCount,
				building_b_VertexOffset,
				building_b_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Building_b"
			));
		}
		else if (!strcmp(objectName, "Building_c"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				building_c_IndexCount,
				building_c_VertexOffset,
				building_c_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Building_c"
			));
		}

		else if (!strcmp(objectName, "Building_d"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				building_d_IndexCount,
				building_d_VertexOffset,
				building_d_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Building_b"
			));
		}
		else if (!strcmp(objectName, "Building_e"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				building_e_IndexCount,
				building_e_VertexOffset,
				building_e_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Building_e"
			));
		}
		else if (!strcmp(objectName, "Well"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				well_IndexCount,
				well_VertexOffset,
				well_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Well"
			));
		}
		else if (!strcmp(objectName, "Food_a"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				food_a_IndexCount,
				food_a_VertexOffset,
				food_a_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Food_a"
			));
		}
		else if (!strcmp(objectName, "Food_b"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				food_b_IndexCount,
				food_b_VertexOffset,
				food_b_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Food_b"
			));
		}
		else if (!strcmp(objectName, "Hay_a"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				hay_a_IndexCount,
				hay_a_VertexOffset,
				hay_a_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Hay_a"
			));
		}
		else if (!strcmp(objectName, "Hay_b"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				hay_b_IndexCount,
				hay_b_VertexOffset,
				hay_b_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Hay_b"
			));
		}
		else if (!strcmp(objectName, "Hay_c"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				hay_c_IndexCount,
				hay_c_VertexOffset,
				hay_c_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Hay_c"
			));
		}
		else if (!strcmp(objectName, "Hay_d"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				hay_d_IndexCount,
				hay_d_VertexOffset,
				hay_d_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Hay_d"
			));
		}
		else if (!strcmp(objectName, "Sack_a"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				sack_a_IndexCount,
				sack_a_VertexOffset,
				sack_a_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Sack_a"
			));
		}
		else if (!strcmp(objectName, "Sack_b"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				sack_b_IndexCount,
				sack_b_VertexOffset,
				sack_b_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Sack_b"
			));
		}
		else if (!strcmp(objectName, "Sewers_entrance"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				sewers_entrance_IndexCount,
				sewers_entrance_VertexOffset,
				sewers_entrance_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Sewers_entrance"
			));
		}
		else if (!strcmp(objectName, "Tent"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				tent_indexCount,
				tent_VertexOffset,
				tent_indexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Tent"
			));
		}
		else if (!strcmp(objectName, "Crate"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				crate_indexCount,
				crate_VertexOffset,
				crate_indexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Crate"
			));
		}
		else if (!strcmp(objectName, "Building_f"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				building_f_IndexCount,
				building_f_VertexOffset,
				building_f_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Building_f"
			));
		}
		else if (!strcmp(objectName, "Barrel"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			mStaticBasicModels.push_back(CStaticBasicModel(M,
				mObjectMaterial,
				barrel_IndexCount,
				barrel_VertexOffset,
				barrel_IndexOffset,
				texMgr.CreateTexture(L"Textures\\Building_Texture.dds"),
				"Barrel"
			));
		}
		//////////////////////////////////////////////////////////////////////////아직 추가 안함.
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

		if (!strcmp(objectName, "wall"))
		{
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			//tempInstanceWorld.push_back(M);
			tempInstanceModelWall.AddInstanceWorld(M);

		}
	}
	//	인스턴싱할 객체를 Setting
	tempInstanceModelWall.SetDrawInfomation(wallIndexCount, wallVertexOffset, wallIndexOffset);
	tempInstanceModelWall.SetMatrial(mObjectMaterial);
	tempInstanceModelWall.SetSRV(texMgr.CreateTexture(L"Textures\\bricks_albedo.png"));
	tempInstanceModelWall.BuildInstanceBuffer(mDevice);
	mInstanceModels.push_back(tempInstanceModelWall);

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
			tempSkinnedModelInstanced.selectedDiffuseMapIndex = rand() % 3;
			ifs >> objectName;
			ifs >> cIgnore >> position.x >> position.y >> position.z;

			tempSkinnedModelInstanced.camPos = XMFLOAT3(position.x, position.y, position.z);
			mSkinnedModelInstance.push_back(tempSkinnedModelInstanced);

			cam.SetPosition(position);

		}
	}
	// 	int randomIndex = rand() % mSkinnedModelInstance.size();
	// 	cam->SetPosition(mSkinnedModelInstance[randomIndex].camPos);
	// 	mSkinnedModelInstance[randomIndex].isPlayer = true;
	// 	mSkinnedModelInstance[randomIndex].cam = cam;

	ifs.close();

}

