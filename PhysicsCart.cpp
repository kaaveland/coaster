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
	wheelsOffsety = 0.5;		// Total widTh of cart becomes 1.0
	thrustFactor = brakingFactor = 0.0;

	// Assign initial values
	v = 0;
	vVelocity = Vector3d(0,0,0);
	vAccel = Vector3d(0,0,0);
	currentDistance = 0;
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
	moveTo(0);
}

bool PhysicsCart::hasTrack() const
{
	return this->track != NULL;
}

void PhysicsCart::moveTo(double distance) {
	assert(track != NULL);
	assert(t >= 0 && t <= 1);

	currentDistance = distance;
	vPos = track->getPos(distance);
	vUp = track->getUp(distance);
	vVelocity = track->getTangentVector(distance) * this->v;		// Align travel with track direction

	isFreefalling = false;
}

void PhysicsCart::nextStep(double dT) {
	
	const Vector3d gvector(0,-gravityAccel,0);	// OOPS

	if (track == NULL) track = new Track(0);	// If no track is set, create a blank track
	if (currentDistance < 0 || currentDistance >= track->getNumberOfPoints())
		isFreefalling = true;			// We have gone outside of the track, set to free fall mode.
	
	double ds_estimate = 0.0;
	bool isGoingForward = (track->getTangentVector(currentDistance) * vVelocity >= 0.0);
	int forward = isGoingForward ? 1 : -1;
	assert (isGoingForward);

	// Update position
	if (!isFreefalling) {
		//pos = track->getPos(currentDistance+deltaIndex);				// We "snap" to the start of the segment. NO!
		this->vPos += dT * vVelocity;		// TESTING
				
		// Find the current ds for the track (perhaps this one will be constant? That would be good...)
		ds_estimate = (track->getPos(currentDistance + forward) - track->getPos(currentDistance)).length();
		cout << "ds is " << ds_estimate << "\n";

		// Update velocity
		vVelocity += dT * vAccel;	// velocity = velocity + accel*dT. Align with tangent vector here too?
		v = vVelocity.length();

	} else {
		this->vPos += dT * vVelocity;		// pos = pos + velocity*dT
		vAccel = gvector;

		// Update velocity
		vVelocity += dT * vAccel;	// velocity = velocity + accel*dT
		v = vVelocity.length();
				
		return;			// RETURN --> Free fall. Do nothing more.
	}
	
	// Update up vector -- so far it will always be equal to the track segment's current up vector
	vUp = track->getUp(currentDistance);

	
	// Calculate how many segments we will travel during dT. TODO: interpolate
	Vector3d vDeltaPos = dT * vVelocity;						// ds = v*dT. Note that vVelocity is "updated" once above.
	int deltaIndex = (int)(vDeltaPos.length()/ds_estimate);		// Change of index, always positive
	assert (deltaIndex >= 0);
	//if (deltaIndex < 1) accumDeltaPos += ds_estimate;

	cout << "deltaIndex is " << deltaIndex << "\n";
		
	// ---- Not free-falling --- //
	bool insideCurve = (vUp * track->getNormalVector(currentDistance) >= 0.0);	// true if "inside" curvature
	double a_N = v*v*track->getCurvature(currentDistance);
	cout << "a_N is " << a_N << "\n";



	if (insideCurve) {		// TODO: Simplify
		// If the acceleration required to keep in circular motion is LESS THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		double asdf = v*v*track->getCurvature(currentDistance) - track->getNormalVector(currentDistance)*gvector;
		if (v*v*track->getCurvature(currentDistance) - track->getNormalVector(currentDistance)*gvector < 1e-12)
			isFreefalling=true;
	
	} else {
		// If the acceleration required to keep in circular motion is GREATER THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		if (v*v*track->getCurvature(currentDistance) - track->getNormalVector(currentDistance)*gvector > 0.0) 
			isFreefalling = true;
		
	}

	Vector3d accNormal = a_N * track->getNormalVector(currentDistance);
	
	// a_T = thrust - braking + G_N
	double a_T = thrustFactor*maxThrust/mass 
		- brakingFactor*friction_static*a_N/mass
		+ gvector * (*track).getTangentVector(currentDistance);	// Possibly air resistance
	Vector3d accTangential = a_T * track->getTangentVector(currentDistance);	// When going backwards, perhaps currentDistance-1?
	cout << "a_T is " << a_T << "\n";

	// Update acceleration vector (state)
	vAccel = accNormal + accTangential;

	// Increase section index.
	currentDistance += (forward)*deltaIndex;
}


void PhysicsCart::setSpeed(double v) {
	assert (v >= 0.0);
	
	vVelocity = v * track->getTangentVector(currentDistance);
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
		"Currently on segment with index: " << currentDistance << "\n" <<
		"isFreefalling = " << isFreefalling << "\n" <<
		"Position = [ " << vPos.x << ", " << vPos.y << ", " << vPos.z << "]\n" <<
		"Up vecto = [" << vUp.x << ", " << vUp.y << ", " << vUp.z << "]\n" << 
		"Velocity = [" << vVelocity.x << ", " << vVelocity.y << ", " << vVelocity.z << "], v = " << v << "\n" <<
		"Accelera = [" << vAccel.x << ", " << vAccel.y << ", " << vAccel.z << "]\n" <<
		"--Track info current index--\n" <<
		"Position = [" << track->getPos(currentDistance).x << ", " << track->getPos(currentDistance).y << ", " << track->getPos(currentDistance).z << "]\n" <<
		"Normal v = [" << track->getNormalVector(currentDistance).x << ", " << track->getNormalVector(currentDistance).y << ", " << track->getNormalVector(currentDistance).z << "]\n" <<
		"Tangent  = [" << track->getTangentVector(currentDistance).x << ", " << track->getTangentVector(currentDistance).y << ", " << track->getTangentVector(currentDistance).z << "]\n";
		


	return res.str();
}