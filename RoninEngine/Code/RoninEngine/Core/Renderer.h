#pragma once

#include "CoreMemObject.h"

class CMesh;

class CRenderer : public CCoreMemObject
{
public:
	
	CRenderer();
	virtual ~CRenderer();

	static CRenderer* GetRenderer() { return ms_pkRenderer; }

	unsigned int GetWidth() const { return m_uiWidth; }
	unsigned int GetHeight() const { return m_uiHeight; }
	
	//////////////////////////////////////////////////////////////////////////
	// Use virtual functions here to reduce the amount of specific object
	// which need to be created on a render specific level

	virtual void ReloadShaders() = 0;
	virtual void RenderMesh(CMesh* pkMesh) = 0;

protected:

	static CRenderer* ms_pkRenderer;

	unsigned int m_uiWidth;
	unsigned int m_uiHeight;

};