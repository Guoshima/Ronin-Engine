#include "CorePCH.h"

#include "Camera.h"

//////////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	m_kViewPort.m_fTop = 1.0f;
	m_kViewPort.m_fBottom = 0.0f;
	m_kViewPort.m_fLeft = 0.0f;
	m_kViewPort.m_fRight = 1.0f;

	m_fNear = 0.1f;
	m_fFar = 100.0f;

	m_fFOV = (float) D3DX_PI / 3.0f;

	/*
	m_kFrustum.m_fNear = 1.0f;
	m_kFrustum.m_fFar = 2.0f;
	m_kFrustum.m_fTop = 0.5f;
	m_kFrustum.m_fBottom = -0.5f;
	m_kFrustum.m_fLeft = -0.5f;
	m_kFrustum.m_fRight = 0.5f;
	m_kFrustum.m_bOrtho = false;
	*/

	m_kWorldTransform.SetUnitTransform();
}

CCamera::~CCamera()
{

}

//////////////////////////////////////////////////////////////////////////

void CCamera::SetWorldLocation(const D3DXVECTOR3& kTranslate)
{
	m_kWorldTransform.SetTranslate(kTranslate);
}

bool CCamera::SetLookAt(
	const D3DXVECTOR3& kLocation, const D3DXVECTOR3& kUpVector)
{
	D3DXVECTOR3 kDir = m_kWorldTransform.GetTranslate() - kLocation;
	if (D3DXVec3LengthSq(&kDir) < 1e-10f)
		return false;
	
	D3DXVec3Normalize(&kDir, &kDir);

	D3DXVECTOR3 kRight;
	D3DXVec3Cross(&kRight, &kUpVector, &kDir);

	D3DXVec3Normalize(&kRight, &kRight);

	// will either be unit length or zero
	if (D3DXVec3LengthSq(&kRight) < 0.5f)
		return false;

	D3DXVECTOR3 kUp;
	D3DXVec3Cross(&kUp, &kDir, &kRight);

	// will either be unit length or zero
	if (D3DXVec3LengthSq(&kUp) < 0.5f)
		return false;

	// set the rotate
	m_kWorldTransform.SetRotate(-kDir, kUp, kRight);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CCamera::GetViewMatrix(CTransform& kViewMatrix) const
{
	m_kWorldTransform.GetInverse(kViewMatrix);
	return true;
}

//////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 CCamera::GetWorldLocation()
{
	return m_kWorldTransform.GetTranslate();
}

D3DXVECTOR3 CCamera::GetRight()
{
	return m_kWorldTransform.GetRow(0);
}

D3DXVECTOR3 CCamera::GetUp()
{
	return m_kWorldTransform.GetRow(1);
}

D3DXVECTOR3 CCamera::GetForward()
{
	return m_kWorldTransform.GetRow(2);
}

//////////////////////////////////////////////////////////////////////////