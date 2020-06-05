#include "COpenFBXWrraper.h"
#include<cstdio>

COpenFBXWrraper::COpenFBXWrraper()
	:m_pExporter(new FBXExporter)
{
	
}

COpenFBXWrraper::COpenFBXWrraper(COpenFBXWrraper& other)
{
}

COpenFBXWrraper::~COpenFBXWrraper()
{
	SAFE_DELETE(m_pExporter);
}

bool COpenFBXWrraper::LoadFBXFile(const char* fbxPath)
{
	if (m_pExporter->GetisLoaded())
	{
		m_pExporter->Shutdown();
	}
	m_pExporter->Initialize();
	m_pExporter->LoadScene(fbxPath, "");
	m_pExporter->ProcessFBX();
	/*if (m_pScene != nullptr)
	{
		m_pScene->destroy();
		m_pScene = nullptr;
	}

	FILE* fp = fopen(fbxPath, "rb");
	if (!fp) return false;

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);
	m_pScene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
	if (!m_pScene) 
		return false;

	delete[] content;
	fclose(fp);
	*/
	return true;
}

bool COpenFBXWrraper::SaveAsOBJ(const char* path)
{
	/*FILE* fp = fopen(path, "wb");
	if (!fp) return false;
	int obj_idx = 0;
	int indices_offset = 0;
	int normals_offset = 0;
	int mesh_count = m_pScene->getMeshCount();
	for (int i = 0; i < mesh_count; ++i)
	{
		fprintf(fp, "o obj%d\ng grp%d\n", i, obj_idx);

		const ofbx::Mesh& mesh = *m_pScene->getMesh(i);
		const ofbx::Geometry& geom = *mesh.getGeometry();
		int vertex_count = geom.getVertexCount();
		const ofbx::Vec3* vertices = geom.getVertices();
		for (int i = 0; i < vertex_count; ++i)
		{
			ofbx::Vec3 v = vertices[i];
			fprintf(fp, "v %f %f %f\n", v.x, v.y, v.z);
		}

		bool has_normals = geom.getNormals() != nullptr;
		if (has_normals)
		{
			const ofbx::Vec3* normals = geom.getNormals();
			int count = geom.getIndexCount();

			for (int i = 0; i < count; ++i)
			{
				ofbx::Vec3 n = normals[i];
				fprintf(fp, "vn %f %f %f\n", n.x, n.y, n.z);
			}
		}

		bool has_uvs = geom.getUVs() != nullptr;
		if (has_uvs)
		{
			const ofbx::Vec2* uvs = geom.getUVs();
			int count = geom.getIndexCount();

			for (int i = 0; i < count; ++i)
			{
				ofbx::Vec2 uv = uvs[i];
				fprintf(fp, "vt %f %f\n", uv.x, uv.y);
			}
		}

		const int* faceIndices = geom.getFaceIndices();
		int index_count = geom.getIndexCount();
		bool new_face = true;
		for (int i = 0; i < index_count; ++i)
		{
			if (new_face)
			{
				fputs("f ", fp);
				new_face = false;
			}
			int idx = (faceIndices[i] < 0) ? -faceIndices[i] : (faceIndices[i] + 1);
			int vertex_idx = indices_offset + idx;
			fprintf(fp, "%d", vertex_idx);

			if (has_uvs)
			{
				int uv_idx = normals_offset + i + 1;
				fprintf(fp, "/%d", uv_idx);
			}
			else
			{
				fprintf(fp, "/");
			}

			if (has_normals)
			{
				int normal_idx = normals_offset + i + 1;
				fprintf(fp, "/%d", normal_idx);
			}
			else
			{
				fprintf(fp, "/");
			}

			new_face = faceIndices[i] < 0;
			fputc(new_face ? '\n' : ' ', fp);
		}

		indices_offset += vertex_count;
		normals_offset += index_count;
		++obj_idx;
	}
	fclose(fp);
	*/
	return true;
}

bool COpenFBXWrraper::GetVertices(std::vector<CModelClass::VertexType>& container)
{
	return m_pExporter->GetVertex(container);
}

bool COpenFBXWrraper::GetIndices(std::vector<int>& container)
{
	return m_pExporter->GetIndices(container);
}

std::string COpenFBXWrraper::GetFBXInfo()
{
	
	return std::string();
}
