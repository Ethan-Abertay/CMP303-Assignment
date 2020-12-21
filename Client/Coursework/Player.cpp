#include "Player.h"
#include "NetworkManager.h"

Player::Player(FPCamera* cam_ptr)
{
	camera = cam_ptr;
	camera->setPlayer(this);
}

Player::~Player()
{

}

void Player::init(NetworkManager* network_ptr)
{
	networkManager = network_ptr;
}

void Player::frame(float dt)
{
	if (fireDelayTimer > 0)
		fireDelayTimer -= dt;
}

void Player::shoot(XMFLOAT3 position, XMFLOAT3 forward)
{
	networkManager->shoot(position, forward);
}

bool Player::canFire()
{
	if (fireDelayTimer <= 0)
	{
		fireDelayTimer = fireDelay;
		return true;
	}
	return false;
}

XMFLOAT3 Player::getFireVelocity(XMFLOAT3 input)
{
	return XM_Math::multiply(XM_Math::normalise(input), fireVelocity);
}
