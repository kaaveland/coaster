#include <stdio.h>
#include "PhysicsCart.h"



PhysicsCart::PhysicsCart(double* pos3d, double* norm3d)
{
	pos3d = new double[3];
	norm3d = new double[3];

	// Assign initial values
	v = 0;
	omegax = 0;

#ifdef DEBUG
	std::cout << "PhysicsCart constructed.";
#endif

}


PhysicsCart::~PhysicsCart(void)
{
}