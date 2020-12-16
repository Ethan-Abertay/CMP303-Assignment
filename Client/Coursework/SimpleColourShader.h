#pragma once
#include "BaseShader.h"

class SimpleColourShader : public BaseShader
{
public:
	struct Params
	{
		ID3D11DeviceContext* deviceContext = 0;
		XMMATRIX* world = 0;
		XMMATRIX* view = 0;
		XMMATRIX* projection = 0;
	};

public:
	SimpleColourShader(ID3D11Device* device, HWND hwnd);
	~SimpleColourShader();

	void setShaderParameters(Params& params);

protected:
	// Functions
	void initShader(const wchar_t* vs, const wchar_t* ps);

	// Buffers
	ID3D11Buffer* matrixBuffer;

};

