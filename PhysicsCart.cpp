#include <stdio.h>
#include "PhysicsCart.h"
#include "Track.h"
#include "Vector3d.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

#define NULL 0
#define WILLFLYTOLERANCE 1d-12

using std::stringstream;
using std::cout;

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
	currentIndex = 0;
	accumDeltaPos = 0.0;

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
	moveToIndex(0);
}

void PhysicsCart::moveToIndex(int index) {
	assert(track != NULL);
	assert(index >= 0 && index < track->getNumberOfPoints());

	currentIndex = index;
	vPos = track->getPos(index);
	vUp = track->getUp(index);
	vVelocity = track->getTangentVector(index) * this->v;		// Align travel with track direction

	isFreefalling = false;
}

void PhysicsCart::nextStep(double dt) {
	
	const Vector3d gvector(0,-gravityAccel,0);	// OOPS

	if (track == NULL) track = new Track(0);	// If no track is set, create a blank track
	if (currentIndex < 0 || currentIndex >= track->getNumberOfPoints())
		isFreefalling = true;			// We have gone outside of the track, set to free fall mode.
	
	double ds_estimate = 0.0;
	bool isGoingForward = (track->getTangentVector(currentIndex) * vVelocity >= 0.0);
	int forward = isGoingForward ? 1 : -1;
	assert (isGoingForward);

	// Update position
	if (!isFreefalling) {
		//pos = track->getPos(currentIndex+deltaIndex);				// We "snap" to the start of the segment. NO!
		this->vPos += dt * vVelocity;		// TESTING
				
		// Find the current ds for the track (perhaps this one will be constant? That would be good...)
		ds_estimate = (track->getPos(currentIndex + forward) - track->getPos(currentIndex)).length();
		cout << "ds is " << ds_estimate << "\n";

		// Update velocity
		vVelocity += dt * vAccel;	// velocity = velocity + accel*dt. Align with tangent vector here too?
		v = vVelocity.length();

	} else {
		this->vPos += dt * vVelocity;		// pos = pos + velocity*dt
		vAccel = gvector;

		// Update velocity
		vVelocity += dt * vAccel;	// velocity = velocity + accel*dt
		v = vVelocity.length();
				
		return;			// RETURN --> Free fall. Do nothing more.
	}
	
	// Update up vector -- so far it will always be equal to the track segment's current up vector
	vUp = track->getUp(currentIndex);

	
	// Calculate how many segments we will travel during dt. TODO: interpolate
	Vector3d vDeltaPos = dt * vVelocity;						// ds = v*dt. Note that vVelocity is "updated" once above.
	int deltaIndex = (int)(vDeltaPos.length()/ds_estimate);		// Change of index, always positive
	assert (deltaIndex >= 0);
	//if (deltaIndex < 1) accumDeltaPos += ds_estimate;

	cout << "deltaIndex is " << deltaIndex << "\n";
		
	// ---- Not free-falling --- //
	bool insideCurve = (vUp * track->getNormalVector(currentIndex) >= 0.0);	// true if "inside" curvature
	double a_N = v*v*track->getCurvature(currentIndex);
	cout << "a_N is " << a_N << "\n";



	if (insideCurve) {		// TODO: Simplify
		// If the acceleration required to keep in circular motion is LESS THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		double asdf = v*v*track->getCurvature(currentIndex) - track->getNormalVector(currentIndex)*gvector;
		if (v*v*track->getCurvature(currentIndex) - track->getNormalVector(currentIndex)*gvector < 1e-12)
			isFreefalling=true;
	
	} else {
		// If the acceleration required to keep in circular motion is GREATER THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		if (v*v*track->getCurvature(currentIndex) - track->getNormalVector(currentIndex)*gvector > 0.0) 
			isFreefalling = true;
		
	}

	Vector3d accNormal = a_N * track->getNormalVector(currentIndex);
	
	// a_T = thrust - braking + G_N
	double a_T = thrustFactor*maxThrust/mass 
		- brakingFactor*friction_static*a_N/mass
		+ gvector * (*track).getTangentVector(currentIndex);	// Possibly air resistance
	Vector3d accTangential = a_T * track->getTangentVector(currentIndex);	// When going backwards, perhaps currentIndex-1?
	cout << "a_T is " << a_T << "\n";

	// Update acceleration vector (state)
	vAccel = accNormal + accTangential;

	// Increase section index.
	currentIndex += (forward)*deltaIndex;
}


void PhysicsCart::setSpeed(double v) {
	assert (v >= 0.0);
	
	vVelocity = v * track->getTangentVector(currentIndex);
	this->v = v;
}

Vector3d PhysicsCart::getPos() const {
	return vPos;
}

Vector3d PhysicsCart::getUp() const {
	return vUp;
}

string PhysicsCart::toString() const {
	stringstream res;
	res << "--PhysicsCart info--\n" << 
		"Currently on segment with index: " << currentIndex << "\n" <<
		"isFreefalling = " << isFreefalling << "\n" <<
		"Position = [ " << vPos.x << ", " << vPos.y << ", " << vPos.z << "]\n" <<
		"Up vecto = [" << vUp.x << ", " << vUp.y << ", " << vUp.z << "]\n" << 
		"Velocity = [" << vVelocity.x << ", " << vVelocity.y << ", " << vVelocity.z << "], v = " << v << "\n" <<
		"Accelera = [" << vAccel.x << ", " << vAccel.y << ", " << vAccel.z << "]\n" <<
		"--Track info current index--\n" <<
		"Position = [" << track->getPos(currentIndex).x << ", " << track->getPos(currentIndex).y << ", " << track->getPos(currentIndex).z << "]\n" <<
		"Normal v = [" << track->getNormalVector(currentIndex).x << ", " << track->getNormalVector(currentIndex).y << ", " << track->getNormalVector(currentIndex).z << "]\n" <<
		"Tangent  = [" << track->getTangentVector(currentIndex).x << ", " << track->getTangentVector(currentIndex).y << ", " << track->getTangentVector(currentIndex).z << "]\n";
		


	return res.str();
}