#pragma once
#include "AModel.h"

class PlayerMesh : public AModel
{
public:
	PlayerMesh(ID3D11Device* device, const std::string& file, XMFLOAT3 colour);
	~PlayerMesh();

private:

};

