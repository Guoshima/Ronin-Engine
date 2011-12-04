#pragma once

#include "Transform.h"
#include "Bound.h"

class CSceneGraphEntry;

class CSceneGraphObject : public CCoreRefMTObject
{
public:

	CSceneGraphObject();
	virtual ~CSceneGraphObject();

	void SetLocalTransform(const CTransform& kTransform);

	CTransform& GetLocalTransform() { return m_kLocalTransform; }
	const CTransform& GetLocalTransform() const { return m_kLocalTransform; }

	const CTransform& GetWorldTransform() const { return m_kWorldTransform; }
	const CBound& GetWorldBound() const;

	void UpdateWorldData();

protected:

	CTransform m_kLocalTransform;
	CTransform m_kWorldTransform;

	// reference to the scenegraph entry which is used during culling and such
	CSceneGraphEntry* m_pkSceneGraphEntry;
};

CoreSmartPointer(CSceneGraphEntry);