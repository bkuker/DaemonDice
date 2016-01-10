
#if !defined(AFX_CAMERA_H__FA93B7A4_8550_4DC3_B1E6_5D94B3319DB2__INCLUDED_)
#define AFX_CAMERA_H__FA93B7A4_8550_4DC3_B1E6_5D94B3319DB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Moveable.h"
class Cell;

class Camera : public Moveable{
public:
	Camera();
	~Camera();
	virtual const D3DXMATRIX* transform() const;

	void Update(FLOAT fSpeed, FLOAT fAngularSpeed);

	//slides
	inline void mRight   ( float fSpeed ) { m_vVelocity2.x	+= fSpeed; }
	inline void mLeft    ( float fSpeed ) { m_vVelocity2.x	-= fSpeed; }
	inline void mUp      ( float fSpeed ) { m_vVelocity2.y	+= fSpeed; }
	inline void mDown    ( float fSpeed ) { m_vVelocity2.y	-= fSpeed; }
	inline void mForward ( float fSpeed ) { m_vVelocity2.z	+= fSpeed; }
	inline void mBackward( float fSpeed ) { m_vVelocity2.z	-= fSpeed; }

	//six DOF rotations
	inline void rRight  ( float fSpeed ) { m_fYawVelocity	+= fSpeed;  }
	inline void rLeft   ( float fSpeed ) { m_fYawVelocity   -= fSpeed;  }
	inline void rDown   ( float fSpeed ) { m_fPitchVelocity += fSpeed;  }
	inline void rUp     ( float fSpeed ) { m_fPitchVelocity -= fSpeed;  }
	inline void rCw		( float fSpeed ) { m_fRollVelocity += fSpeed;   }
	inline void rCcw    ( float fSpeed ) { m_fRollVelocity -= fSpeed;   }

	//decelerate (dampen movement)
	void decelerate( float decelVelocity );

	//D3DXVECTOR3 m_vCameraPosition; //position of camera in world coords
	//D3DXVECTOR3 vT;

protected:

	//Cell* m_Cell;

	float m_fAngleX;
	float m_fAngleY;
	float m_fAngleZ;

	D3DXVECTOR3	m_vUp,		//The vertical Vector
				m_vRight,	//The Horizontal Vector
				m_vLook;	//The Cameras Look (in the z direction) vector.

	D3DXVECTOR3 m_vPosition, m_vVelocity;

	float		m_fFov,		//Width of the vertical field-of-view in radians.
				m_fAspect,	//Aspect ratio of the camera's angle of view. 
				m_fNear,	//The distance to the near clipping plane.
				m_fFar;		//The distance to the far clipping plane.

	D3DXVECTOR3	m_vVelocity2; //velocity for movement

	static D3DXMATRIX s_mDiscard;

	float		m_fYawVelocity; //how fast we Yaw
	float		m_fPitchVelocity; //how fast we Pitch
	float       m_fRollVelocity; //how fast we Roll ***ROLL***
};

#endif