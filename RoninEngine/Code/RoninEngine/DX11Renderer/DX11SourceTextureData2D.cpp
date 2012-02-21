#include "DX11RendererPCH.h"

#include "DX11SourceTextureData2D.h"

#include <FileSystem.h>

//////////////////////////////////////////////////////////////////////////

CDX11SourceTextureData2D::CDX11SourceTextureData2D(
	const SSourceTextureCreateParams& kParams) : m_kCreateParams(kParams)
{
	
}

CDX11SourceTextureData2D::~CDX11SourceTextureData2D()
{

}

//////////////////////////////////////////////////////////////////////////

bool CDX11SourceTextureData2D::CreateD3DData(ID3D11Device* pkDevice)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC desc;

	CFileSystem* pFileSystem = CCoreEngine::Get()->GetFileSystem();
	CFile* pFile = pFileSystem->OpenFile(m_kCreateParams.m_sFileName, CFile::eOpenMode_Read);
	if (pFile == 0)
		return false;

	size_t stSize = pFile->GetSize();
	char* acData = IntAlloc(char, stSize);
	if (pFile->ReadData(acData, stSize) != stSize)
	{
		IntFree(acData);
		pFileSystem->CloseFile(pFile);
		return false;
	}

	// Load the texture and initialize an ID3D11Texture2D object.
	D3DX11CreateTextureFromMemory( pkDevice, acData, stSize, 
			NULL, NULL, (ID3D11Resource**) &m_d3dTexture2D, NULL );

	// free the temporary read data and close the file again
	IntFree(acData);
	pFileSystem->CloseFile(pFile);

	// Get a texture description to determine the texture
	// format of the loaded texture.
	m_d3dTexture2D->GetDesc( &desc );

	// Fill in the D3D11_SHADER_RESOURCE_VIEW_DESC structure.
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	// Create the shader resource view.
	pkDevice->CreateShaderResourceView(m_d3dTexture2D, 
		&srvDesc, &m_d3dShaderResourceView );

	return true;
}

//////////////////////////////////////////////////////////////////////////