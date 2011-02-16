#include <stdio.h>
#include "PhysicsCart.h"
#include "Vector3d.h"
#include "VectorTools.h"

#define NULL 0


PhysicsCart::PhysicsCart(Vector3d initpos, Vector3d initnorm)
{
	// Set "constants"
	mass = 100;
	Ix = 100;
	friction_static = 0.20;
	friction_glide = 0.10;
	gravityAccel = 9.81;

	// Assign initial values
	v = 0;
	wheelsOffsetx = 0;
	wheelsOffsety = 0.5;		// Total width of cart becomes 1.0

	pos = initpos;
	norm = initnorm;
	track = NULL;


#ifdef DEBUG
	std::cout << "PhysicsCart constructed. \n";
#endif
}


PhysicsCart::~PhysicsCart(void)
{
}

void PhysicsCart::setTrack(Track *track) {
	this->track = track;
}

void PhysicsCart::nextStep(double dt) {
	if (track == NULL) track = new Track(0);	// If no track is set, create a blank track

	// Calculate displacement
	Vector3d delta = vectorTimesScalar(velocity, dt);	// ds = v*dt

	// Current ds for the track (perhaps this one will be constant? That would be good...)
	double ds_estimate = vectorLength( vectorDiff(track->getPos(currentIndex+1), track->getPos(currentIndex)));

	// Check how many segments we will travel during dt, rounded down.
	int deltaIndex = vectorLength(delta)/ds_estimate;
	currentIndex += deltaIndex;

	// Get position when positioned on the new segment
	Vector3d newPos = track->getPos(currentIndex);

	double zDisplacement = vectorDiff(newPos, pos).z;	// displacement in gravity-direction, negative is down
	double zDueToGravity = 0.5*gravityAccel*dt*dt;		// assuming constant acceleration
	bool isInPositiveLoop = vectorDot()



		
}

Vector3d PhysicsCart::getNorm3d() const {
	return norm;
}