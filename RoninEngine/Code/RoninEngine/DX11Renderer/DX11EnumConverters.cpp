#include "DX11RendererPCH.h"

#include "DX11EnumConverters.h"

//////////////////////////////////////////////////////////////////////////

const char** CDX11EnumConverters::ms_pacSemanticNames = 0;

//////////////////////////////////////////////////////////////////////////

void CDX11EnumConverters::InitData()
{
	ms_pacSemanticNames = (LPCTSTR*) IntAlloc(LPCTSTR,3);
	ms_pacSemanticNames[0] = "POSITION";
	ms_pacSemanticNames[1] = "TEXCOORD";
	ms_pacSemanticNames[2] = "NORMAL";
}

void CDX11EnumConverters::ReleaseData()
{
	IntFree(ms_pacSemanticNames);
}

//////////////////////////////////////////////////////////////////////////

LPCTSTR CDX11EnumConverters::GetSemanticName(
	CMeshDataEntryDescriptor::EMeshDataSemantic eSemantic)
{
	switch (eSemantic) 
	{
	case CMeshDataEntryDescriptor::SEMANTIC_POSITION:
		{
			return ms_pacSemanticNames[0];
		}

	case CMeshDataEntryDescriptor::SEMANTIC_TEXCOORD:
		{
			return ms_pacSemanticNames[1];
		}
	case CMeshDataEntryDescriptor::SEMANTIC_NORMAL:
		{
			return ms_pacSemanticNames[2];
		}
	}

	return 0;
}

DXGI_FORMAT CDX11EnumConverters::GetFormat(
	CMeshDataEntryDescriptor::EMeshDataEntryFormat eFormat)
{
	switch (eFormat)
	{
	case CMeshDataEntryDescriptor::FORMAT_UINT_16_X:
		return DXGI_FORMAT_R16_UINT;
	case CMeshDataEntryDescriptor::FORMAT_UINT_32_X:
		return DXGI_FORMAT_R32_UINT;
	case CMeshDataEntryDescriptor::FORMAT_FLOAT_16_XYZ:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XYZ:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case CMeshDataEntryDescriptor::FORMAT_FLOAT_16_XY:
		return DXGI_FORMAT_R16G16_FLOAT;
	case CMeshDataEntryDescriptor::FORMAT_FLOAT_32_XY:
		return DXGI_FORMAT_R32G32_FLOAT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CDX11EnumConverters::GetTextureCreateFormat(
	CTexture::EFormat eFormat)
{
	switch (eFormat)
	{
	case CTexture::FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_TYPELESS;
	case CTexture::FORMAT_R16G16_FLOAT:
	case CTexture::FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_TYPELESS;
	case CTexture::FORMAT_R24_UNORM_S8_UINT:
		return DXGI_FORMAT_R24G8_TYPELESS;
	case CTexture::FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	}

	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CDX11EnumConverters::GetShaderResourceViewFormat(
	CTexture::EFormat eFormat)
{
	switch (eFormat)
	{
	case CTexture::FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case CTexture::FORMAT_R16G16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case CTexture::FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case CTexture::FORMAT_R24_UNORM_S8_UINT:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case CTexture::FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CDX11EnumConverters::GetRenderTargetViewFormat(
	CTexture::EFormat eFormat)
{
	switch (eFormat)
	{
	case CTexture::FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case CTexture::FORMAT_R16G16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case CTexture::FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case CTexture::FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT CDX11EnumConverters::GetDepthStencilViewFormat(
	CTexture::EFormat eFormat)
{
	switch (eFormat)
	{
	case CTexture::FORMAT_R32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
	case CTexture::FORMAT_R24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

D3D11_INPUT_CLASSIFICATION CDX11EnumConverters::GetInputClassification(
	CMeshDataEntry::EMeshDataInputType eInputType)
{
	switch (eInputType) 
	{
	case CMeshDataEntry::TYPE_VERTEX_DATA: 
		{
			return D3D11_INPUT_PER_VERTEX_DATA;
		}

	}

	return D3D11_INPUT_PER_VERTEX_DATA; 
}

D3D11_PRIMITIVE_TOPOLOGY CDX11EnumConverters::GetPrimitiveToplogy(
	CMeshData::ETopology eTopology)
{
	switch (eTopology)
	{
	case CMeshData::TOPOLOGY_TRIANGLE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	case CMeshData::TOPOLOGY_TRIANGLE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		
	}

	return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

//////////////////////////////////////////////////////////////////////////