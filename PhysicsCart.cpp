#include <stdio.h>
#include "PhysicsCart.h"
#include "Vector3d.h"


PhysicsCart::PhysicsCart(double* pos3d, double* norm3d)
{
	// Set "constants"
	mass = 100;
	Ix = 100;
	friction_static = 0.20;
	friction_glide = 0.10;

	// Assign initial values
	v = 0;
	//velocity.x = velocity.y = velocity.z = 0;
	//angularVelocity.x = angularVelocity.y = angularVelocity.z = 0;
	//pos.x = pos3d[0]; pos.y = pos3d[1]; pos.z = pos3d[2];
	//norm.x = norm3d[0]; norm.y = norm3d[1]; norm.z = norm3d[2];


#ifdef DEBUG
	std::cout << "PhysicsCart constructed. \n";
#endif
}


PhysicsCart::~PhysicsCart(void)
{
}

void PhysicsCart::nextStep(double dt) {
	
}

Vector3d PhysicsCart::getNorm3d() const {
	return norm;
}