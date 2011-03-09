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
#define MAXDELTATIME 1.0

using std::stringstream;
using std::cout;

double inline signum(double d) {
	return ((d > 0) - (d < 0));
}

PhysicsCart::PhysicsCart()
{
	// Set "constants"
	mass = 1.0;
	Ix = 1.0;
	friction_static = 1.0;
	friction_glide = 1.0;
	maxThrust = 1.0;
	wheelsOffsetx = 0;
	wheelsOffsety = 0.5;		// Total widTh of cart becomes 1.0
	thrustFactor = brakingFactor = 0.0;
	brakingFactor = 0.01;

	// Assign initial values
	v = 0;
	vVelocity = Vector3d(0,0,0);
	vAccel = Vector3d(0,0,0);
	currentDistance = 0;

	gvector = Vector3d(0,-10,0);
	
	isFreefalling = false;
	track = NULL;

	step = 0;

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
	assert(distance >= 0 && distance <= 1);

	currentDistance = distance;
	vPos = track->getPos(distance);	// FIX
	vUp = track->getUp((int)distance);	// FIX
	
	vVelocity = Vector3d(0,0,0);
	v = 0;
	vAccel = Vector3d(0,0,0);
	
	isFreefalling = false;
}

// Calculates a_T. delta_distance specifies the distance forward or backward to calculate for. If delta_distance is 0.0, 
// a_T will be calculated at the PhysicsCart's current distance.
double PhysicsCart::calculate_a_T(double deltaDistance) const
{	
	double new_t = track->metersToT(currentDistance + deltaDistance);
	double a_T = thrustFactor*maxThrust/mass 
		- brakingFactor*friction_static/mass
		+ gvector * track->getTangentVector(new_t);	// Possibly air resistance

	return a_T;
}

double PhysicsCart::calculate_a_N(double deltaDistance) const
{
	double new_t = track->metersToT(currentDistance + deltaDistance);
	return v*v*track->getCurvature(new_t);
}

void PhysicsCart::nextStep(double dT) {
	assert(track != NULL);
	if (dT >= MAXDELTATIME) return;
	cout << toString() << "\n";


	if (currentDistance < 0.0 || currentDistance >= track->getTrackLength())
		isFreefalling = true;			// We have gone outside of the track, set to free fall mode


	if (isFreefalling) {
		vAccel = gvector;
		vPos +=  vVelocity * dT + 0.5 * vAccel * dT * dT;
		vVelocity += vAccel * dT;
		v = vVelocity.length();

		return;
	}

	//const double trackDeltaDistance = track->getDelta();
	const double current_t = track->metersToT(currentDistance);

	int direction = 1; //signum(vVelocity * track->getTangentVector(current_t));
	double delta_distance_predicted = dT * v;
	
	if (currentDistance + direction*delta_distance_predicted < 0.0 || 
		currentDistance + direction*delta_distance_predicted > track->getTrackLength()) { 
			isFreefalling = true; 
			return;
		}	// Out of the track
	
	double a_T = 0.5 * (calculate_a_T(0.0) + calculate_a_T(direction*delta_distance_predicted));	// Heun's method
	double a_N = 0.5 * (calculate_a_N(0.0) + calculate_a_N(direction*delta_distance_predicted));	//
	double delta_distance_corrected = (dT * v + 0.5 * a_T * dT*dT);
	v = v + a_T * dT;
	
	// Snap to track (direction and position)
	vVelocity = v * track->getTangentVector(currentDistance + delta_distance_corrected);
	vPos = track->getPos(currentDistance + delta_distance_corrected);
	currentDistance += delta_distance_corrected;
			
	bool insideCurve = (vUp * track->getNormalVector(currentDistance) >= 0.0);	// true if "inside" curvature
	// cout << "a_N is " << a_N << "\n";
		
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
}


void PhysicsCart::setSpeed(double v) {

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
		"Tangent  = [" << track->getTangentVector(currentDistance).x << ", " << track->getTangentVector(currentDistance).y << ", " << track->getTangentVector(currentDistance).z << "]\n" <<
		"Curvature = " << track->getCurvature(currentDistance) << "\n";
		


	return res.str();
}
