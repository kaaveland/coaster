#include <stdio.h>
#include "PhysicsCart.h"



PhysicsCart::PhysicsCart(double* pos3d, double* norm3d)
{
	// Set "constants"
	mass = 100;
	Ix = 100;
	friction_static = 0.20;
	friction_glide = 0.10;

	// Assign initial values
	v = 0;
	omegax = 0;

	this->pos3d = pos3d;	// Copy values?
	this->norm3d = norm3d;


#ifdef DEBUG
	std::cout << "PhysicsCart constructed. \n";
#endif

}


PhysicsCart::~PhysicsCart(void)
{
}