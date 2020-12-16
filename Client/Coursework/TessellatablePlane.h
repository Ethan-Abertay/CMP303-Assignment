#pragma once
#include "BaseMesh.h"

class TessellatablePlane : public BaseMesh
{
public:
	TessellatablePlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT2 dimensions_, XMINT2 res, XMFLOAT2 texRes);
	~TessellatablePlane();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST) override;

private:
	void initBuffers(ID3D11Device* device);

	XMFLOAT2 dimensions;
	XMFLOAT2 texResolution;
	XMINT2 resolution;

};


