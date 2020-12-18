#include "Player.h"

Player::Player(FPCamera* cam_ptr)
{
	camera = cam_ptr;
}

Player::~Player()
{

}

void Player::frame(float dt)
{
	if (fireDelayTimer > 0)
		fireDelayTimer -= dt;
}

bool Player::canFire()
{
	if (fireDelayTimer <= 0)
		return true;
	return false;
}

XMFLOAT3 Player::getFireVelocity(XMFLOAT3 input)
{
	return XM_Math::multiply(XM_Math::normalise(input), fireVelocity);
}
