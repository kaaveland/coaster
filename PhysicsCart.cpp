#include <stdio.h>
#include "PhysicsCart.h"
#include "Vector3d.h"
#include "VectorTools.h"

#define NULL 0


PhysicsCart::PhysicsCart(Vector3d initpos, Vector3d initup)
{
	// Set "constants"
	mass = 1.0;
	Ix = 1.0;
	friction_static = 1.0;
	friction_glide = 1.0;
	gravityAccel = 10.0;
	maxThrust = 1.0;
	wheelsOffsetx = 0;
	wheelsOffsety = 0.5;		// Total width of cart becomes 1.0

	// Assign initial values
	v = 0;
	velocity = Vector3d(0,0,0);
	accel = Vector3d(0,0,0);

	pos = initpos;
	up = initup;
	isFreefalling = false;
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

	// TODO: TEST THIS FUNCTION OMG AND IMPLEMENT OPERATOR OVERLOADS FOR VECTOR OMGOMG

	// Find the current ds for the track (perhaps this one will be constant? That would be good...)
	Vector3d delta = vectorTimesScalar(velocity, dt);	// ds = v*dt
	double ds_estimate = vectorLength( vectorDiff(track->getPos(currentIndex+1), track->getPos(currentIndex)));

	// Calculate how many segments we will travel during dt, rounded down (using ds estimate). TODO: interpolate
	int deltaIndex = (int)(vectorLength(delta)/ds_estimate);

	// Update position
	if (!isFreefalling) {
		pos = track->getPos(currentIndex+deltaIndex);				// We "snap" to the start of the segment
	} else {
		pos = vectorSum(pos, vectorTimesScalar(velocity, dt));		// pos = pos + velocity*dt, try this one above aswell
	}

	// Update velocity
	velocity = vectorSum(velocity, vectorTimesScalar(accel, dt));	// velocity = velocity + accel*dt
	v = vectorLength(velocity);

	Vector3d gvector(0,0,-gravityAccel);

	if (!isFreefalling) {

		// Check if the cart loses normal force (contact), in which case it will detach from the track
		//bool hasNormalForce = (vectorDot(up, track->getNormalVector(currentIndex)) >= 0);	// true if "inside" curvature
		Vector3d displacementIfFrefall = vectorSum(vectorTimesScalar(velocity, dt), vectorTimesScalar(gvector,dt*dt));	// Accuracy?
		// If this point is not above the track in any way, the cart will stay on the track
		isFreefalling = (vectorDot(displacementIfFrefall, track->getUp(currentIndex)) > 0.0); 

		// a_N = v^2/r = v^2 * kappa.
		double a_N = v*v*track->getCurvature(currentIndex);
		Vector3d accNormal = vectorTimesScalar(track->getNormalVector(currentIndex), a_N);

		// a_T = thrust - braking + G_N
		double a_T = thrustFactor*maxThrust/mass 
			- brakingFactor*friction_static*a_N*mass
			+ vectorDot(gvector, track->getTangentVector(currentIndex));
		Vector3d accTangential = vectorTimesScalar(track->getTangentVector(currentIndex), a_T);

		// Update acceleration vector (state)
		accel = vectorSum(accNormal, accTangential);

	} else {
		accel = gvector;
	}

	currentIndex += deltaIndex;
}

Vector3d PhysicsCart::getUp() const {
	return up;
}