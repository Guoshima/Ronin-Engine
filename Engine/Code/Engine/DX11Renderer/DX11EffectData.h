#pragma once

#include <CoreMemObject.h>

#include <d3dx11effect.h>

class CDX11ConstantBuffer;
CoreSmartPointer(CDX11ConstantBuffer);

class CDX11EffectData : public CCoreRefSTObject
{
public:

	CDX11EffectData(ID3DX11Effect* d3dEffect,
		ID3DX11EffectTechnique* d3dTechnique);
	~CDX11EffectData();

	const stl_string& GetName() const { return m_sName; }

	ID3DX11Effect* GetEffect() { return m_d3dEffect; }
	ID3DX11EffectTechnique* GetTechnique() { return m_d3dTechnique; }

	unsigned short GetPassCount() { return m_d3dPasses.size(); }
	ID3DX11EffectPass* GetPass(unsigned short us) { return m_d3dPasses[us]; }

	const void* GetInputSignatureData(unsigned int& uiSize);

	void AddConstantBuffer(CDX11ConstantBuffer* pkCB);
	const stl_vector<CDX11ConstantBufferPtr>& GetConstanBufferList() {
		return m_vConstantBufferList;
	}

	void AddShaderResource(unsigned int uiSlot, 
		ID3DX11EffectShaderResourceVariable* pkShaderResource);
	ID3DX11EffectShaderResourceVariable* GetShaderResourceAt(unsigned int uiSlot);

protected:

	void ExtractData();

	//////////////////////////////////////////////////////////////////////////

	stl_string m_sName;

	unsigned int m_uiInputSignatureDataSize;
	const void* m_pkInputSignatureData;

	ID3DX11Effect* m_d3dEffect;
	ID3DX11EffectTechnique* m_d3dTechnique;
	stl_vector<ID3DX11EffectPass*> m_d3dPasses;

	typedef stl_map<unsigned int, ID3DX11EffectShaderResourceVariable*> TShaderResourceMap;
	TShaderResourceMap m_d3dShaderResources;

	typedef stl_vector<CDX11ConstantBufferPtr> TConstantBufferList;
	TConstantBufferList m_vConstantBufferList;
};

CoreSmartPointer(CDX11EffectData);