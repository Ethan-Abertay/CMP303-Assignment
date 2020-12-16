#include "TessellatablePlane.h"

TessellatablePlane::TessellatablePlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT2 dimensions_, XMINT2 res, XMFLOAT2 texRes)
{
	dimensions = dimensions_;
	resolution = res;
	texResolution = texRes;
	initBuffers(device);
}

TessellatablePlane::~TessellatablePlane()
{
	BaseMesh::~BaseMesh();	// Is this not automatically done anyway?
}

void TessellatablePlane::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(top);
}

void TessellatablePlane::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = resolution.x * resolution.y;
	indexCount = (resolution.x - 1) * (resolution.y - 1) * 4;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Start at zero and move to the max
	const float xMax = dimensions.x;
	const float zMax = dimensions.y;
	const float xInc = xMax / (float)(resolution.x - 1);
	const float zInc = zMax / (float)(resolution.y - 1);
	const float uInc = (float)texResolution.x / (float)(resolution.x - 1);
	const float vInc = (float)texResolution.y / (float)(resolution.y - 1);
	float x = 0, z = 0;
	float u = 0, v = 0;

	// Convert 2D coords into array index
	auto getIndex = [&](int i, int j) -> int
	{
		return (i * resolution.x) + j;
	};

	// Load the vertex array with data (create grid of vertices)
	for (int i = 0; i < resolution.y; i++)
	{
		for (int j = 0; j < resolution.x; j++)
		{
			// Get index
			int index = getIndex(i, j);

			// Create vertex 
			vertices[index].position = XMFLOAT3(x, 0.f, z);
			vertices[index].normal = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[index].texture = XMFLOAT2(u, v);

			// Increment x and u
			x += xInc;
			u += uInc;
		}

		// Reset x and u
		x = 0;
		u = 0;

		// Increment z and v
		z += zInc;
		v += vInc;
	}

	// Load the index array with data (specify the squares)
	int index = 0;
	for (int i = 0; i < resolution.y - 1; i++)
	{
		for (int j = 0; j < resolution.x - 1; j++)
		{
			indices[index] = getIndex(i, j);
			index++;

			indices[index] = getIndex(i, j + 1);
			index++;

			indices[index] = getIndex(i + 1, j + 1);
			index++;

			indices[index] = getIndex(i + 1, j);
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}


