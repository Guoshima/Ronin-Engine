#pragma once

#include <CoreMemObject.h>

#include <MeshData.h>
#include <MeshDataEntry.h>
#include <MeshDataEntryDescriptor.h>

#include <Texture.h>

class CDX11EnumConverters : public CCoreMemObject
{
public:

	static void InitData();
	static void ReleaseData();

	static LPCTSTR GetSemanticName(
		CMeshDataEntryDescriptor::EMeshDataSemantic eSemantic);

	static DXGI_FORMAT GetFormat(
		CMeshDataEntryDescriptor::EMeshDataEntryFormat eFormat);

	static DXGI_FORMAT GetTextureCreateFormat(
		CTexture::EFormat eFormat);
	static DXGI_FORMAT GetShaderResourceViewFormat(
		CTexture::EFormat eFormat);
	static DXGI_FORMAT GetRenderTargetViewFormat(
		CTexture::EFormat eFormat);
	static DXGI_FORMAT GetDepthStencilViewFormat(
		CTexture::EFormat eFormat);

	static D3D11_INPUT_CLASSIFICATION GetInputClassification(
		CMeshDataEntry::EMeshDataInputType eInputType);

	static D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveToplogy(
		CMeshData::ETopology eTopology);

protected:

	static LPCTSTR* ms_pacSemanticNames;
};