#pragma once

#include "CoreMemObject.h"

#include "Transform.h"

class CCamera : public CCoreRefMTObject
{
public:

	struct SViewPort {
		float m_fTop;
		float m_fBottom;
		float m_fLeft;
		float m_fRight;
	};

	/*
	struct SFrustum {
		float m_fLeft;
		float m_fRight;
		float m_fTop;
		float m_fBottom;
		float m_fNear;
		float m_fFar;
		bool  m_bOrtho;
	};
	*/

	CCamera();
	~CCamera();

	SViewPort& GetViewPort() { return m_kViewPort; }
	const SViewPort& GetViewPort() const { return m_kViewPort; }
	void SetViewPort(const SViewPort& kViewPort) { m_kViewPort = kViewPort; }

	/*
	SFrustum& GetFrustum() { return m_kFrustum; }
	const SFrustum& GetFrustum() const { return m_kFrustum; }
	void SetFrustum(const SFrustum& kFrustum) { m_kFrustum = kFrustum; }
	*/

	void SetNearFarDistance(float fNear, float fFar) {
		m_fNear = fNear;
		m_fFar = fFar;
	}
	float GetNearDistance() const { return m_fNear; }
	float GetFarDistance() const { return m_fFar; }

	void SetFOV(float fFOV) { m_fFOV = fFOV; }
	float GetFov() const { return m_fFOV; }

	CTransform& GetWorldTransform() { return m_kWorldTransform; }
	const CTransform& GetWorldTransform() const { return m_kWorldTransform; }
	void SetWorldTransform(const CTransform& kTransform) { m_kWorldTransform = kTransform; }

	D3DXVECTOR3 GetWorldLocation();

	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();
	D3DXVECTOR3 GetForward();

	bool GetViewMatrix(CTransform& kViewMatrix) const;

	void SetWorldLocation(const D3DXVECTOR3& kTranslate);
	bool SetLookAt(const D3DXVECTOR3& kLocation, const D3DXVECTOR3& kUpVector);

protected:

	//////////////////////////////////////////////////////////////////////////

	CTransform m_kWorldTransform;

	SViewPort m_kViewPort;

	float m_fNear;
	float m_fFar;

	float m_fFOV;
	//SFrustum m_kFrustum;
};

CoreSmartPointer(CCamera);