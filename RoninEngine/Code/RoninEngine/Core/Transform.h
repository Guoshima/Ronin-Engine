#pragma once

#include "CoreMemObject.h"

class CTransform : public CCoreMemObject
{
public:

	void SetUnitTransform();

	void SetTranslate(const D3DXVECTOR3& kTranslate);
	void SetRotate(
		const D3DXVECTOR3& kForward, 
		const D3DXVECTOR3& kUp, 
		const D3DXVECTOR3& kRight);

	void SetRotateAroundAxis(D3DXVECTOR3 kAxis, float fAngle);

	void SetXRotation(float fAngle);
	void SetYRotation(float fAngle);
	void SetZRotation(float fAngle);

	CTransform operator*(const CTransform& kOther);

	D3DXVECTOR3 GetTranslate();
	D3DXVECTOR3 GetCol(int iCol);
	D3DXVECTOR3 GetRow(int iRow);

	void GetInverse(CTransform& kInverse) const;
	
	const D3DXMATRIXA16& GetData() const { return m_kData; }
	D3DXMATRIXA16& GetData() { return m_kData; }

protected:

	D3DXMATRIXA16 m_kData;
};
