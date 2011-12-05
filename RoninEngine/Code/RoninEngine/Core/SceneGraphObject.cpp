#include "CorePCH.h"

#include "SceneGraphObject.h"

#include "SceneGraphEntry.h"

//////////////////////////////////////////////////////////////////////////

CSceneGraphObject::CSceneGraphObject()
{
	m_kLocalTransform.SetUnitTransform();
	m_kWorldTransform.SetUnitTransform();

	m_pkSceneGraphEntry = IntNew CSceneGraphEntry(this);
}

CSceneGraphObject::~CSceneGraphObject()
{
	IntDelete m_pkSceneGraphEntry;
}

//////////////////////////////////////////////////////////////////////////

void CSceneGraphObject::UpdateWorldData()
{
	// for now simple set world transform equal to local transform
	m_kWorldTransform = m_kLocalTransform;

	// update the translate on the bounding box
	D3DXVECTOR3 kWorldTranslate = m_kWorldTransform.GetTranslate();
	m_pkSceneGraphEntry->SetWorldTranslate(kWorldTranslate);
}

//////////////////////////////////////////////////////////////////////////