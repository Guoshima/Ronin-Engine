#include "CorePCH.h"

#include "Transform.h"

//////////////////////////////////////////////////////////////////////////

void CTransform::SetUnitTransform()
{
	D3DXMatrixIdentity( &m_kData );
}

void CTransform::SetTranslate(const D3DXVECTOR3& kTranslate)
{
	m_kData._41 = kTranslate.x;
	m_kData._42 = kTranslate.y;
	m_kData._43 = kTranslate.z;
}

void CTransform::SetRotate(
	const D3DXVECTOR3& kForward, 
	const D3DXVECTOR3& kUp, 
	const D3DXVECTOR3& kRight)
{
	m_kData._11 = kRight.x;
	m_kData._12 = kRight.y;
	m_kData._13 = kRight.z;

	m_kData._21 = kUp.x;
	m_kData._22 = kUp.y;
	m_kData._23 = kUp.z;

	m_kData._31 = kForward.x;
	m_kData._32 = kForward.y;
	m_kData._33 = kForward.z;
}

//////////////////////////////////////////////////////////////////////////

void CTransform::GetInverse(CTransform& kInverse) const
{
	D3DXMatrixInverse(&kInverse.m_kData, 0, &m_kData);
}

//////////////////////////////////////////////////////////////////////////

void CTransform::SetRotateAroundAxis(D3DXVECTOR3 kAxis, float fAngle)
{
	D3DXMatrixRotationAxis(&m_kData, &kAxis, fAngle);
}

void CTransform::SetXRotation(float fAngle)
{
	D3DXMatrixRotationX(&m_kData, fAngle);
}

void CTransform::SetYRotation(float fAngle)
{
	D3DXMatrixRotationY(&m_kData, fAngle);
}

void CTransform::SetZRotation(float fAngle)
{
	D3DXMatrixRotationZ(&m_kData, fAngle);
}

//////////////////////////////////////////////////////////////////////////

CTransform CTransform::operator *(const CTransform& kOther)
{
	CTransform kResult;
	kResult.m_kData = m_kData * kOther.m_kData;
	return kResult;
}

//////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 CTransform::GetTranslate()
{
	D3DXVECTOR3 kTranslate;
	kTranslate.x = m_kData._41;
	kTranslate.y = m_kData._42;
	kTranslate.z = m_kData._43;
	return kTranslate;
}

D3DXVECTOR3 CTransform::GetRow(int iRow)
{
	D3DXVECTOR3 kResult;
	if (iRow == 0)
	{
		kResult.x = m_kData._11;
		kResult.y = m_kData._12;
		kResult.z = m_kData._13;
	}
	else if (iRow == 1)
	{
		kResult.x = m_kData._21;
		kResult.y = m_kData._22;
		kResult.z = m_kData._23;
	}
	else if (iRow == 2)
	{
		kResult.x = m_kData._31;
		kResult.y = m_kData._32;
		kResult.z = m_kData._33;
	}	
	return kResult;
}

D3DXVECTOR3 CTransform::GetCol(int iCol)
{
	D3DXVECTOR3 kResult;
	if (iCol == 0)
	{
		kResult.x = m_kData._11;
		kResult.y = m_kData._21;
		kResult.z = m_kData._31;
	}
	else if (iCol == 1)
	{
		kResult.x = m_kData._12;
		kResult.y = m_kData._22;
		kResult.z = m_kData._32;
	}
	else if (iCol == 2)
	{
		kResult.x = m_kData._13;
		kResult.y = m_kData._23;
		kResult.z = m_kData._33;
	}	
	return kResult;
}

//////////////////////////////////////////////////////////////////////////