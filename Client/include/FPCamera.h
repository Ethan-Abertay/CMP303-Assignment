#pragma once

#include "camera.h"
#include "input.h"

using namespace DirectX;

class Player;

class FPCamera : public Camera
{
public:
	/*void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}*/

	FPCamera(Input* in, int width, int height, HWND hnd);	///< Initialised default camera object
	//~FPCamera();

	void move(float dt);	///< Move camera, handles basic camera movement

	void setPlayer(Player* ptr) { player = ptr; };

	XMFLOAT3 getForwardVector();

	// Public variables
	bool bWalkMode = true;

private:
	Input* input;
	int winWidth, winHeight;///< stores window width and height
	int deltax, deltay;		///< for mouse movement
	POINT cursor;			///< Used for converting mouse coordinates for client to screen space
	HWND wnd;				///< handle to the window

	float floorHeight = 0.f;
	float playerHeight = 2.f;
	float walkSpeed = 7.5f;
	float runSpeed = 12.5f;
	float fallSpeed = 20.f;
	float fallAcceleration = 0.f;
	float jumpAcceleration = -10.f;
	bool bJumping = false;

	Player* player;
};