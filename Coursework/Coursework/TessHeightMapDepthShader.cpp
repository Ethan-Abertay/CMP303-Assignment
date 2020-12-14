#include "TessHeightMapDepthShader.h"

TessHeightMapDepthShader::TessHeightMapDepthShader(ID3D11Device* device, HWND hwnd, bool initShaders) : DepthShader(device, hwnd, false)
{
	if (initShaders)
		initShader(L"TessellatedHeightMap_vs.cso", L"TessHeightMapDepth_hs.cso", L"TessHeightMapDepth_ds.cso", L"TrigDepth_ps.cso");
}

void TessHeightMapDepthShader::setShaderParameters(TessDepthShaderParams& params, ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HullDataBufferType *hullPtr;
	result = deviceContext->Map(hullBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	hullPtr = (HullDataBufferType*)mappedResource.pData;
	hullPtr->camPos = *params.camPos;
	hullPtr->maxTessDistance = *params.maxTessDistance;
	hullPtr->maxTessFactor = *params.maxTessFactor;
	hullPtr->worldMatrix = XMMatrixTranspose(world);
	deviceContext->Unmap(hullBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &hullBuffer);

	DomainDataBuffer2Type *domainPtr;
	result = deviceContext->Map(domainBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	domainPtr = (DomainDataBuffer2Type*)mappedResource.pData;
	domainPtr->maxTessRange = *params.maxTessDistance;
	XMFLOAT4 pos = XMFLOAT4(params.camPos->x, params.camPos->y, params.camPos->z, 0.f);
	domainPtr->camPos = pos;
	domainPtr->camPos.w = *params.maxHeight;
	deviceContext->Unmap(domainBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &domainBuffer);

	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->DSSetShaderResources(0, 1, &params.heightMap);		// Send texture for object
	deviceContext->DSSetSamplers(0, 1, &sampleState);		// Send standard sample state
}

void TessHeightMapDepthShader::initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps)
{
	DepthShader::initShader(vs, ps);

	// Load other required shaders.
	loadHullShader(hs);
	loadDomainShader(ds);

	D3D11_BUFFER_DESC hullDataDesc;
	hullDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	hullDataDesc.ByteWidth = sizeof(HullDataBufferType);
	hullDataDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hullDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hullDataDesc.MiscFlags = 0;
	hullDataDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&hullDataDesc, NULL, &hullBuffer);

	D3D11_BUFFER_DESC domainDataDesc;
	domainDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	domainDataDesc.ByteWidth = sizeof(DomainDataBuffer2Type);
	domainDataDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	domainDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	domainDataDesc.MiscFlags = 0;
	domainDataDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&domainDataDesc, NULL, &domainBuffer);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}
