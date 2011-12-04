#pragma once

#include "CoreMemObject.h"

class CSamplerDesc : public CCoreRefMTObject
{
public:
	
	CSamplerDesc();
	virtual ~CSamplerDesc() {}

	bool IsDirty() const { return m_bDirty; }
	void ResetDirty() { m_bDirty = false; }

protected:

	void SetDirty() { m_bDirty = true; }

	//////////////////////////////////////////////////////////////////////////

	bool m_bDirty;

};

CoreSmartPointer(CSamplerDesc);