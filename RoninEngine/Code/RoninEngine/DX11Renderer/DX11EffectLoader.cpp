#include "DX11RendererPCH.h"

#include "DX11EffectLoader.h"

#include "DX11EffectData.h"
#include "DX11EffectMaterial.h"

#include "DX11ConstantBufferDesc.h"
#include "DX11ConstantBuffer.h"

#include <Mesh.h>
#include <MeshData.h>
#include <MeshRenderData.h>

#include <MaterialManager.h>

#include <FileSystem.h>

//////////////////////////////////////////////////////////////////////////

CDX11EffectManager::CDX11EffectManager()
{
	// set the default shader flags
	m_dwShaderFlags = D3D10_SHADER_OPTIMIZATION_LEVEL0;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	m_dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	// make sure we support backwards compatibilty
	m_dwShaderFlags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

	// register this loader as listener for file change events on .fx files
	TStringVector extensions;
	extensions.push_back("fx");
	FILE_SYSTEM->RegisterFileExtensionChangeListener(this, extensions);
}

CDX11EffectManager::~CDX11EffectManager()
{
	Clear();
}

void CDX11EffectManager::Clear()
{
#ifdef _DEBUG
	for (TEffectDataMap::iterator kIter = m_mEffectData.begin();
		kIter != m_mEffectData.end(); ++kIter)
	{
		CDX11EffectData* pEffectData = kIter->second;
		assert(pEffectData->GetRefCount() == 1);
	}
#endif

	m_mEffectData.clear();
}

//////////////////////////////////////////////////////////////////////////

CDX11EffectMaterial* CDX11EffectManager::CreateEffectMaterial(
	const stl_string& sTechniqueName)
{
	CDX11EffectData* pkEffectData = GetEffectData(sTechniqueName);
	if (pkEffectData == 0)
		return 0;

	CDX11EffectMaterial* pkEffectMaterial = 
		IntNew CDX11EffectMaterial(pkEffectData->GetName());
	pkEffectMaterial->SetEffectData(pkEffectData);

	return pkEffectMaterial;
}

CDX11EffectData* CDX11EffectManager::GetEffectData(
	const stl_string& sTechniqueName)
{
	// retrieve effect data by name
	TEffectDataMap::iterator pkIter = m_mEffectData.find(sTechniqueName);
	if (pkIter == m_mEffectData.end())
		return 0;

	return pkIter->second;
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectManager::ProcessFileChange(
	const SFileChangeInfo& fileChangeInfo)
{
	char acTmp[512];
	sprintf_s(acTmp, 512, "DX11EffectManager should reload shader: %s \n", 
		fileChangeInfo.m_sFilename.c_str());
	OutputDebugString(acTmp);
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectManager::LoadEffectsFromFolder(
	ID3D11Device* pkDevice, const char* acFolder)
{
	// find all fx files in given folder
	m_sEffectPath = acFolder;

	stl_string sPath(acFolder);
	sPath += "*.fx";

	CFileSystem* pFileSystem = FILE_SYSTEM;

	CFileSystem::SFindData findData;
	CFileSystem::TFindDataHandle hFindHandle = 
		pFileSystem->FindFirstFile(sPath, findData);

	if (hFindHandle != CFileSystem::ms_nInvalidFileHandle)
	{
		bool bRes = true;
		while (bRes)
		{
			stl_string sFileName(acFolder);
			sFileName += findData.m_sPath;

			LoadEffectFile(pkDevice, sFileName);			

			bRes = pFileSystem->FindNextFile(hFindHandle, findData);
		}
	}

	pFileSystem->FindClose(hFindHandle);
}

void CDX11EffectManager::ReloadEffects(ID3D11Device* pkDevice)
{
	// release all the effect data on the loaded materials
	CMaterialManager* pMaterialManager = CCoreEngine::Get()->GetMaterialManager();
	CMaterialManager::TMaterialMap& mMaterials = pMaterialManager->GetMaterials();
	for (CMaterialManager::TMaterialMap::iterator it = mMaterials.begin(); 
		it != mMaterials.end(); ++it)
	{
		CDX11EffectMaterial* pEffectMaterial = SmartPointerCast(CDX11EffectMaterial, it->second);
		pEffectMaterial->ReleaseEffectData();
	}

	// clear the local effect data
	Clear();

	// reload the effects from disc
	LoadEffectsFromFolder(pkDevice, m_sEffectPath.c_str());

	// reset the effect data on the materials
	for (CMaterialManager::TMaterialMap::iterator it = mMaterials.begin(); 
		it != mMaterials.end(); ++it)
	{
		CDX11EffectMaterial* pEffectMaterial = SmartPointerCast(CDX11EffectMaterial, it->second);
		
		CDX11EffectData* pkEffectData = GetEffectData(pEffectMaterial->GetName());
		assert(pkEffectData);
		if (pkEffectData)
			pEffectMaterial->SetEffectData(pkEffectData);
	}
}

bool CDX11EffectManager::LoadEffectFile(
	ID3D11Device* pkDevice, const stl_string& sFileName)
{
	ID3DBlob* pkShader = NULL;
	ID3DBlob* pkErrors = NULL;

	CFileSystem* pFileSystem = CCoreEngine::Get()->GetFileSystem();
	CFile* pFile = pFileSystem->OpenFile(sFileName, CFile::eOpenMode_Read);
	if (pFile == 0)
		return false;

	size_t stSize = pFile->GetSize();
	char* acShaderData = IntAlloc(char, stSize);
	if (pFile->ReadData(acShaderData, stSize) != stSize)
	{
		IntFree(acShaderData);
		pFileSystem->CloseFile(pFile);
		return false;
	}

	HRESULT hr = D3DX11CompileFromMemory( 
		acShaderData, stSize, sFileName.c_str(), NULL, NULL, "", "fx_5_0", 
		m_dwShaderFlags, 0, NULL, &pkShader, &pkErrors, NULL );
	//HRESULT hr = D3DX11CompileFromFile( 
	//	sFileName.c_str(), NULL, NULL, "", "fx_5_0", m_dwShaderFlags, 
	//	0, NULL, &pkShader, &pkErrors, NULL );
	if( FAILED( hr ) || pkShader == 0 )
	{
		if( pkErrors )
		{
			char* acError = (char*) pkErrors->GetBufferPointer();
			OutputDebugString(acError);
		}

		SAFE_RELEASE(pkShader);
		SAFE_RELEASE(pkErrors);

		IntFree(acShaderData);
		pFileSystem->CloseFile(pFile);

		return false;
	}
	
	IntFree(acShaderData);

	ID3DX11Effect* pkEffect = 0;

	hr = D3DX11CreateEffectFromMemory(pkShader->GetBufferPointer(), 
		pkShader->GetBufferSize(), 0, pkDevice, &pkEffect);
	if( FAILED( hr ) || pkEffect == 0 )
	{
		SAFE_RELEASE(pkShader);
		SAFE_RELEASE(pkErrors);
		SAFE_RELEASE(pkEffect);

		pFileSystem->CloseFile(pFile);

		return false;
	}

	D3DX11_EFFECT_DESC kEffectDesc;
	pkEffect->GetDesc(&kEffectDesc);

	for (unsigned short us = 0; us < kEffectDesc.GlobalVariables; ++us)
	{
		ID3DX11EffectVariable* pkID3DX11ConstantBuffer = 
			pkEffect->GetVariableByIndex(us);

		int i = 0;
		//m_vTechniques.push_back(pkTechnique);
	}

	for (unsigned short us = 0; us < kEffectDesc.Techniques; ++us)
	{
		ID3DX11EffectTechnique* pkID3D11Technique = 
			pkEffect->GetTechniqueByIndex(us);

		CDX11EffectData* pkEntry = IntNew CDX11EffectData(
			pkEffect, pkID3D11Technique);

		ParseConstantBuffers(pkEffect, pkEntry);	
	
		m_mEffectData[pkEntry->GetName()] = pkEntry;
	}

	SAFE_RELEASE(pkShader);
	SAFE_RELEASE(pkErrors);
	SAFE_RELEASE(pkEffect);

	pFileSystem->CloseFile(pFile);

	return true;
}

void CDX11EffectManager::ParseConstantBuffers(
	ID3DX11Effect* pkEffect, CDX11EffectData* pkEffectData)
{
	if (pkEffect == 0) return;

	D3DX11_EFFECT_DESC kEffectDesc;
	pkEffect->GetDesc(&kEffectDesc);

	ID3DX11EffectConstantBuffer* pkGlobalConstantBuffer = 0;
	CDX11ConstantBufferDesc* pkGlobalConstantBufferDesc = 0;
	typedef stl_map<ID3DX11EffectConstantBuffer*, 
		CDX11ConstantBufferDesc*> TConstantBufferMap;
	TConstantBufferMap kConstantBufferMap;

	for (unsigned short us = 0; us < kEffectDesc.ConstantBuffers; ++us)
	{
		ID3DX11EffectConstantBuffer* pkID3DX11ConstantBuffer = 
			pkEffect->GetConstantBufferByIndex(us);

		ID3DX11EffectType* pkID3DX11Type = pkID3DX11ConstantBuffer->GetType();

		D3DX11_EFFECT_TYPE_DESC kEffectTypeDesc;
		pkID3DX11Type->GetDesc(&kEffectTypeDesc);

		ID3D11Buffer* d3dConstantBuffer;
		pkID3DX11ConstantBuffer->GetConstantBuffer(&d3dConstantBuffer);

		D3D11_BUFFER_DESC kBufferDesc;
		d3dConstantBuffer->GetDesc(&kBufferDesc);

		D3DX11_EFFECT_VARIABLE_DESC kEffectVariableDesc;
		pkID3DX11ConstantBuffer->GetDesc(&kEffectVariableDesc);

		CDX11ConstantBufferDesc* pkConstantBufferDesc = 
			IntNew CDX11ConstantBufferDesc();
		pkConstantBufferDesc->SetName(kEffectVariableDesc.Name);

		kConstantBufferMap[pkID3DX11ConstantBuffer] = pkConstantBufferDesc;

		SAFE_RELEASE(d3dConstantBuffer);
	}

	for (unsigned short us = 0; us < kEffectDesc.GlobalVariables; ++us)
	{
		ID3DX11EffectVariable* pkID3DX11EffectVariable = 
			pkEffect->GetVariableByIndex(us);

		ID3DX11EffectType* pkID3DX11Type = pkID3DX11EffectVariable->GetType();

		D3DX11_EFFECT_TYPE_DESC kEffectTypeDesc;
		pkID3DX11Type->GetDesc(&kEffectTypeDesc);

		D3DX11_EFFECT_VARIABLE_DESC kEffectVariableDesc;
		pkID3DX11EffectVariable->GetDesc(&kEffectVariableDesc);

		if (kEffectTypeDesc.Type == D3D10_SVT_SAMPLER)
		{
			//ID3DX11EffectSamplerVariable* pkSampler = 
			//	pkID3DX11EffectVariable->AsSampler();

		} else if (kEffectTypeDesc.Type == D3D10_SVT_TEXTURE2D)
		{
			ID3DX11EffectShaderResourceVariable* pkShaderResource = 
				pkID3DX11EffectVariable->AsShaderResource();
			unsigned int uiTextureSlot = GetTextureSlot(
				kEffectVariableDesc.Semantic);

			if (uiTextureSlot < MAX_TEXTURE_SLOTS)
			{
				pkEffectData->AddShaderResource(uiTextureSlot, pkShaderResource);
			}
		} else {

			CShaderData::EShaderDataType eType = GetShaderDataType(&kEffectTypeDesc);
			if (eType == CShaderData::TYPE_INVALID)
				continue;

			CDX11ConstantBufferDesc::SVariableDesc* pkVariableDesc = 
				IntNew CDX11ConstantBufferDesc::SVariableDesc;
			//pkVariableDesc->m_eType = eType;
			pkVariableDesc->m_sName = kEffectVariableDesc.Name;
			pkVariableDesc->m_uiOffset = 0; // will be computed later
			pkVariableDesc->m_uiCount = kEffectTypeDesc.Rows * 
				kEffectTypeDesc.Columns;
			pkVariableDesc->m_uiSize = kEffectTypeDesc.PackedSize;
			pkVariableDesc->m_ePredefinedType = GetPredefinedType(
				kEffectVariableDesc.Semantic);
			if (pkVariableDesc->m_ePredefinedType != CDX11ConstantBufferDesc::PREDEFINED_INVALID)
				pkVariableDesc->m_bPredefined = true;

			// find out to which constant buffer this variable is mapped
			ID3DX11EffectConstantBuffer* pkID3DX11ConstantBuffer =
				pkID3DX11EffectVariable->GetParentConstantBuffer();

			CDX11ConstantBufferDesc* pkConstantBufferDesc = 0;

			// is this variable assigned to a constant buffer
			if (pkID3DX11ConstantBuffer)
			{	
				// do we already have some info on this constant buffer
				TConstantBufferMap::iterator kFound = kConstantBufferMap.find(
					pkID3DX11ConstantBuffer);
				if (kFound == kConstantBufferMap.end())
				{
					if (pkGlobalConstantBufferDesc == 0)
					{
						pkGlobalConstantBuffer = pkID3DX11ConstantBuffer;
						pkGlobalConstantBufferDesc = IntNew CDX11ConstantBufferDesc();
						pkGlobalConstantBufferDesc->SetName("Global");
					}
					pkConstantBufferDesc = pkGlobalConstantBufferDesc;
				} else
					pkConstantBufferDesc = kFound->second;
			} else 
			{
				// this shouldn't be possible!
				int i = 0;
			}

			if (pkConstantBufferDesc)
				pkConstantBufferDesc->AddVariable(pkVariableDesc);
		}		
	}	

	// compute the offset of each variable into the contant buffer 
	for (TConstantBufferMap::iterator kIter = kConstantBufferMap.begin(); 
		kIter != kConstantBufferMap.end(); ++kIter)
	{
		CDX11ConstantBufferDesc* pkDesc = kIter->second;
		unsigned int uiOffset = 0;
		
		CDX11ConstantBufferDesc::TVariableDescList& vVariables = 
			pkDesc->GetVariableDescList();
		for (unsigned short us = 0; us < vVariables.size(); ++us)
		{
			CDX11ConstantBufferDesc::SVariableDesc* pkVariable = vVariables[us];
			pkVariable->m_uiOffset = uiOffset;

			uiOffset += pkVariable->m_uiSize;
		}

		ID3DX11EffectConstantBuffer* pkID3DX11ConstantBuffer = kIter->first;
		ID3D11Buffer* d3dConstantBuffer;
		pkID3DX11ConstantBuffer->GetConstantBuffer(&d3dConstantBuffer);

		pkEffectData->AddConstantBuffer(
			IntNew CDX11ConstantBuffer(d3dConstantBuffer, pkDesc));
	}

	// check for shared constant buffers which we can remap
}

CShaderData::EShaderDataType CDX11EffectManager::GetShaderDataType(
	D3DX11_EFFECT_TYPE_DESC* pkEffectTypeDesc)
{
	if (pkEffectTypeDesc == 0)
		return CShaderData::TYPE_INVALID;

	switch (pkEffectTypeDesc->Type)
	{
	case D3D10_SVT_FLOAT : return CShaderData::TYPE_FLOAT;
	case D3D10_SVT_INT : return CShaderData::TYPE_INT;
	}

	return CShaderData::TYPE_INVALID;
}

unsigned int CDX11EffectManager::GetTextureSlot(LPCTSTR sSemantic)
{
	if (sSemantic == 0)
		return unsigned int(-1);

	if (strcmp(sSemantic,"SLOT_0") == 0)
		return 0;
	else if (strcmp(sSemantic,"SLOT_1") == 0)
		return 1;
	else if (strcmp(sSemantic,"SLOT_2") == 0)
		return 2;
	else if (strcmp(sSemantic,"SLOT_3") == 0)
		return 3;

	return unsigned int(-1);
}

CDX11ConstantBufferDesc::EShaderDataPredefined CDX11EffectManager::GetPredefinedType(
	LPCTSTR sSemantic)
{
	if (sSemantic == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_INVALID;

	if (strcmp(sSemantic,"WORLDVIEWPROJECTION") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_WORLDVIEWPROJECTION;
	else if (strcmp(sSemantic,"VIEWPROJECTION") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_VIEWPROJECTION;
	else if (strcmp(sSemantic,"WORLDVIEW") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_WORLDVIEW;
	else if (strcmp(sSemantic,"PROJECTION") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_PROJECTION;
	else if (strcmp(sSemantic,"VIEW") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_VIEW;
	else if (strcmp(sSemantic,"WORLD") == 0)
		return CDX11ConstantBufferDesc::PREDEFINED_WORLD;

	return CDX11ConstantBufferDesc::PREDEFINED_INVALID;
}

//////////////////////////////////////////////////////////////////////////