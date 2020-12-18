#include "SimpleColourShader.h"

SimpleColourShader::SimpleColourShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"SimpleColour_vs.cso", L"SimpleColour_ps.cso");
}

SimpleColourShader::~SimpleColourShader()
{
}

void SimpleColourShader::setShaderParameters(Params& params)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Matrices need to be transposed
	XMMATRIX tworld, tview, tproj;
	tworld = XMMatrixTranspose(*params.world);
	tview = XMMatrixTranspose(*params.view);
	tproj = XMMatrixTranspose(*params.projection);

	// Update matrix buffer
	MatrixBufferType* matrixPtr;
	result = params.deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	matrixPtr->world = tworld;// worldMatrix;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;
	params.deviceContext->Unmap(matrixBuffer, 0);
	params.deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Update data buffer
	DataBufferType* dataPtr;
	result = params.deviceContext->Map(dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (DataBufferType*)mappedResource.pData;
	dataPtr->colour = params.colour;
	params.deviceContext->Unmap(dataBuffer, 0);
	params.deviceContext->PSSetConstantBuffers(0, 1, &dataBuffer);
}

void SimpleColourShader::initShader(const wchar_t* vs, const wchar_t* ps)
{
	// Load (+ compile) shader files
	loadVertexShader(vs);
	loadPixelShader(ps);

	// Setup the description of the dynamic matrix constant buffer
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup data buffer
	D3D11_BUFFER_DESC DataBufferDesc;
	DataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	DataBufferDesc.ByteWidth = sizeof(DataBufferType);
	DataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DataBufferDesc.MiscFlags = 0;
	DataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&DataBufferDesc, NULL, &dataBuffer);
}
