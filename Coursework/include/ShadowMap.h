#pragma once
#include "d3d.h"

using namespace DirectX;

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, int mWidth, int mHeight);
	~ShadowMap();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void clearSRV(ID3D11DeviceContext* dc) { dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0); };


	ID3D11ShaderResourceView* getDepthMapSRV() { return mDepthMapSRV; };

private:
	ID3D11DepthStencilView* mDepthMapDSV;
	ID3D11ShaderResourceView* mDepthMapSRV;
	D3D11_VIEWPORT viewport;
	ID3D11RenderTargetView* renderTargets[1];
	ID3D11Texture2D* depthMap;
};