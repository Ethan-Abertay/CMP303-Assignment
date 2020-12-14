#pragma once
#include "BaseShader.h"

class BlendPassShader : public BaseShader
{
private:
	struct DataBuffer
	{
		XMFLOAT3 rayColour;
		float padding;
		XMUINT2 resolution;
		XMFLOAT2 padding2;
	};

public:
	struct Params
	{
		Params() {};

		XMFLOAT3 *rayColour = 0;
		ID3D11ShaderResourceView* rayMap = 0;
		ID3D11ShaderResourceView* sceneTexture = 0;
	};

public:
	BlendPassShader(ID3D11Device* device, HWND hwnd, int w, int h);
	~BlendPassShader();

	void setShaderParameters(ID3D11DeviceContext* dc, Params& params);
	void createOutputUAV();
	ID3D11ShaderResourceView* getSRV() { return m_srvTexOutput; };
	void unbind(ID3D11DeviceContext* dc);

private:
	void initShader(const wchar_t* cs, const wchar_t* asdf) override;

	ID3D11Buffer* dataBuffer;

	ID3D11ShaderResourceView* srv;
	ID3D11UnorderedAccessView* uav;

	// texture set
	ID3D11Texture2D* m_tex;
	ID3D11UnorderedAccessView* m_uavAccess;
	ID3D11ShaderResourceView* m_srvTexOutput;

	ID3D11SamplerState* sampleState;

	int sWidth;
	int sHeight;
};
