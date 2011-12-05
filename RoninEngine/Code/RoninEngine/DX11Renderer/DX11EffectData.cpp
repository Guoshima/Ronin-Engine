#include "DX11RendererPCH.h"

#include "DX11EffectData.h"

#include "DX11ConstantBuffer.h"
#include "DX11ConstantBufferDesc.h"

//////////////////////////////////////////////////////////////////////////

CDX11EffectData::CDX11EffectData(
	ID3DX11Effect* d3dEffect, ID3DX11EffectTechnique* d3dTechnique) :
m_d3dEffect(d3dEffect), m_d3dTechnique(d3dTechnique),
m_pkInputSignatureData(0), m_uiInputSignatureDataSize(0)
{
	SAFE_ADDREF(m_d3dEffect);

	ExtractData();
}

CDX11EffectData::~CDX11EffectData()
{
#ifdef _DEBUG
	for (TConstantBufferList::iterator it = m_vConstantBufferList.begin();
		it != m_vConstantBufferList.end(); ++it)
	{
		CDX11ConstantBuffer* pConstantBuffer = *it;
		assert(pConstantBuffer->GetRefCount() == 1);
	}
#endif
	m_vConstantBufferList.clear();

	SAFE_RELEASE(m_d3dEffect);
}

//////////////////////////////////////////////////////////////////////////

const void* CDX11EffectData::GetInputSignatureData(unsigned int& uiSize)
{
	uiSize = m_uiInputSignatureDataSize;
	return m_pkInputSignatureData;
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectData::ExtractData()
{
	if (m_d3dTechnique == 0)
		return; 

	D3DX11_TECHNIQUE_DESC kTechDesc;
	m_d3dTechnique->GetDesc( &kTechDesc );

	m_sName = kTechDesc.Name;
	for( unsigned short us = 0; us < kTechDesc.Passes; ++us )
	{
		ID3DX11EffectPass* d3dPass = m_d3dTechnique->GetPassByIndex(us);

		// store the pass
		m_d3dPasses.push_back(d3dPass);

		// process the vertex shader
		{
			D3DX11_PASS_SHADER_DESC kPassShaderDesc;
			d3dPass->GetVertexShaderDesc(&kPassShaderDesc);

			D3DX11_EFFECT_SHADER_DESC kEffectShaderDesc;
			kPassShaderDesc.pShaderVariable->GetShaderDesc(0, &kEffectShaderDesc);

			ID3D11ShaderReflection* pkReflector = NULL; 
			D3DReflect( kEffectShaderDesc.pBytecode, kEffectShaderDesc.BytecodeLength, 
				IID_ID3D11ShaderReflection, (void**) &pkReflector);

			D3D11_SHADER_DESC kShaderDesc;
			pkReflector->GetDesc(&kShaderDesc);

			for (unsigned short us = 0; us < kShaderDesc.ConstantBuffers; ++us)
			{
				ID3D11ShaderReflectionConstantBuffer* pkConstantBuffer =
					pkReflector->GetConstantBufferByIndex(us);

				int i = 0;
			}

			if (m_pkInputSignatureData == 0)
			{
				m_uiInputSignatureDataSize = kEffectShaderDesc.BytecodeLength;
				m_pkInputSignatureData = kEffectShaderDesc.pBytecode;
			}
		}

		// process the pixel shader
		{
			D3DX11_PASS_SHADER_DESC kPassShaderDesc;
			d3dPass->GetPixelShaderDesc(&kPassShaderDesc);

			D3DX11_EFFECT_SHADER_DESC kEffectShaderDesc;
			kPassShaderDesc.pShaderVariable->GetShaderDesc(0, &kEffectShaderDesc);

			ID3D11ShaderReflection* pkReflector = NULL; 
			D3DReflect( kEffectShaderDesc.pBytecode, kEffectShaderDesc.BytecodeLength, 
				IID_ID3D11ShaderReflection, (void**) &pkReflector);

			if (pkReflector)
			{
				D3D11_SHADER_DESC kShaderDesc;
				pkReflector->GetDesc(&kShaderDesc);

				for (unsigned short us = 0; us < kShaderDesc.ConstantBuffers; ++us)
				{
					ID3D11ShaderReflectionConstantBuffer* pkConstantBuffer =
						pkReflector->GetConstantBufferByIndex(us);

					int i = 0;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectData::AddConstantBuffer(CDX11ConstantBuffer* pkCB)
{
	m_vConstantBufferList.push_back(pkCB);
}

//////////////////////////////////////////////////////////////////////////

void CDX11EffectData::AddShaderResource(
	unsigned int uiSlot, ID3DX11EffectShaderResourceVariable* pkShaderResource)
{
	m_d3dShaderResources[uiSlot] = pkShaderResource;
}

ID3DX11EffectShaderResourceVariable* CDX11EffectData::GetShaderResourceAt(
	unsigned int uiSlot)
{
	TShaderResourceMap::const_iterator pkIter = m_d3dShaderResources.find(uiSlot);
	if (pkIter != m_d3dShaderResources.end())
		return pkIter->second;

	return 0;
}

//////////////////////////////////////////////////////////////////////////