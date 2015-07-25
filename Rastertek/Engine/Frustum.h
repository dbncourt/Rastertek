////////////////////////////////////////////////////////////////////////////////
// Filename: Frustum.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: Frustum
///////////////////////////////////////////////////////////////////////////////
class Frustum
{
private:
	D3DXPLANE m_planes[6];

public:
	Frustum();
	Frustum(const Frustum& other);
	~Frustum();

	void ConstructFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix);

	bool CheckPoint(D3DXVECTOR3 point);
	bool CheckCube(D3DXVECTOR3 centerPoint, float radius);
	bool CheckSphere(D3DXVECTOR3 centerPoint, float radius);
	bool CheckRectangle(D3DXVECTOR3 centerPoint, D3DXVECTOR3 size);
};

#endif