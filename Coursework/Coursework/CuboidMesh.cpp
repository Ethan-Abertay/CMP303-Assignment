#include "CuboidMesh.h"

CuboidMesh::CuboidMesh(ID3D11Device* device, int resolution, XMINT3 dimensions)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = ((6 * resolution) * resolution) * 6 * dimensions.x * dimensions.y * dimensions.z;

	indexCount = vertexCount;

	// Create the vertex and index array.
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Vertex variables
	float yincrement = 2.0f / resolution;
	float xincrement = 2.0f / resolution;
	float ystart = 1.0f;
	float xstart = -1.0f;
	//UV variables
	float txu = 0.0f;
	float txv = 0.0f;
	float txuinc = 1.0f / resolution;	// UV increment
	float txvinc = 1.0f / resolution;
	//Counters
	int v = 0;	// vertex counter
	int i = 0;	// index counter

	//front face
	ystart = (dimensions.y);
	xstart = -(dimensions.x);
	for (int y = 0; y < resolution * dimensions.y; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.x; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -dimensions.z);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -dimensions.z);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart, ystart, -dimensions.z);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -dimensions.z);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart - yincrement, -dimensions.z);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -dimensions.z);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			txu += txuinc;
			//ystart -= yincrement;

		}

		ystart -= yincrement;
		xstart = -(dimensions.x);

		txu = 0;
		txv += txvinc;

	}

	txv = 0;

	//back face
	ystart = (dimensions.y);
	xstart = (dimensions.x);
	for (int y = 0; y < resolution * dimensions.y; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.x; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, dimensions.z);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, dimensions.z);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, ystart, dimensions.z);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, dimensions.z);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart - yincrement, dimensions.z);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, dimensions.z);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;

		}

		ystart -= yincrement;
		xstart = (dimensions.x);

		txu = 0;
		txv += txvinc;

	}

	txv = 0;

	//right face
	ystart = (dimensions.y);
	xstart = -(dimensions.z);
	for (int y = 0; y < resolution * dimensions.y; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.z; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(dimensions.x, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(dimensions.x, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(dimensions.x, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(dimensions.x, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(dimensions.x, ystart - yincrement, xstart + xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(dimensions.x, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;

		}

		ystart -= yincrement;
		xstart = -(dimensions.z);
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//left face
	ystart = (dimensions.y);
	xstart = (dimensions.z);
	for (int y = 0; y < resolution * dimensions.y; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.z; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart - yincrement, xstart - xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-dimensions.x, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = (dimensions.z);
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//top face
	ystart = (dimensions.z);
	xstart = -(dimensions.x);
	for (int y = 0; y < resolution * dimensions.z; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.x; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, dimensions.y, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, dimensions.y, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, dimensions.y, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, dimensions.y, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, dimensions.y, ystart - yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, dimensions.y, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -(dimensions.x);
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//bottom face
	ystart = -(dimensions.z);
	xstart = -(dimensions.x);

	for (int y = 0; y < resolution * dimensions.z; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution * dimensions.x; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, -dimensions.y, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -dimensions.y, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, -dimensions.y, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, -dimensions.y, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, -dimensions.y, ystart + yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -dimensions.y, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart += yincrement;
		xstart = -(dimensions.x);
		txu = 0;
		txv += txvinc;
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
