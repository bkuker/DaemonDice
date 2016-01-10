// File:		Camera.cpp
// Author:		Dennis Colburn
// Notes:		This should work correctly... if it doesn't please notify me...
//

#include "stdafx.h"
#include "Camera.h"


D3DXMATRIX Camera::s_mDiscard;

Camera::Camera() {
	//set initial angles of rotation
	m_fAngleX = 0.0f;
	m_fAngleY = 0.0f;
	m_fAngleZ = 0.0f;

	//set initial roll velocities
	m_fYawVelocity = 0.0f; 
	m_fPitchVelocity = 0.0f; 
	m_fRollVelocity = 0.0f; 

	//set initial position
	m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//start with no movement
	m_vVelocity2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation( &m_mOrient, 0.0f, 0.0f, 0.0f );
	
}

Camera::~Camera() {	

}

void Camera::decelerate(float decelVelocity) {
	//multiplies the velocity properties by the rate of deceleration.
	m_vVelocity2 *= decelVelocity;
	m_fYawVelocity *= decelVelocity;
	m_fPitchVelocity *= decelVelocity;
	m_fRollVelocity *= decelVelocity;
}

const D3DXMATRIX* Camera::transform() const {
	return D3DXMatrixMultiply( 
				&s_mDiscard, position(), orientation() );
}

void Camera::Update(FLOAT fSpeed, FLOAT fAngularSpeed) {

	m_vPosition += m_vVelocity;

	// Update the position vector
	m_vVelocity = m_vVelocity2 * fSpeed;
    D3DXVec3TransformNormal( &m_vVelocity, &m_vVelocity, D3DXMatrixInverse(&s_mDiscard, NULL, &m_mOrient) );

	

	// Update the yaw, pitch, roll angles
	m_fAngleX   += fAngularSpeed * m_fRollVelocity;
	m_fAngleZ   += fAngularSpeed * m_fYawVelocity;
    m_fAngleY   += fAngularSpeed * m_fPitchVelocity;


	// Set update the matricies
	D3DXQUATERNION qR;	
    D3DXQuaternionRotationYawPitchRoll( &qR, -m_fAngleZ, -m_fAngleY, -m_fAngleX );	
    D3DXMatrixAffineTransformation( &m_mOrient, 1.0f, NULL, &qR, NULL );
	D3DXMatrixTranslation( &m_mPos, -m_vPosition.x, -m_vPosition.y, -m_vPosition.z );
//	D3DXMatrixMultiply( &m_mTransform, &m_mPos, &m_mOrient );
}

