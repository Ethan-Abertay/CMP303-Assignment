#pragma once
#include "SceneShader.h"

class TessellatedHeightMapShader : public BaseShader
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
		//XMMATRIX rotationMatrix;
	};

public:
	struct TessShaderParams
	{
		SceneShader::ShaderParams* shaderParams;
		int *maxTessFactor;
		XMFLOAT3 *camPos;
		float *maxHeight;
		int* maxTessDistance;
		ID3D11ShaderResourceView* heightMap = 0;
		ID3D11ShaderResourceView* normalMap = 0;
		//XMMATRIX* rotationMatrix = 0;
	};

public:
	TessellatedHeightMapShader(ID3D11Device* device, HWND hwnd);
	~TessellatedHeightMapShader();

	void setShaderParameters(TessShaderParams& params);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* hullDataBuffer;
	ID3D11Buffer* domainDataBuffer2;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* lightMatrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
};

