#include "Projectile.h"

Projectile::Projectile(BaseMesh* mesh, XMFLOAT3 pos_, XMFLOAT3 vel_, float timeout_) : HighLevelMesh(mesh)
{
	timeout = timeout_;
	pos = pos_;
	vel = vel_;
}

bool Projectile::frame(float dt)
{
	// Check timeout
	timeout -= dt;
	if (timeout <= 0)
		return false;

	// Update position
	XM_Math::add_this(pos, XM_Math::multiply(vel, dt));

	// Update world matrix
	XMMATRIX m = XMMatrixTranslation(pos.x, pos.y, pos.z);
	setWorldMatrix(m);

	return true;
}
