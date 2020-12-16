#pragma once
#include "DepthShader.h"

class TessHeightMapDepthShader : public DepthShader
{
private:
	struct HullDataBufferType
	{
		int maxTessFactor;
		XMFLOAT3 camPos;
		XMMATRIX worldMatrix;
		int maxTessDistance;      // The number of control patches around you that are tessellated 
		XMFLOAT3 padding;
	};
	struct DomainDataBuffer2Type
	{
		int maxTessRange;
		XMFLOAT3 padding;
		XMFLOAT4 camPos;	// w is height
	};

public:
	struct TessDepthShaderParams
	{
		int* maxTessFactor;
		XMFLOAT3* camPos;
		float* maxHeight;
		int* maxTessDistance;
		ID3D11ShaderResourceView* heightMap = 0;
	};

public:
	TessHeightMapDepthShader(ID3D11Device* device, HWND hwnd, bool initShaders = true);

	void setShaderParameters(TessDepthShaderParams& params, ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

	ID3D11Buffer* hullBuffer, *domainBuffer;

	// Samplers
	ID3D11SamplerState* sampleState;	// Needed for height map

};

