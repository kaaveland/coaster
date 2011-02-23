#include <stdio.h>
#include "PhysicsCart.h"
#include "Vector3d.h"

#define NULL 0


PhysicsCart::PhysicsCart()
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
	thrustFactor = brakingFactor = 0.0;

	// Assign initial values
	v = 0;
	vVelocity = Vector3d(0,0,0);
	vAccel = Vector3d(0,0,0);

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
	setTrackIndex(0);
}

void PhysicsCart::setTrackIndex(int index) {
	currentIndex = index;
	vPos = track->getPos(index);
	vUp = track->getUp(index);
}

void PhysicsCart::nextStep(double dt) {
	if (track == NULL) track = new Track(0);	// If no track is set, create a blank track

	// TODO: TEST THIS FUNCTION OMG AND IMPLEMENT OPERATOR OVERLOADS FOR VECTOR OMGOMG

	// Find the current ds for the track (perhaps this one will be constant? That would be good...)
	double ds_estimate = (track->getPos(currentIndex+1) - track->getPos(currentIndex)).length();

	// Calculate how many segments we will travel during dt. TODO: interpolate
	Vector3d vDeltaPos = dt * vVelocity;						// ds = v*dt
	double deltaIndex = vDeltaPos.length()/ds_estimate;			// We use a float as index in order to interpolate later
	
	// Update position
	if (!isFreefalling) {
		//pos = track->getPos(currentIndex+deltaIndex);				// We "snap" to the start of the segment. NO!
		this->vPos += dt * vVelocity;		// TESTING
	} else {
		this->vPos += dt * vVelocity;		// pos = pos + velocity*dt, try this one above aswell
	}

	// Update velocity
	vVelocity += dt * vAccel;	// velocity = velocity + accel*dt
	v = vVelocity.length();

	Vector3d gvector(0,0,-gravityAccel);

	if (!isFreefalling) {

		// Check if the cart loses normal force (contact), in which case it will detach from the track
		
		Vector3d displacementIfFrefall = dt * vVelocity +  dt*dt*gvector;		// Accuracy?
		// If this point is not above the track in any way, the cart will stay on the track
		isFreefalling = (displacementIfFrefall * track->getUp(currentIndex) > 0.0); 

		bool hasNormalForce = (vUp * track->getNormalVector(currentIndex) >= 0.0);	// true if "inside" curvature
		double a_N;
		if (hasNormalForce) {
			// a_N = v^2/r = v^2 * kappa.
			a_N = v*v*track->getCurvature(currentIndex);
		} else {
			a_N = 1.0;
		}


		Vector3d accNormal = a_N * track->getNormalVector(currentIndex);

		// a_T = thrust - braking + G_N
		double a_T = thrustFactor*maxThrust/mass 
			- brakingFactor*friction_static*a_N/mass
			+ gvector * track->getTangentVector(currentIndex);
		Vector3d accTangential = a_T * track->getTangentVector(currentIndex);

		// Update acceleration vector (state)
		vAccel = accNormal + accTangential;

	} else {
		vAccel = gvector;
	}

	currentIndex += (int)deltaIndex;
}

Vector3d PhysicsCart::getPos() const {
	return vPos;
}

Vector3d PhysicsCart::getUp() const {
	return vUp;
}