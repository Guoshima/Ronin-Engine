#include "CorePCH.h"

#include "Renderer.h"

CRenderer* CRenderer::ms_pkRenderer = 0;

//////////////////////////////////////////////////////////////////////////

CRenderer::CRenderer() : m_uiWidth(0), m_uiHeight(0) {}
CRenderer::~CRenderer() {}

//////////////////////////////////////////////////////////////////////////