#pragma once

#include "CoreMemObject.h"

class CBound : public CCoreMemObject
{
public:

	enum BoundType {
		BOUND_SPHERE,
		BOUND_AABB,
		BOUND_OBB,
		BOUND_MAX
	};

	CBound(BoundType eType = BOUND_SPHERE);

	void SetCenterAndRadius(const D3DXVECTOR3& kCenter, float fRadius) {
		m_kCenter = kCenter;
		m_fRadius = fRadius;
	}
	void SetCenter(const D3DXVECTOR3& kCenter) {
		m_kCenter = kCenter;
	}
	void SetRadius(float fValue) {
		m_fRadius = fValue;
	}
	
	BoundType GetBoundType() const { return (BoundType) m_eType; }
	const D3DXVECTOR3& GetCenter() const { return m_kCenter; }
	float GetRadius() const { return m_fRadius; }

protected:

	// type of bound
	unsigned char m_eType;

	// Sphere data is always available
	D3DXVECTOR3 m_kCenter;
	float m_fRadius;
};
