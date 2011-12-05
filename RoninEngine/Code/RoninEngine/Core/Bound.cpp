#include "CorePCH.h"

#include "Bound.h"

//////////////////////////////////////////////////////////////////////////

CBound::CBound(CBound::BoundType eType) : m_eType(eType), m_fRadius(0.0f)
{
	m_kCenter.x = m_kCenter.y = m_kCenter.z = 0.0f; 
}

//////////////////////////////////////////////////////////////////////////