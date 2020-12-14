#pragma once
#include "DXF.h"
#include <vector>

// Using declerations
using std::vector;

// #defines
#define MAX_LIGHTS 8
#define MAX_POINTLIGHTS 1

class SceneShader : public BaseShader
{
public:
	struct ShaderParams
	{
		ShaderParams() {};

		ID3D11DeviceContext* deviceContext = 0;
		XMMATRIX* world = 0;
		XMMATRIX* view = 0;
		XMMATRIX* projection = 0;
		ID3D11ShaderResourceView* texture = 0;
		vector<ID3D11ShaderResourceView*> shadowMaps;
		vector<ID3D11ShaderResourceView*> pointShadowMaps;
		vector<Light*> *lights = 0;
		XMFLOAT3 *camPosition = 0;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse[MAX_LIGHTS];
		XMFLOAT4 direction[MAX_LIGHTS];
		XMFLOAT4 position[MAX_LIGHTS];
		XMFLOAT4 specularity[MAX_LIGHTS];
		XMFLOAT4 attenValues[MAX_LIGHTS]; // w is padding

		/*
			position[i].w:
				0 = directional light
				1 = point light
				2 = spotlight


			direction[i].w = spotlight exponents

			specularity[i].xyz = colour
			specularity[i].w = specular power
			must set w to 1 when using it as colour

			attenValues:
				x = constant
				y = linear
				z = quadratic 
		*/
	};
	struct LightMatrixType
	{
		XMMATRIX lightViewMatrix[MAX_LIGHTS];
		XMMATRIX lightProjectionMatrix[MAX_LIGHTS];
		XMMATRIX pointLightViewMatrix[MAX_POINTLIGHTS * 6];
		XMMATRIX pointLightProjectionMatrix[MAX_POINTLIGHTS];
		XMFLOAT4 camPosition;	// w is padding
	};

public:
	SceneShader(ID3D11Device* device, HWND hwnd, bool initShaders = true);
	~SceneShader();

	void setShaderParameters(ShaderParams &params);

protected:
	// Functions
	void initShader(const wchar_t* vs, const wchar_t* ps);

	// Buffers
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* lightMatrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

};

