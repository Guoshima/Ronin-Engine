#pragma once

#include "CoreMemObject.h"

class CTexture;
CoreSmartPointer(CTexture);

class CRenderTarget : public CCoreRefMTObject
{
public:

	enum EClearFlags
	{
		CLEAR_BACKBUFFER = 0x1,
		CLEAR_STENCILBUFFER = 0x2,
		CLEAR_DEPTHBUFFER = 0x4,
		CLEAR_NONE = 0, 
		CLEAR_ALL = CLEAR_BACKBUFFER | CLEAR_DEPTHBUFFER | CLEAR_STENCILBUFFER
	}; 

	CRenderTarget();
	~CRenderTarget();

	bool Init(unsigned int uiNumOfTargets, 
		const stl_vector<CTexturePtr>& vRenderTargets,
		CTexture* pkDepthStencilView);
	bool Init(CTexture* pkDepthStencilView);

	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }

	unsigned int GetNumOfTargets() const { return m_uiNumOfTargets; }
	stl_vector<CTexturePtr>& GetRenderTargets() { return m_vRenderTargets; }
	CTexture* GetRenderTarget(unsigned int uiTarget) { 
		return m_vRenderTargets[uiTarget]; 
	}
	CTexture* GetDepthStencil() { return m_spDepthStencil; }

	void SetClearColor(D3DXVECTOR4 kColor);
	void SetClearColor(D3DXVECTOR4 kColor, unsigned int uiTarget);
	void SetDepthClear(float fValue) { m_fDepthClear = fValue; }
	void SetStencilClear(int iValue) { m_iStencilClear = iValue; }

	const D3DXVECTOR4& GetClearColor(unsigned int uiTarget) const {
		return m_akClearColors[uiTarget];
	}
	float GetDepthClear() const { return m_fDepthClear; }
	int GetStencilClear() const { return m_iStencilClear; }

private:

	//////////////////////////////////////////////////////////////////////////

	unsigned int m_uiWidth;
	unsigned int m_uiHeight;

	unsigned int m_uiNumOfTargets;
	stl_vector<CTexturePtr> m_vRenderTargets;
	CTexturePtr m_spDepthStencil;

	D3DXVECTOR4* m_akClearColors;
	float m_fDepthClear;
	int m_iStencilClear;
};

CoreSmartPointer(CRenderTarget);