#pragma once

#include <CoreMemObject.h>

class CDX11ConstantBuffer;
CoreSmartPointer(CDX11ConstantBuffer);

typedef stl_vector<CDX11ConstantBufferPtr> TConstantBufferVector;

class CDX11Shader : public CCoreMemObject
{
public:
	
	enum EShaderType
	{
		SHADER_UNDEFINED,
		SHADER_VERTEX,
		SHADER_PIXEL,
		SHADER_GEOMETRY,
		SHADER_DOMAIN,
		SHADER_HULL,
		SHADER_COMPUTE,
		MAX_SHADER_COUNT,
	};

	CDX11Shader();
	~CDX11Shader();	

protected:

	//////////////////////////////////////////////////////////////////////////

	/*
	template<class T, class D3DTYPE> struct SShaderDependency
	{
		UINT    StartIndex;
		UINT    Count;

		T       *ppFXPointers;              // Array of ptrs to FX objects (CBs, SShaderResources, etc)
		D3DTYPE *ppD3DObjects;              // Array of ptrs to matching D3D objects

		SShaderDependency()
		{
			StartIndex = Count = 0;

			ppD3DObjects = NULL;
			ppFXPointers = NULL;
		}
	};

	typedef SShaderDependency<SConstantBuffer*, ID3D11Buffer*> SShaderCBDependency;
	typedef SShaderDependency<SSamplerBlock*, ID3D11SamplerState*> SShaderSamplerDependency;
	typedef SShaderDependency<SShaderResource*, ID3D11ShaderResourceView*> SShaderResourceDependency;
	typedef SShaderDependency<SUnorderedAccessView*, ID3D11UnorderedAccessView*> SUnorderedAccessViewDependency;
	typedef SShaderDependency<SInterface*, ID3D11ClassInstance*> SInterfaceDependency;
	*/

	// Type of shader
	EShaderType m_eShaderType;

	// shader itself
	ID3D11DeviceChild* m_d3dShader;

	// all the constant buffers which are used in this shader	
	TConstantBufferVector m_vConstantBuffers;

	// all the samplers which are used by this shader


	// all the resource views (textures) used by this shader

};