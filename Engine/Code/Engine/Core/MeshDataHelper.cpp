#include "CorePCH.h"

#include "MeshDataHelper.h"

#include "MeshData.h"
#include "MeshDataEntry.h"
#include "MeshDataEntryDescriptor.h"

//////////////////////////////////////////////////////////////////////////

CMeshData* CMeshDataHelper::CreateScreenSpaceMeshData()
{
	// create the vertex buffer data
	stl_vector<CMeshDataEntryDescriptorPtr> vVertexDataDescriptors;
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_POSITION, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ));
	vVertexDataDescriptors.push_back(IntNew CMeshDataEntryDescriptor(
		CMeshDataEntryDescriptor::SEMANTIC_TEXCOORD, 0,
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XY));

	unsigned int uiSizeOfPos = CMeshDataEntryDescriptor::GetSizeOfFormat(
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ);
	unsigned int uiSizeOfTC = CMeshDataEntryDescriptor::GetSizeOfFormat(
		CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XY);

	unsigned int uiSizeOfVertex = uiSizeOfPos + uiSizeOfTC;

	CMeshDataEntry* pkVertexBufferEntry = CMeshDataEntry::Create(
		vVertexDataDescriptors, 4, CMeshDataEntry::TYPE_VERTEX_DATA);

	const char* pkVertexData = (const char*) pkVertexBufferEntry->LockData();
	D3DXVECTOR3* pkVertices = (D3DXVECTOR3*) pkVertexData;
	D3DXVECTOR2* pkTextures = (D3DXVECTOR2*) (pkVertexData + uiSizeOfPos);

	pkVertices[0] = D3DXVECTOR3(-1,-1,1);
	pkTextures[0] = D3DXVECTOR2(0,1);

	pkVertexData += uiSizeOfVertex;
	pkVertices = (D3DXVECTOR3*) pkVertexData;
	pkTextures = (D3DXVECTOR2*) (pkVertexData + uiSizeOfPos);

	pkVertices[0] = D3DXVECTOR3(-1,1,1);
	pkTextures[0] = D3DXVECTOR2(0,0);

	pkVertexData += uiSizeOfVertex;
	pkVertices = (D3DXVECTOR3*) pkVertexData;
	pkTextures = (D3DXVECTOR2*) (pkVertexData + uiSizeOfPos);

	pkVertices[0] = D3DXVECTOR3(1,-1,1);
	pkTextures[0] = D3DXVECTOR2(1,1);

	pkVertexData += uiSizeOfVertex;
	pkVertices = (D3DXVECTOR3*) pkVertexData;
	pkTextures = (D3DXVECTOR2*) (pkVertexData + uiSizeOfPos);

	pkVertices[0] = D3DXVECTOR3(1,1,1);
	pkTextures[0] = D3DXVECTOR2(1,0);	

	pkVertexBufferEntry->UnLockData();

	// no index buffer required

	// create the mesh data itself
	CMeshData* pkMeshData = IntNew CMeshData(
		0, pkVertexBufferEntry);
	pkMeshData->SetTopology(CMeshData::TOPOLOGY_TRIANGLE_STRIP);

	return pkMeshData;
}