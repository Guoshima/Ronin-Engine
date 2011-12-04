#pragma once

#include "DX11Material.h"

class CDX11EffectData;
CoreSmartPointer(CDX11EffectData);

class CDX11EffectMaterial : public CDX11Material
{
public:

	CDX11EffectMaterial(
		const stl_string& sMaterialName);
	~CDX11EffectMaterial();

	void SetEffectData(CDX11EffectData* pkEffectData);
	void ReleaseEffectData();

	//////////////////////////////////////////////////////////////////////////
	// Implementation of DX11 Material
	//////////////////////////////////////////////////////////////////////////

	virtual unsigned int GetPassCount();
	virtual void ApplyPass(unsigned int uiPassID, 
		ID3D11DeviceContext* pkDeviceContext);

	virtual const void* GetInputSignatureData(unsigned int& uiSize);

	virtual void ApplyTextures(CDX11Renderer* pkRenderer, 
		CMesh* pkMesh);

private:

	//////////////////////////////////////////////////////////////////////////

	CDX11EffectDataPtr m_spEffectData;

};