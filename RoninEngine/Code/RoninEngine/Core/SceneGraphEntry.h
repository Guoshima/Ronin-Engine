#pragma once

#include "CoreMemObject.h"
#include "Bound.h"

class CSceneGraphObject;

// very object used in scenegraph for fast traversel
class CSceneGraphEntry : public CCoreMemObject 
{
public:

	CSceneGraphEntry(CSceneGraphObject* pkBackRefernce);
	~CSceneGraphEntry() {}

	const CBound& GetWorldBound() const { return m_kWorldBound; }
	const CSceneGraphObject* GetEntry() const { return m_pkBackReference; }

	// don't share the world bound because we want to keep it
	// very local in the cache to reduce cache misses when
	// traversing the scenegraph
	void SetWorldBound(const CBound& kBound) { m_kWorldBound = kBound; }

	// fast update functions
	void SetWorldTranslate(const D3DXVECTOR3& kTranslate) {
		m_kWorldBound.SetCenter(kTranslate);
	}

protected:

	// bound of object
	CBound m_kWorldBound;

	// back reference to the original object
	CSceneGraphObject* m_pkBackReference;

	// list of children if there are any
	CSceneGraphEntry* m_pkChildren;
};