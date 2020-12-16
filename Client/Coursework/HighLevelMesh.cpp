#include "HighLevelMesh.h"

HighLevelMesh::HighLevelMesh(BaseMesh* meshPtr)
{
	mesh = meshPtr;
}

HighLevelMesh::~HighLevelMesh()
{
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	// Delete world matrices
	for (int i = 0; i < worldMatrices.size(); i++)
	{
		delete worldMatrices[i];
		worldMatrices[i] = 0;
	}
	worldMatrices.clear();

	//// Delete rotation matrices
	//for (int i = 0; i < rotationMatrices.size(); i++)
	//{
	//	delete rotationMatrices[i];
	//	rotationMatrices[i] = 0;
	//}
	//rotationMatrices.clear();
}

void HighLevelMesh::finalRender(SceneShader* shader, D3D* renderer, SceneShader::ShaderParams& params)
{
	assert(texture);	// Ensure I haven't forgotten to set this

	for (int i = 0; i < worldMatrices.size(); i++)
	{
		params.world = worldMatrices[i];	// Set world matrix
		params.texture = texture;			// Set texture

		mesh->sendData(renderer->getDeviceContext());			// Send mesh data
		shader->setShaderParameters(params);					// Set shader parameterss
		shader->render(renderer->getDeviceContext(), mesh->getIndexCount());	// Render
	}
}

void HighLevelMesh::finalRender(TessellatedHeightMapShader* shader, D3D* renderer, TessellatedHeightMapShader::TessShaderParams& params)
{
	assert(texture);	// Ensure I haven't forgotten to set this
	assert(heightMap);
	assert(normalMap);

	for (int i = 0; i < worldMatrices.size(); i++)
	{
		params.shaderParams->world = worldMatrices[i];	// Set world matrix
		//params.rotationMatrix = rotationMatrices[i];	// Set rotation matrix (for rotating normals from normal map)
		params.shaderParams->texture = texture;			// Set texture
		params.heightMap = heightMap;	// Set height map
		params.normalMap = normalMap;	// Set normal map
		params.maxHeight = &heightMapDisplacement;
		params.maxTessDistance = &maxTessDistance;
		params.maxTessFactor = &maxTessFactor;

		mesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);			// Send mesh data
		shader->setShaderParameters(params);					// Set shader parameterss
		shader->render(renderer->getDeviceContext(), mesh->getIndexCount());	// Render
	}
}

void HighLevelMesh::depthRender(DepthShader* shader, D3D* renderer, XMMATRIX& view, XMMATRIX& projection)
{
	for (int i = 0; i < worldMatrices.size(); i++)
	{
		mesh->sendData(renderer->getDeviceContext());		// Send mesh data
		shader->setShaderParameters(renderer->getDeviceContext(), *worldMatrices[i], view, projection);	// Set shader parameters
		shader->render(renderer->getDeviceContext(), mesh->getIndexCount());	// Render
	}
}

void HighLevelMesh::depthRender(TessHeightMapDepthShader* shader, TessHeightMapDepthShader::TessDepthShaderParams& params, D3D* renderer, XMMATRIX& view, XMMATRIX& projection)
{
	assert(heightMap);

	for (int i = 0; i < worldMatrices.size(); i++)
	{
		params.heightMap = heightMap;	// Set height map
		params.maxHeight = &heightMapDisplacement;
		params.maxTessDistance = &maxTessDistance;
		params.maxTessFactor = &maxTessFactor;

		mesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);		// Send mesh data
		shader->setShaderParameters(params, renderer->getDeviceContext(), *worldMatrices[i], view, projection);	// Set shader parameters
		shader->render(renderer->getDeviceContext(), mesh->getIndexCount());	// Render
	}
}

void HighLevelMesh::colourRender(SimpleColourShader* shader, D3D* renderer, SimpleColourShader::Params& params)
{
	params.world = worldMatrices[0];

	mesh->sendData(renderer->getDeviceContext());			// Send mesh data
	shader->setShaderParameters(params);					// Set shader parameterss
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());	// Render
}
