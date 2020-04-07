#ifndef MESHGEOMETRY_H
#define MESHGEOMETRY_H

#include "Common/d3dUtil.h"
#include "Vertex.h"
class MeshGeometry
{
public:
	struct Subset
	{
		Subset() : 
	      Id(-1), 
			VertexStart(0), VertexCount(0),
			FaceStart(0), FaceCount(0)
		{
		}

		UINT Id;
		UINT VertexStart;
		UINT VertexCount;
		UINT FaceStart;
		UINT FaceCount;
	};

public:
    MeshGeometry();
	~MeshGeometry();

	template <typename VertexType>
	void SetVertices(ID3D11Device* device, const VertexType* vertices, UINT count, bool isPosTanTexNormal);
	
	void SetIndices(ID3D11Device* device, const UINT* indices, UINT count);

	void SetSubsetTable(std::vector<Subset>& subsetTable);

	void Draw(ID3D11DeviceContext* dc, UINT subsetId);

private:
	MeshGeometry(const MeshGeometry& rhs);
	MeshGeometry& operator=(const MeshGeometry& rhs);

private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	DXGI_FORMAT mIndexBufferFormat; // Always 16-bit
	UINT mVertexStride;

	std::vector<Subset> mSubsetTable;
};

template <typename VertexType>
void MeshGeometry::SetVertices(ID3D11Device* device, const VertexType* vertices, UINT count, bool isPosTanTexNormal)
{
	ReleaseCOM(mVB);
	if(!isPosTanTexNormal)
	mVertexStride = sizeof(Vertex::PosNormalTexTanSkinned);
	else
	{
		mVertexStride = sizeof(Vertex::PosNormalTexTan);
	}
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	if(!isPosTanTexNormal)
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTanSkinned) * count;
	else
	{
		vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * count;
	}
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    HR(device->CreateBuffer(&vbd, &vinitData, &mVB));
}


#endif // MESHGEOMETRY_H