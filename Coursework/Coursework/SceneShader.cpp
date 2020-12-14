#include "SceneShader.h"

SceneShader::SceneShader(ID3D11Device* device, HWND hwnd, bool initShaders) : BaseShader(device, hwnd)
{
	if (initShaders)
		initShader(L"SceneShader_vs.cso", L"SceneShader_ps.cso");
}

SceneShader::~SceneShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (lightMatrixBuffer)
	{
		lightMatrixBuffer->Release();
		lightMatrixBuffer = 0;
	}
}

void SceneShader::setShaderParameters(ShaderParams& params)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixPtr;
	LightBufferType* lightPtr;
	LightMatrixType* lightMatrixPtr;

	// Matrices need to be transposed
	XMMATRIX tworld, tview, tproj;
	tworld = XMMatrixTranspose(*params.world);
	tview = XMMatrixTranspose(*params.view);
	tproj = XMMatrixTranspose(*params.projection);

	// Update matrix buffer
	result = params.deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	matrixPtr->world = tworld;// worldMatrix;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;
	params.deviceContext->Unmap(matrixBuffer, 0);
	params.deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Update light buffers
	result = params.deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (SceneShader::LightBufferType*)mappedResource.pData;
	int noOfNonPointLights = 0;
	int noOfPointLights = 0;
	vector<Light*> pointLights;
	auto disableLight = [&](int i)
	{
		lightPtr->diffuse[i] = XMFLOAT4(0, 0, 0, 0);
		lightPtr->direction[i] = XMFLOAT4(0, 0, 0, 0);
		lightPtr->position[i] = XMFLOAT4(0, 0, 0, 0);
		lightPtr->specularity[i] = XMFLOAT4(0, 0, 0, 0);
		lightPtr->attenValues[i] = XMFLOAT4(0, 0, 0, 0);
	};
	lightPtr->ambient = params.lights->front()->getAmbientColour();	// Set the ambient to the first light's value
	for (int i = 0; i < MAX_LIGHTS; i++)	// Set all light variables
	{
		if (i < params.lights->size())
		{
			if (!(*params.lights)[i]->getBEnabled())
			{
				// Disable 
				disableLight(i);
				continue;
			}

			if ((*params.lights)[i]->getLightType() == Light::LightType::DIRECTIONAL)
			{
				lightPtr->position[i].w = 0;	// Set padding to indicate this is a directional light
				noOfNonPointLights++;
			}
			else if ((*params.lights)[i]->getLightType() == Light::LightType::POINT)
			{
				lightPtr->position[i].w = 1;	// Set padding to indicate this is a point light
				noOfPointLights++;
				pointLights.push_back((*params.lights)[i]);	// Store pointer to the point light so you can put the info after other lights
				continue;
			}
			else if ((*params.lights)[i]->getLightType() == Light::LightType::SPOTLIGHT)
			{
				lightPtr->position[i].w = 2;	// Set padding to indicate this is a spot light
				lightPtr->direction[i].w = (*params.lights)[i]->getSpotlightExponent();
				noOfNonPointLights++;
			}

			// Set diffuse
			lightPtr->diffuse[i] = (*params.lights)[i]->getDiffuseColour();

			// Set direction
			lightPtr->direction[i].x = (*params.lights)[i]->getDirection().x;
			lightPtr->direction[i].y = (*params.lights)[i]->getDirection().y;
			lightPtr->direction[i].z = (*params.lights)[i]->getDirection().z;

			// Set position
			lightPtr->position[i].x = (*params.lights)[i]->getPosition().x;
			lightPtr->position[i].y = (*params.lights)[i]->getPosition().y;
			lightPtr->position[i].z = (*params.lights)[i]->getPosition().z;

			// Set specular info
			lightPtr->specularity[i] = (*params.lights)[i]->getSpecularColour();
			lightPtr->specularity[i].w = (*params.lights)[i]->getSpecularPower();

			// Set attenuation info
			lightPtr->attenValues[i].x = (*params.lights)[i]->getAttenConst();
			lightPtr->attenValues[i].y = (*params.lights)[i]->getAttenLinear();
			lightPtr->attenValues[i].z = (*params.lights)[i]->getAttenQuadratic();
		}
		else
		{
			// Disable light
			disableLight(i);
		}
	}
	//for (int i = noOfNonPointLights; i < noOfNonPointLights + pointLights.size(); i++)	// Now put all point light info after
	//{
	//	// Set diffuse
	//	lightPtr->diffuse[i] = pointLights[i]->getDiffuseColour();

	//	// Set direction
	//	lightPtr->direction[i].x = pointLights[i]->getDirection().x;
	//	lightPtr->direction[i].y = pointLights[i]->getDirection().y;
	//	lightPtr->direction[i].z = pointLights[i]->getDirection().z;

	//	// Set position
	//	lightPtr->position[i].x = pointLights[i]->getPosition().x;
	//	lightPtr->position[i].y = pointLights[i]->getPosition().y;
	//	lightPtr->position[i].z = pointLights[i]->getPosition().z;

	//	// Set specular info
	//	lightPtr->specularity[i] = pointLights[i]->getSpecularColour();
	//	lightPtr->specularity[i].w = pointLights[i]->getSpecularPower();

	//	// Set attenuation info
	//	lightPtr->attenValues[i].x = pointLights[i]->getAttenConst();
	//	lightPtr->attenValues[i].y = pointLights[i]->getAttenLinear();
	//	lightPtr->attenValues[i].z = pointLights[i]->getAttenQuadratic();
	//}
	lightPtr->attenValues[0].w = noOfNonPointLights;
	lightPtr->attenValues[1].w = noOfPointLights;
	params.deviceContext->Unmap(lightBuffer, 0);
	params.deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Get light matrix data
	result = params.deviceContext->Map(lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightMatrixPtr = (LightMatrixType*)mappedResource.pData;

	// Give camera's position
	XMFLOAT4 pos = XMFLOAT4(params.camPosition->x, params.camPosition->y, params.camPosition->z, 0.f);
	lightMatrixPtr->camPosition = pos;

	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	auto disableLightMatrices = [&](int i)
	{
		lightMatrixPtr->lightViewMatrix[i] = XMMatrixIdentity();
		lightMatrixPtr->lightProjectionMatrix[i] = XMMatrixIdentity();
	};
	int pointLightCounter = 0;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (i < params.lights->size())
		{
			if ((*params.lights)[i]->getBEnabled())
			{
				// Give correct matrices

				if ((*params.lights)[i]->getLightType() == Light::LightType::DIRECTIONAL)
					lightProjectionMatrix = XMMatrixTranspose((*params.lights)[i]->getOrthoMatrix());
				else
					lightProjectionMatrix = XMMatrixTranspose((*params.lights)[i]->getProjectionMatrix());

				if ((*params.lights)[i]->getLightType() != Light::LightType::POINT)
				{
					lightViewMatrix = XMMatrixTranspose((*params.lights)[i]->getViewMatrix());
					lightMatrixPtr->lightViewMatrix[i] = lightViewMatrix;
					lightMatrixPtr->lightProjectionMatrix[i] = lightProjectionMatrix;
				}
				else
				{
					// Point lights have more (and different) matrices
					lightMatrixPtr->pointLightProjectionMatrix[pointLightCounter] = lightProjectionMatrix;
					int counter = 0;
					for (int j = pointLightCounter * 6; j < (pointLightCounter * 6) + 6; j++)
					{
						lightMatrixPtr->pointLightViewMatrix[j] = (*params.lights)[i]->getViewMatrix(counter);
						counter++;
					}
				}
			}
			else
			{
				// Light is disabled
				disableLightMatrices(i);
			}
		}
		else
		{
			// There is no light
			disableLightMatrices(i);
		}
	}

	params.deviceContext->Unmap(lightMatrixBuffer, 0);
	params.deviceContext->VSSetConstantBuffers(1, 1, &lightMatrixBuffer);

	// Set textures and samplers
	params.deviceContext->PSSetShaderResources(0, 1, &params.texture);			// Send texture for object
	params.deviceContext->PSSetShaderResources(1, params.shadowMaps.size(), &params.shadowMaps.front());	// Send depth textures
	//params.deviceContext->PSSetShaderResources(1 + params.shadowMaps.size(), params.pointShadowMaps.size(), &params.pointShadowMaps.front());	// Send point light shadow maps
	params.deviceContext->PSSetSamplers(0, 1, &sampleState);		// Send standard sample state
	params.deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);	// Send shadow sample state
}

void SceneShader::initShader(const wchar_t* vs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightMatrixBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vs);
	loadPixelShader(ps);
	
	// Setup the description of the dynamic matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
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

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup the description of the light dynamic constant buffer 
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup the description of the light matrix dynamic constant buffer 
	lightMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightMatrixBufferDesc.ByteWidth = sizeof(LightMatrixType);
	lightMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightMatrixBufferDesc.MiscFlags = 0;
	lightMatrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightMatrixBufferDesc, NULL, &lightMatrixBuffer);
}
