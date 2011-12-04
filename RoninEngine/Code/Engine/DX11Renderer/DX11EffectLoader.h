#pragma once

#include "DX11EffectData.h"

#include <ShaderData.h>
#include "DX11ConstantBufferDesc.h"

#include <FileChangeInfo.h>

class CDX11EffectMaterial;

class CDX11EffectManager : public CCoreMemObject, IFileChangeListener
{
public:

	CDX11EffectManager();
	~CDX11EffectManager();

	void Clear();

	CDX11EffectMaterial* CreateEffectMaterial(
		const stl_string& sTechniqueName);

	void SetShaderFlags(DWORD dwShaderFlags) { m_dwShaderFlags = dwShaderFlags; }
	DWORD GetShaderFlags() const { return m_dwShaderFlags; }

	void LoadEffectsFromFolder(
		ID3D11Device* pkDevice, 
		const char* acFolder);
	void ReloadEffects(ID3D11Device* pkDevice);

	unsigned int GetNumOfEffects() { return m_mEffectData.size(); }

	//////////////////////////////////////////////////////////////////////////
	//	Implementation of IFileChangeListener
	//////////////////////////////////////////////////////////////////////////

	virtual void ProcessFileChange(const SFileChangeInfo& fileChangeInfo);

private:

	bool LoadEffectFile(ID3D11Device* pkDevice,
		const stl_string& sFileName);
	void ParseConstantBuffers(ID3DX11Effect* pkEffect, 
		CDX11EffectData* pkEntry);

	CDX11EffectData* GetEffectData(const stl_string& sTechniqueName);

	CShaderData::EShaderDataType GetShaderDataType(
		D3DX11_EFFECT_TYPE_DESC* pkEffectTypeDesc);
	CDX11ConstantBufferDesc::EShaderDataPredefined GetPredefinedType(
		LPCTSTR sSemantic);
	unsigned int GetTextureSlot(LPCTSTR sSemantic);

	//////////////////////////////////////////////////////////////////////////

	DWORD m_dwShaderFlags;

	stl_string m_sEffectPath;

	typedef stl_map<stl_string, CDX11EffectDataPtr> TEffectDataMap;
	TEffectDataMap m_mEffectData;
};