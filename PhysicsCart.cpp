#include <stdio.h>
#include "PhysicsCart.h"
#include "Track.h"
#include "Vector3d.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

#define NULL 0
#define WILLFLYTOLERANCE 1e-12
#define MAXDELTATIME 1e-1
#define MAXDELTATIME_LAG 1e-1
#define SPEEDCUTOFF_FRICTION 1e-6
#define NULLVECTOR Vector3d(0,0,0)

using std::stringstream;
using std::cout;

int inline signum(double d) {
	return (int)((d > 0) - (d < 0));
}

PhysicsCart::PhysicsCart() : gvector(0,-9.81,0)
{
	// Set "constants"
	mass = 1.0;
	Ix = 1.0;
	friction_static = 1.0;
	friction_glide = 1.0;
	maxThrust = 500.0;
	maxBreak = 500.0;
	wheelsOffsetx = 0;
	wheelsOffsety = 0.5;		// Total widTh of cart becomes 1.0
	thrustFactor = 0.0;
	brakingFactor = 0.0;

	// Assign initial values
	v = 0;
	vVelocity = Vector3d(0,0,0);
	vAccel = Vector3d(0,0,0);
	vForward = Vector3d(0,0,0);
	currentDistance = 0;
	current_t = 0;

		
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
	// NOTE: this function does a crude approximation!
	assert(track != NULL);
	if (distance < 0.0) distance = 0.0;
	if (distance > track->getTrackLength()) distance = track->getTrackLength();

	// Snapping to closest control point
	double t = (double)distance/track->getTrackLength();
	currentDistance = track->getArcLengthToControlPoint(t);
	current_t = t;

	vPos = track->getPos(t);
	vUp = track->getUp(t);
	vVelocity = Vector3d(0,0,0);
	vForward = track->getTangentVector(t);
	v = 0;
	vAccel = Vector3d(0,0,0);
	
	isFreefalling = false;
}

// Calculates a_T. delta_distance specifies the distance forward or backward to calculate for. If delta_distance is 0.0, 
// a_T will be calculated at the PhysicsCart's current distance.
double PhysicsCart::calculate_a_T(double deltaDistance) const
{	
	double new_t = current_t + track->deltaDistanceTodeltaT(deltaDistance, current_t);
	int direction = 0;
	/* if (abs(v) > SPEEDCUTOFF_FRICTION )*/
	direction = (int)(v/abs(v));	// Positive if going forward on track
	
	double a_T = thrustFactor*maxThrust/mass 
		- direction*brakingFactor*maxBreak
		+ gvector * track->getTangentVector(new_t);	// Possibly air resistance

	return a_T;
}

double PhysicsCart::calculate_a_N(double deltaDistance) const
{
	double new_t = track->deltaDistanceTodeltaT(deltaDistance, current_t);
	return v*v*track->getCurvature(new_t);
}

void PhysicsCart::nextStep(double dT)
{
	assert(track != NULL);

	if (dT > MAXDELTATIME_LAG) return;
	calculateNextStep(dT); return;

	int repeats = (int)(dT / MAXDELTATIME) + 1;
	for (double i = 1; i <= repeats; i++)
		calculateNextStep(dT * (i/repeats));
}

void PhysicsCart::calculateNextStep(double dT) {
		
	//cout << toString() << "Moving " << dT << " seconds.\nCurrent distance: " << currentDistance << " / " << track->getTrackLength() << "\n";

	if (isFreefalling) {
		//vAccel = gvector;
		vPos +=  vVelocity * dT; // + 0.5 * vAccel * dT * dT;
		vVelocity += gvector * dT;
		return;
	}
	
	if (currentDistance < 0.0 || currentDistance >= track->getTrackLength())
		isFreefalling = true;			// We have gone outside of the track, set to free fall mode
	
	//const double trackDeltaDistance = track->getDelta();
	//const double current_t = track->metersToT(currentDistance);

	int direction = 1; //signum(vVelocity * track->getTangentVector(current_t));
	double delta_distance_predicted = dT * v;
	//assert (delta_distance_predicted < track->getSmoothedDelta() * track->getSmoothingValue());	
	// If not, we will get varying speeds etc. TODO: use minimum segment length or max possible distance per time step to get smoot response
	
	if (currentDistance + direction*delta_distance_predicted < 0.0 || 
		currentDistance + direction*delta_distance_predicted > track->getTrackLength()) { 
			isFreefalling = true; 
			return;
		}	// Out of the track
	
	double a_T = 0.5 * (calculate_a_T(0.0) + calculate_a_T(direction*delta_distance_predicted));	// Heun's method
	double a_N = 0.5 * (calculate_a_N(0.0) + calculate_a_N(direction*delta_distance_predicted));	//
	double delta_distance_corrected = (dT * v + 0.5 * a_T * dT*dT);
	v = v + a_T * dT;
	
	// Update distance and current t value
	current_t += track->deltaDistanceTodeltaT(delta_distance_corrected, current_t);
	currentDistance += delta_distance_corrected;
	
	// Snap to track (position, direction, up and forward)
	vPos = track->getPos(current_t);
	vVelocity = v*track->getTangentVector(current_t);
	if (track->getUp(current_t) != NULLVECTOR)		// If the up vector goes crazy singular, don't change it!
		vUp = track->getUp(current_t);
	else assert(false);

	vForward = track->getTangentVector(current_t);
				
	bool insideCurve = (vUp * track->getNormalVector(current_t) >= 0.0);	// true if "inside" curvature
	// cout << "a_N is " << a_N << "\n";
	//printf("Curvature: %lf, up dot normal: %lf \n", track->getCurvature(current_t), vUp * track->getNormalVector(current_t));
	//printf("v: %lf \n", v);
		
	if (insideCurve) {		// TODO: Simplify
		// If the acceleration required to keep in circular motion is LESS THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		double asdf = v*v*track->getCurvature(current_t) - track->getNormalVector(current_t)*gvector;
		if (v*v*track->getCurvature(current_t) - track->getNormalVector(current_t)*gvector < 1e-12)
			// Disabled for now
			//isFreefalling = true;
			;
	
	} else {
		// If the acceleration required to keep in circular motion is GREATER THAN the gravity component towards the
		//  center of curvature, the cart will lose traction and fly! Should probably have a tolerance here (vertical attitudes).
		double asdf = v*v*track->getCurvature(current_t) - track->getNormalVector(current_t)*gvector;
		if (v*v*track->getCurvature(current_t) - track->getNormalVector(current_t)*gvector > 1e-12) 
			// Disabled for now
			// isFreefalling = true;
			;
	}
}

double PhysicsCart::getSpeed() {
	return this->v;
}

void PhysicsCart::setSpeed(double v) {
	this->v = v;
}

void PhysicsCart::setBraking(double factor) {
	brakingFactor = factor;
}

void PhysicsCart::setThrust(double factor) {
	thrustFactor = factor;
}

Vector3d PhysicsCart::getPos() const {
	return vPos;
}

Vector3d PhysicsCart::getUp() const {
	if (!isFreefalling) return vUp;
	return vUp;
}

Vector3d PhysicsCart::getForward() const {
	if (!isFreefalling) return vForward;
	return vForward;
	//return vVelocity/vVelocity.length();
	// TODO: angular velocity and calculate forward in freefall

}

string PhysicsCart::toString() const {
	stringstream res;
	res << "--PhysicsCart info--\n" << 
		"Currently on distance: " << currentDistance << "\n" <<
		"current_t = " << current_t << "\n" <<
		"isFreefalling = " << isFreefalling << "\n" <<
		"Position = [ " << vPos.x << ", " << vPos.y << ", " << vPos.z << "]\n" <<
		"Up vecto = [" << vUp.x << ", " << vUp.y << ", " << vUp.z << "]\n" << 
		"Velocity = [" << vVelocity.x << ", " << vVelocity.y << ", " << vVelocity.z << "], v = " << v << "\n" <<
		"Accelera = [" << vAccel.x << ", " << vAccel.y << ", " << vAccel.z << "]\n" <<
		"--Track info current index--\n" <<
		"Position = [" << track->getPos(current_t).x << ", " << track->getPos(current_t).y << ", " << track->getPos(current_t).z << "]\n" <<
		"Normal v = [" << track->getNormalVector(current_t).x << ", " << track->getNormalVector(current_t).y << ", " << track->getNormalVector(current_t).z << "]\n" <<
		"Tangent  = [" << track->getTangentVector(current_t).x << ", " << track->getTangentVector(current_t).y << ", " << track->getTangentVector(current_t).z << "]\n" <<
		"Forward = [" << getForward().x << ", " << getForward().y << ", " << getForward().z << "]\n" <<
		"Curvature = " << track->getCurvature(current_t) << "\n" <<
		"Tangent(t) dot NonNormalizedNormal(t) = " << track->getTangentVector(current_t) * track->getNonNormalizedNormalVector(current_t) << "\n";
		
	return res.str();
}
