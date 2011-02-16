#include <stdio.h>
#include "PhysicsCart.h"
#include "Vector3d.h"
#include "VectorTools.h"

#define NULL 0


PhysicsCart::PhysicsCart(Vector3d initpos, Vector3d initup)
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
	up = initup;
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

	// Calculate displacement if continuing in a straight line
	Vector3d delta = vectorTimesScalar(velocity, dt);	// ds = v*dt

	// Current ds for the track (perhaps this one will be constant? That would be good...)
	double ds_estimate = vectorLength( vectorDiff(track->getPos(currentIndex+1), track->getPos(currentIndex)));

	// Check how many segments we will travel during dt, rounded down (using ds estimate).
	int deltaIndex = (int)(vectorLength(delta)/ds_estimate);
	currentIndex += deltaIndex;
	
	Vector3d newPos = track->getPos(currentIndex);		// Get position when positioned on the new segment
	double zDisplacement = vectorDiff(newPos, pos).z;	// Displacement in gravity-direction, negative is down
	double zDisplacementFreefall = velocity.z*dt + 0.5*gravityAccel*dt*dt;		// z displacement if cart was in free fall
		
	bool hasNormalForce = (vectorDot(up, track->getNormalVector(currentIndex)) >= 0);	// true if "inside" curvature
			
}

Vector3d PhysicsCart::getUp() const {
	return up;
}