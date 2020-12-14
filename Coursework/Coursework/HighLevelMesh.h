#pragma once
#include "BaseMesh.h"
#include "SceneShader.h"
#include "DepthShader.h"
#include "TessellatedHeightMapShader.h"
#include "TessHeightMapDepthShader.h"
#include <vector>

using std::vector;

// High level is almost a misnomer
// This class adds a bit of functionality to any mesh that inherits from base mesh
class HighLevelMesh
{
public:
	HighLevelMesh(BaseMesh* meshPtr);
	~HighLevelMesh();

	// Render functions
	void finalRender(SceneShader* shader, D3D* renderer, SceneShader::ShaderParams& params);
	void finalRender(TessellatedHeightMapShader* shader, D3D* renderer, TessellatedHeightMapShader::TessShaderParams& params);
	void depthRender(DepthShader* shader, D3D* renderer, XMMATRIX& view, XMMATRIX& projection);
	void depthRender(TessHeightMapDepthShader* shader, TessHeightMapDepthShader::TessDepthShaderParams &params, D3D* renderer, XMMATRIX& view, XMMATRIX& projection);

	// Getters
	float* getHeightMapDisplacement_ptr() { return &heightMapDisplacement; };
	int* getMaxTessDistance_ptr() { return &maxTessDistance; };
	int* getMaxTessFactor_ptr() { return &maxTessFactor; };

	// Setters
	void addWorldMatrix(XMMATRIX m) { worldMatrices.push_back(new XMMATRIX(m)); };
	//void addRotationMatrix(XMMATRIX m) { rotationMatrices.push_back(new XMMATRIX(m)); };
	void setTexture(ID3D11ShaderResourceView* t) { texture = t; };
	void setHeightmap(ID3D11ShaderResourceView* t) { heightMap = t; };
	void setNormalMap(ID3D11ShaderResourceView* t) { normalMap = t; };
	void setHeightMapDisplacementMax(float f) { heightMapDisplacement = f; };
	void setMaxTessFactor(int i) { maxTessFactor = i; };
	void setMaxTessDistance(int i) { maxTessDistance = i; };

private:
	BaseMesh* mesh = 0;

	vector<XMMATRIX*> worldMatrices;
	//vector<XMMATRIX*> rotationMatrices;
	ID3D11ShaderResourceView* texture = 0;
	ID3D11ShaderResourceView* heightMap = 0;
	ID3D11ShaderResourceView* normalMap = 0;

	float heightMapDisplacement = 0;
	int maxTessDistance = 0;
	int maxTessFactor = 0;
};

